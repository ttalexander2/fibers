#pragma once

#ifndef FIBERS_JOB_HANDLE_H
#define FIBERS_JOB_HANDLE_H

#include <exception>
#include <iostream>

#include "primitives/mutex.h"

namespace fibers
{
    class fiber_manager;

    class job_handle_helper {
        template<typename ReturnType>
        friend class job_handle;
        static size_t get_new_id();
        static void yield();
    };

    template <typename ResultType>
    class job_handle
    {
        friend class fiber_manager;

    public:

        job_handle(const job_handle&) = delete;
        job_handle& operator= (const job_handle&) = delete;

        ~job_handle() = default;

        [[nodiscard]] bool has_result() noexcept {
            return result_valid;
        }

        ResultType get_result()
        {
            return result;
        }

        void wait_for_completion() {
            while (!has_result())
            {
                job_handle_helper::yield();
            }
        }

         [[nodiscard]] inline size_t get_id() const { return job_id; }



    private:
        job_handle() {
            job_id = job_handle_helper::get_new_id();
        }

        void set_result(ResultType&& val) {
            result = val;
            result_valid = true;
        }

        size_t job_id = 0;

        // Mutex for setting/accessing the result
        mutex result_lock;
        // Warning, this pointer will be set by the fiber, so accessing it requires the use of the mutex.
        ResultType result{};
        std::atomic<bool> result_valid = false;
    };





}

#endif //FIBERS_JOB_HANDLE_H
