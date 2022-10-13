#include <atomic>
#include "../include/fiber.h"

#include "../include/fiber_manager.h"


namespace fibers
{
    fiber::fiber() {
        static std::atomic<size_t> counter;
        id = counter++;
    }




}
