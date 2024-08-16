#include "../include/job_handle.h"

#include "../include/fiber_manager.h"
#include "../include/fibers.h"

namespace fibers
{
    size_t job_handle_helper::get_new_id() {
        static std::atomic<size_t> job_counter = 0;
        return job_counter++;
    }

    void job_handle_helper::yield() {
        fibers::current::yield();
    }
}
