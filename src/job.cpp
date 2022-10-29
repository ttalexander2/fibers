#include "../include/job.h"

namespace fibers
{

    job::job(std::function<void()> f)
        : func(f)
    {
    }
}