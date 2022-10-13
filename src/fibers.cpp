#include "../include/fibers.h"

namespace fibers::current
{
    void yield()
    {
        fiber_manager::instance().yield_current_fiber();
    }
}