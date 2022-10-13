#include "../include/job.h"

#include "../include/fiber_manager.h"
#include "../include/fibers.h"

namespace fibers
{
    job::job()
    {
        static std::atomic<size_t> job_counter = 0;
        job_id = job_counter++;
    }

    bool job::has_result() noexcept
    {
        bool result_exists = false;
        result_lock.lock();
        result_exists = result != nullptr;
        result_lock.unlock();
        return result_exists;
    }

    template <typename T>
    T job::get_result()
    {
        T value;
        bool result_exists = false;
        result_lock.lock();
        result_exists = result != nullptr;
        if (result_exists)
            value = *dynamic_cast<T*>(result);
        result_lock.unlock();

        if (!result_exists)
            throw std::exception("Result not found! Consider using has_result() or try_get_result().");
        return value;
    }

    void job::wait_for_completion() {
        while (!has_result())
        {
            fibers::current::yield();
        }
    }
}