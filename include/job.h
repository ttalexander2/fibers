#pragma once

#ifndef FIBERS_JOB_H
#define FIBERS_JOB_H

#include "primitives/mutex.h"

namespace fibers
{
    class fiber_manager;

    class job
    {
        friend class fiber_manager;

    public:

        job(const job&) = delete;
        job& operator= (const job&) = delete;

        ~job() = default;
        [[nodiscard]] bool has_result() noexcept;

        template <typename T>
        T get_result();

        void wait_for_completion();

         [[nodiscard]] inline size_t get_id() const { return job_id; }



    private:
        job();
        size_t job_id = 0;

        // Mutex for setting/accessing the result
        mutex result_lock;
        // Warning, this pointer will be set by the fiber, so accessing it requires the use of the mutex.
        void* result = nullptr;
    };





}

#endif //FIBERS_JOB_H
