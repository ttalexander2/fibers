#pragma once

#ifndef FIBERS_FIBERS_H
#define FIBERS_FIBERS_H

#include "fiber_manager.h"
#include "job_handle.h"
#include "lambda_type_deduction.h"
#include "fiber_allocator.h"
#include "job_handle.h"

namespace fibers
{
    /// @brief Function to initialize the fibers system. Should be called only once in the application lifetime,
    /// and before the use of the job system.
    /// \tparam Allocator Memory allocator for fiber stack_space. Default uses a free_list_allocator, which preallocates
    /// memory on the heap. Allocations for stack space occur immediately before job execution, so
    /// any provided allocator should take into account the constant allocation/deallocation.
    /// \param fibers Number of fibers to create.
    /// \param stack_size Size in bytes for a fiber's stack.
    template <typename Allocator = free_list_allocator>
    inline void initialize(size_t fibers = 200, size_t stack_size = 4096)
    {
        fiber_manager::instance().initialize<Allocator>(fibers, stack_size);
    }


    inline void shutdown()
    {
        fiber_manager::instance().shutdown();
    }

    /// @brief Queues a job_handle to run on a fiber, from a standard function.
    template <typename Ret, typename... Args>
    inline job_handle<Ret>* queue_job(std::function<Ret(Args...)> function, Args... args)
    {
        return fiber_manager::instance().queue_job(function, std::forward<Args>(args)...);
    }

    /// @brief Queues a job_handle to run on a fiber, from a lambda function.
    template <typename Func, typename... Args, typename Ret = std::invoke_result_t<Func, Args...>>
    inline job_handle<Ret>* queue_job(Func&& f, Args... args)
    {
        return fiber_manager::instance().queue_job(FFL(f), std::forward<Args>(args)...);
    }

    /// @brief Queues an IO operation to run on the IO thread.
    /// Fiber yields until corresponding IO operation is complete.
    template <typename Ret, typename... Args>
    inline void yield_io(std::function<Ret(Args...)> function, Args... args)
    {

    }

    /// @brief Queues an IO operation to run on the IO thread.
    /// Fiber yields until corresponding IO operation is complete.
    template <typename Func, typename... Args, typename Ret = std::invoke_result_t<Func&&(Args&&...)>>
    inline void yield_io(Func&& f, Args... args)
    {
        //fiber_manager::instance().io
    }

    //bool is_currently_a_fiber();

    namespace current
    {
        /// @brief Yields the execution of this fiber to another.
        /// This fiber is immediately added to the ready queue, and execution will resume on an available thread
        /// when it reaches the front of the queue.
        void yield();

        /// @brief Puts the fiber to sleep for a set period of time.
        /// This function yields to another fiber, and won't queue the fiber for execution until the duration is
        /// complete.
        /// NOTE: This function likely shouldn't be used for precise timings, as execution is not guaranteed
        /// to continue immediately, only when it reaches the front of the ready queue, and there exists
        /// an available fiber execution thread.
        template <class Rep, class Period>
        void sleep(const std::chrono::duration<Rep, Period>& duration)
        {

        }
    }

}

#endif //FIBERS_FIBERS_H
