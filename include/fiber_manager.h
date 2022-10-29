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
#include "job_handle.h"
#include "third_party/moodycamel/concurrentqueue.h"
#include "io_operation.h"
#include "hardware_thread.h"
#include "primitives/spinlock.h"
#include "thread_map.h"
#include "fiber_thread.h"
#include "job.h"
#include "priority.h"

namespace fibers
{
    struct job_handle;

    template <typename T>
    using queue = moodycamel::ConcurrentQueue<T>;


    // Centralized class for managing the fiber system.
    class fiber_manager
    {
        friend struct job_handle;

    public:

        // Meyers singleton (guaranteed to be thread-safe by cpp standard)
        static fiber_manager& instance() {
            static fiber_manager instance;
            return instance;
        }

        template<typename StackAllocator = free_list_allocator>
        void initialize(size_t num_fibers = 200, size_t fiber_stack_size = 4096)
        {
            static_assert(std::is_base_of_v<fiber_allocator, StackAllocator> && !std::is_abstract_v<StackAllocator>,
                    "Provided allocator does not derive from fiber_allocator!");
            initialize(num_fibers, fiber_stack_size, new StackAllocator(fiber_stack_size * num_fibers));

        }

        template<typename Ret, typename... Args>
        job_handle&& queue_job(std::function<Ret(Args...)> function, Args... args, priority job_priority = priority::normal) {

            job_handle handle{};

            // This function moves the promise, function, and argument pack to the lifetime of the lambda function.
            // This allows us to execute this function on a fiber, ensuring required captures
            // remain in scope, as well as ensuring that the requested result is set.
            auto func = [job_result_ptr = &handle.result,
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
            auto* job = new class job(func);
            if (job_priority == priority::normal)
            {
                job_queue_normal_priority.enqueue(job);
            }
            else if (job_priority == priority::low)
            {
                job_queue_low_priority.enqueue(job);
            }
            else if (job_priority == priority::high)
            {
                job_queue_high_priority.enqueue(job);
            }
            else
            {
                // Something went very wrong
            }


            return std::move(handle);
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

        void initialize(size_t fibers, size_t stack_size, fiber_allocator* allocator);

        static size_t get_this_thread_id();

        void shutdown();
        static void create_threads();

        static void register_fiber_thread(fiber_thread* thread);

        static void run_fiber_thread(fiber_thread* thread);
        static void run_sleep_thread(hardware_thread* thread);
        static void run_io_thread(hardware_thread* thread);

        // This function will serve as the entry point for a job_handle. A pointer to this function is what we
        // will give our context upon initial execution.
        static void run_fiber(fiber* fiber_to_run);

        static hardware_thread* get_current_thread();
        static fiber_thread* get_current_fiber_thread();
        static fiber* get_current_fiber();

    private:
        size_t fiber_count = 0;
        size_t stack_size = 0;


        // Allocator for allocating the stack for each fiber.
        fiber_allocator* stack_allocator{};

        // Lock for the stack_allocator. Necessary since multiple fiber execution threads will need to access.
        spinlock allocator_lock;

        std::atomic<bool> threads_registered = false;

        // Map of fiber threads. Used for determining fiber context. Writing locks threads, reading is lock free.
        thread_map threads;

        // Thead for handling sleep operations.
        hardware_thread* sleep_thread = nullptr;

        // ID of the sleep thread.
        size_t sleep_thread_id{};

        // Thread for handling IO operations.
        hardware_thread* io_thread = nullptr;

        // ID of the IO thread.
        size_t io_thread_id{};

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


        queue<job*> job_queue_low_priority;
        queue<job*> job_queue_normal_priority;
        queue<job*> job_queue_high_priority;


    };

}

#endif
