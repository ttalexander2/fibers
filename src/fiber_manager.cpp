#include "../include/fiber_manager.h"
#include "../include/fibers.h"


#include <iostream>
#include <mutex>
#if __has_include(<Windows.h>)
#include <Windows.h>
#endif

#include <chrono>
using namespace std::chrono_literals;

namespace fibers
{
    static auto processor_count = std::thread::hardware_concurrency();

    // Since we would like to support earlier than c++20, we'll use this
    // barrier implementation for synchronizing the creation of IO, Sleep, and Fiber threads.
    // This way, we can ensure threads are registered in the thread map, so that we can detect
    // which fiber code is being executed from, by making use of std::this_thread, and performing
    // a lookup in the thread map to retrieve the current fiber.
    class barrier
    {
        std::mutex mutex_;
        std::condition_variable cv_;
        size_t threshold_;
        size_t count_;
        size_t generation_;
    public:
        explicit barrier(size_t count) :
        threshold_(count), count_(count), generation_(0) {}

        void wait() {
            std::unique_lock<std::mutex> lock{mutex_};
            auto gen = generation_;
            if (!--count_) {
                generation_++;
                count_ = threshold_;
                cv_.notify_all();
            }
            else
            {
                cv_.wait(lock, [this, gen] { return gen != generation_; });
            }

        }
    };

    // Barrier used for synchronizing the creation of threads and their registration with the fiber manager.
    static barrier thread_registration(std::thread::hardware_concurrency());

    void fiber_manager::initialize_impl(size_t fiber_count_, size_t stack_size_, fiber_allocator* allocator) {
        fiber_count = fiber_count_;
        stack_size = stack_size_;
        stack_allocator = allocator;
        running = true;
        create_fibers();
        create_threads();
    }

    void fiber_manager::shutdown()
    {
        running = false;
        if (stack_allocator != nullptr)
        {
            delete stack_allocator;
            stack_allocator = nullptr;
        }

    }


    void fiber_manager::create_fibers()
    {
        for (int i = 0; i < fiber_count; i++)
        {
            auto* f = new fiber();
            ready_queue.enqueue(f);
        }
    }


    void fiber_manager::create_threads()
    {
        auto* io = new hardware_thread(0);
        io->start(run_io_thread, io);

        auto* sleep = new hardware_thread(1);
        sleep->start(run_sleep_thread, sleep);

        for (unsigned int i = 2; i < std::thread::hardware_concurrency(); ++i)
        {
            auto* thread = new fiber_thread(i);
            thread->start(run_fiber_thread, thread);
        }

    }



    void fiber_manager::run_fiber_thread(fiber_thread* thread) {
        register_fiber_thread(thread);

        size_t s_stack_size = instance().stack_size;

        while(instance().running)
        {
            fiber* fiber_to_execute;

            // There already exists a ready fiber.
            if (instance().ready_queue.try_dequeue(fiber_to_execute) && fiber_to_execute != nullptr)
            {
                thread->current = fiber_to_execute;

                // This fiber has either finished executing a previous job, or has not yet executed a job
                if (!fiber_to_execute->started)
                {
                    // Cleanup fiber from previous execution
                    if (fiber_to_execute->stack != nullptr){
                        instance().stack_allocator->deallocate(fiber_to_execute->stack);
                        fiber_to_execute->stack = nullptr;
                        fiber_to_execute->ctx = context{};
                        fiber_to_execute->return_ctx = context{};
                        delete fiber_to_execute->current_job;
                        fiber_to_execute->current_job = nullptr;
                        fiber_to_execute->completed = false;
                    }

                    // Get the next job
                    job* next_job = nullptr;
                    while (next_job == nullptr)
                    {
                        next_job = instance().get_next_job();
                    }

                    fiber_to_execute->current_job = next_job;

                    // Allocate the stack
                    fiber_to_execute->stack = instance().stack_allocator->allocate(s_stack_size);

                    // Set stack pointer and execution function
                    fiber_to_execute->ctx.rsp = fiber_to_execute->stack;
                    fiber_to_execute->ctx.rip = (void*)fiber_run;

                    // Get the current context for this thread. We'll save this context so the fiber can return here when
                    // execution of the job is complete.
                    fibers::get_context(&fiber_to_execute->return_ctx);

                    // We check if the job is already complete. If it is, we don't want to start the job again.
                    if (!fiber_to_execute->completed)
                    {
                        std::cout << "before set context\n";
                        fibers::set_context(&fiber_to_execute->ctx);
                    }
                    else
                    {
                        std::cout << "returned from set context\n";
                    }
                    continue;

                }
                // The fiber we dequeued was in the middle of execution of a job. This could have been previously sleeping,
                // waiting on an IO action, or yielded. In this case, we will get our return context, to ensure we switch
                // to the correct thread, and switch to the fiber thread.
                else
                {
                    fibers::get_context(&fiber_to_execute->return_ctx);

                    if (!fiber_to_execute->completed)
                    {
                        fibers::set_context(&fiber_to_execute->ctx);
                    }
                    continue;
                }
            }
        }

        std::cout << "fiber thread stopped\n";
    }

