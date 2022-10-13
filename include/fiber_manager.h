#pragma once
#ifndef FIBERS_FIBER_MANAGER_H
#define FIBERS_FIBER_MANAGER_H

#include <thread>
#include <vector>
#include <mutex>
#include <future>
#include <xutility>
#include <map>

#include "fiber.h"
#include "free_list_allocator.h"
#include "job.h"
#include "third_party/moodycamel/concurrentqueue.h"
#include "io_operation.h"
#include "hardware_thread.h"
#include "primitives/spinlock.h"
#include "thread_map.h"
#include "fiber_thread.h"

namespace fibers
{
    struct job;

    template <typename T>
    using queue = moodycamel::ConcurrentQueue<T>;


    // Centralized class for managing the fiber system.
    class fiber_manager
    {
        friend struct job;

    public:

        static fiber_manager& instance() {
            static fiber_manager instance;
            return instance;
        }

        void initialize(size_t fibers = 200, size_t stack_size = 4096);

        template<typename Ret, typename... Args>
        job&& queue_job(std::function<Ret(Args...)> function, Args... args) {

            job job_handle{};

            // This function moves the promise, function, and argument pack to the lifetime of the lambda function.
            // This allows us to execute this function on a fiber, ensuring required captures
            // remain in scope, as well as ensuring that the requested result is set.
            auto func = [job_result_ptr = &job_handle.result,
                         func = function, args = std::make_tuple(std::forward<Args>(args) ...)]
                    () -> void {
                return std::apply([&](auto&& ... args) {
                    void* result = reinterpret_cast<void*>(func(args...));
                    if (job_result_ptr != nullptr)
                    {
                        *job_result_ptr = result;
                    }
                }, std::move(args));
            };

            // Create fiber and add to the ready queue
            auto* f = new fiber();
            f->function = func;
            ready_queue.enqueue(f);

            return std::move(job_handle);
        }

        void yield_current_fiber();

        template <class Rep, class Period>
        void sleep_current_fiber(const std::chrono::duration<Rep, Period>& duration);




        fiber_manager(const fiber_manager&)= delete;
        fiber_manager& operator=(const fiber_manager&)= delete;

    private:
        fiber_manager() = default;
        ~fiber_manager() {
            shutdown();
        };

        static size_t get_this_thread_id();

        void shutdown();
        static void create_threads();

        static void register_fiber_thread(fiber_thread* thread);

        static void run_fiber_thread(fiber_thread* thread);
        static void run_sleep_thread(hardware_thread* thread);
        static void run_io_thread(hardware_thread* thread);

        // This function will serve as the entry point for a job. A pointer to this function is what we
        // will give our context upon initial execution.
        static void run_fiber(fiber* fiber_to_run);

        static hardware_thread* get_current_thread();
        static fiber_thread* get_current_fiber_thread();
        static fiber* get_current_fiber();

        size_t fiber_count = 0;
        size_t stack_size = 0;


        // Allocator for allocating the stack for each fiber.
        free_list_allocator* stack_allocator{};

        // Lock for the stack_allocator. Necessary since multiple fiber execution threads will need to access.
        spinlock allocator_lock;

        std::atomic<bool> threads_registered = false;

        // Map of fiber threads. Used for determining fiber context. Writing locks threads, reading is lock free.
        thread_map threads;

        // Thead for handling sleep operations.
        hardware_thread* sleep_thread;

        // ID of the sleep thread.
        size_t sleep_thread_id;

        // Thread for handling IO operations.
        hardware_thread* io_thread;

        // ID of the IO thread.
        size_t io_thread_id;

        // Queue of fibers ready to be executed on any thread
        // Producers: Any thread.
        // Consumers: Any hardware_thread.
        queue<fiber*> ready_queue;

        // Queue of io_operations to be performed by the IO thread
        // Producers: Any hardware_thread.
        // Consumers: io_thread.
        queue<io_operation> io_queue;

        // Queue of fibers requesting to be put to sleep.
        // Producers: Any hardware_thread.
        // Consumers: sleep_thread (Sleep management thread).
        queue<fiber*> sleep_queue;


        // Queue of fiber IDs to wake up.
        // Producers: io_thread
        // Consumers: sleep_thread
        queue<size_t> wake_queue;

        // List of currently sleeping fibers.
        // This structure is NOT concurrent, only meant to be accessed by the sleep management thread.
        std::vector<fiber*> sleeping_fibers;


    };

}

#endif
