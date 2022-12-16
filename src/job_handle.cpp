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

    void job_handle::wait_for_completion() {
        while (!has_result())
        {
            std::cout << "yielding\n";
            fibers::current::yield();
        }
    }

    void job_handle::set_result(void* newResult) {
        result_lock.lock();
        result = newResult;
        result_lock.unlock();
    }
}