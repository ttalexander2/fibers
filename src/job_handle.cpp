#include "../include/job_handle.h"

#include "../include/fiber_manager.h"
#include "../include/fibers.h"

namespace fibers
{
    job_handle::job_handle()
    {
        static std::atomic<size_t> job_counter = 0;
        job_id = job_counter++;
    }

    bool job_handle::has_result() noexcept
    {
        bool result_exists = false;
        result_lock.lock();
        result_exists = result != nullptr;
        result_lock.unlock();
        return result_exists;
    }

    template <typename T>
    T job_handle::get_result()
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

    void job_handle::wait_for_completion() {
        while (!has_result())
        {
            fibers::current::yield();
        }
    }
}