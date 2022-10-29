#include "../include/fiber_manager.h"
#include "../include/fibers.h"


#include <iostream>
#include <mutex>
#if __has_include(<Windows.h>)
#include <Windows.h>
#endif

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

    void fiber_manager::initialize(size_t fiber_count_, size_t stack_size_, fiber_allocator* allocator) {
        fiber_count = fiber_count_;
        stack_size = stack_size_;
        stack_allocator = allocator;
        create_threads();
    }

    void fiber_manager::shutdown()
    {
        delete stack_allocator;
    }


    void fiber_manager::create_threads()
    {
        auto* io = new hardware_thread(0);
        io->start(run_io_thread);

        auto* sleep = new hardware_thread(1);
        sleep->start(run_sleep_thread);

        for (unsigned int i = 2; i < std::thread::hardware_concurrency(); ++i)
        {
            auto* thread = new fiber_thread(i);
            thread->start(run_fiber_thread);
        }

    }

    void fiber_manager::run_fiber(fiber* fiber_to_run) {
        fiber_to_run->started = true;
        fiber_to_run->function();
        fiber_to_run->completed = true;
    }



    void fiber_manager::run_fiber_thread(fiber_thread* thread) {
        register_fiber_thread(thread);
        while(true)
        {
            fiber* fiber_to_execute;
            if (instance().ready_queue.try_dequeue(fiber_to_execute) && fiber_to_execute != nullptr)
            {
                thread->current = fiber_to_execute;
                if (!fiber_to_execute->started)
                {
                    // TODO: Allocate stack
                    fiber_to_execute
                    // TODO: Create context
                }
            }
        }
    }

    void fiber_manager::run_sleep_thread(hardware_thread* thread) {
        fiber_manager::instance().sleep_thread_id = get_this_thread_id();
        fiber_manager::instance().sleep_thread = thread;
        thread_registration.wait();

    }

    void fiber_manager::run_io_thread(hardware_thread* thread) {
        fiber_manager::instance().io_thread_id = get_this_thread_id();
        fiber_manager::instance().io_thread = thread;
        thread_registration.wait();
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
        const fiber_thread* thread = get_current_fiber_thread();
        if (thread == nullptr)
            return nullptr;
        return thread->current;
    }

    template<class Rep, class Period>
    void fiber_manager::sleep_current_fiber(const std::chrono::duration<Rep, Period> &duration) {
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
