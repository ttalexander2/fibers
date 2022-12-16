#pragma once

#ifndef FIBERS_JOB_HANDLE_H
#define FIBERS_JOB_HANDLE_H

#include <exception>
#include "primitives/mutex.h"

namespace fibers
{
    class fiber_manager;

    class job_handle
    {
        friend class fiber_manager;

    public:

        job_handle(const job_handle&) = delete;
        job_handle& operator= (const job_handle&) = delete;

        ~job_handle() = default;
        [[nodiscard]] bool has_result() noexcept;

        template <typename T>
        T get_result()
        {
            T value;
            bool result_exists = false;
            result_lock.lock();
            result_exists = result != nullptr;
            if (result_exists)
                value = *static_cast<T*>(result);
            result_lock.unlock();

            if (!result_exists) {
                throw std::exception("Result not found! Consider using has_result() or try_get_result().");
            }
            return value;
        }

        void wait_for_completion();

         [[nodiscard]] inline size_t get_id() const { return job_id; }



    private:
        job_handle();

         void set_result(void* result);

        size_t job_id = 0;

        // Mutex for setting/accessing the result
        mutex result_lock;
        // Warning, this pointer will be set by the fiber, so accessing it requires the use of the mutex.
        void* result = nullptr;
    };





}

#endif //FIBERS_JOB_HANDLE_H
