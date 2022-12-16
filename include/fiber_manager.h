#pragma once
#ifndef FIBERS_FIBER_MANAGER_H
#define FIBERS_FIBER_MANAGER_H

#include <thread>
#include <vector>
#include <mutex>
#include <future>
#include <xutility>
#include <map>
#include <iostream>

#include "fiber.h"
#include "free_list_allocator.h"
#include "job_handle.h"
#include "io_operation.h"
#include "hardware_thread.h"
#include "primitives/spinlock.h"
#include "thread_map.h"
#include "fiber_thread.h"
#include "job.h"
#include "priority.h"
#include "third_party/moodycamel/concurrentqueue.h"

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
        static fiber_manager& instance() noexcept {
            static fiber_manager instance;
            return instance;
        }

        template<typename StackAllocator = free_list_allocator>
        void initialize(size_t num_fibers = 200, size_t fiber_stack_size = 4096)
        {
            static_assert(std::is_base_of_v<fiber_allocator, StackAllocator> && !std::is_abstract_v<StackAllocator>,
                    "Provided allocator does not derive from fiber_allocator!");
            initialize_impl(num_fibers, fiber_stack_size, new StackAllocator(fiber_stack_size * num_fibers));

        }

        template<typename Ret, typename... Args>
        job_handle&& queue_job(std::function<Ret(Args...)> function, Args... args, priority job_priority = priority::normal) {

            job_handle handle{};

            std::cout << "handle before: " << std::hex << &handle << "\n";

            // This function moves the promise, function, and argument pack to the lifetime of the lambda function.
            // This allows us to execute this function on a fiber, ensuring required captures
            // remain in scope, as well as ensuring that the requested result is set.
            auto func = [job_result = &handle,
                         func = function, args = std::make_tuple(std::forward<Args>(args) ...)]
                    () -> void {
                return std::apply([&](auto&& ... args) {
                    std::cout << "Job being executed...\n";
                    void* result = reinterpret_cast<void*>(func(args...));
                    std::cout << "handle set: " << std::hex << job_result << "\n";
                    if (result != nullptr)
                    {
                        job_result->set_result(result);
                    }
                }, std::move(args));
            };

            // Create fiber and add to the ready queue
            auto* job = new class job(func);
            job->job_priority = job_priority;

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

        template<class Rep, class Period>
        void sleep_current_fiber(const std::chrono::duration<Rep, Period> &duration) {
            fiber_thread* thread = get_current_fiber_thread();
            if (thread == nullptr)
                return;
            fiber* fiber_to_yield = thread->current;
            if (fiber_to_yield != nullptr)
            {
                sleep_queue.enqueue(thread->current);
                thread->current = nullptr;
            }
        }


        fiber_manager(const fiber_manager&)= delete;
        fiber_manager& operator=(const fiber_manager&)= delete;

        void shutdown();

    private:
        fiber_manager()
        {
            std::cout << "created fiber manager\n";
        }
        ~fiber_manager() {
            shutdown();
        };

        void initialize_impl(size_t fibers, size_t stack_size, fiber_allocator* allocator);

        static size_t get_this_thread_id();

        static void create_threads();
        void create_fibers();

        static void register_fiber_thread(fiber_thread* thread);

        static void run_fiber_thread(fiber_thread* thread);
        static void run_sleep_thread(hardware_thread* thread);
        static void run_io_thread(hardware_thread* thread);

        // This is used as the entry point for the fiber.
        static void fiber_run();


        static hardware_thread* get_current_thread();
        static fiber_thread* get_current_fiber_thread();
        static fiber* get_current_fiber();


        job* get_next_job();

    private:
        size_t fiber_count = 0;
        size_t stack_size = 0;


        // Allocator for allocating the stack for each fiber.
        fiber_allocator* stack_allocator{};

        // Lock for the stack_allocator. Necessary since multiple fiber execution threads will need to access.
        spinlock allocator_lock;

        std::atomic<bool> running = false;

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

        // Job queues
        queue<job*> job_queue_low_priority;
        queue<job*> job_queue_normal_priority;
        queue<job*> job_queue_high_priority;


    };

}

#endif