    void fiber_manager::run_sleep_thread(hardware_thread* thread) {
        fiber_manager::instance().sleep_thread_id = get_this_thread_id();
        fiber_manager::instance().sleep_thread = thread;
        thread_registration.wait();

        while(instance().running)
        {
            std::this_thread::sleep_for(0.2s);
        }

        std::cout << "sleep thread stopped\n";

    }

    void fiber_manager::run_io_thread(hardware_thread* thread) {
        fiber_manager::instance().io_thread_id = get_this_thread_id();
        fiber_manager::instance().io_thread = thread;
        thread_registration.wait();

        while(instance().running)
        {
            std::this_thread::sleep_for(0.2s);
        }

        std::cout << "io thread stopped\n";
    }

    void fiber_manager::register_fiber_thread(fiber_thread* thread) {
        fiber_manager::instance().threads.write(get_this_thread_id(), thread);
        thread_registration.wait();
    }

    hardware_thread *fiber_manager::get_current_thread() {
        size_t thread_id = get_this_thread_id();
        if (thread_id == fiber_manager::instance().io_thread_id)
            return fiber_manager::instance().io_thread;
        if (thread_id == fiber_manager::instance().sleep_thread_id)
            return fiber_manager::instance().sleep_thread;
        if (!fiber_manager::instance().threads.contains(thread_id))
            return nullptr;
        return fiber_manager::instance().threads.get(thread_id);
    }

    size_t fiber_manager::get_this_thread_id() {
        return std::hash<std::thread::id>{}(std::this_thread::get_id());
    }

    fiber_thread *fiber_manager::get_current_fiber_thread() {
        size_t thread_id = get_this_thread_id();
        if (!fiber_manager::instance().threads.contains(thread_id))
            return nullptr;
        return fiber_manager::instance().threads.get(thread_id);
    }

    fiber *fiber_manager::get_current_fiber() {
        fiber_thread* thread = get_current_fiber_thread();
        if (thread == nullptr)
            return nullptr;
        return thread->current;
    }

    void fiber_manager::yield_current_fiber() {
        fiber_thread* thread = get_current_fiber_thread();
        if (thread == nullptr) {
            // Function was called from outside a fiber thread.
            // In this case, we'll yield the current thread.
            std::this_thread::yield();
            return;
        }
        fiber* fiber_to_yield = thread->current;
        if (fiber_to_yield != nullptr)
        {
            ready_queue.enqueue(thread->current);
            thread->current = nullptr;
        }

    }

    job *fiber_manager::get_next_job() {

        // Get next job based on priority from the various queues.
        job* next_job;

        if (job_queue_high_priority.try_dequeue(next_job))
        {
            return next_job;
        }

        if (job_queue_normal_priority.try_dequeue(next_job))
        {
            return next_job;
        }

        if (job_queue_low_priority.try_dequeue(next_job))
        {
            return next_job;
        }

        return nullptr;
    }

    void fiber_manager::fiber_run() {
        fiber* current_fiber = instance().get_current_fiber();
        if (current_fiber == nullptr)
            return;
        current_fiber->started = true;
        current_fiber->current_job->func();
        current_fiber->completed = true;
        current_fiber->started = false;

        std::cout << "Finished fiber run\n";
        //Since we didn't save a normal return address, we'll use the context in local fiber storage.

        // TODO: This doesn't work.
        fibers::set_context(&current_fiber->return_ctx);


    }

    //fiber fiber_manager::prepare_fiber(fiber fiber)
    //{
    //    if (!initialized)
    //        return fiber;
    //
    //    fiber.ctx = context{};
    //    void* stackPtr = stack_allocator->allocate(stack_size);
    //    fiber.ctx.rsp = stackPtr;
    //    //fiber.context.rip = fiber.function.;
    //
    //    return fiber;
    //
    //}



}
