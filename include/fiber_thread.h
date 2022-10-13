#pragma once
#ifndef FIBERS_FIBER_THREAD_H
#define FIBERS_FIBER_THREAD_H


#include "hardware_thread.h"
#include "fiber.h"

namespace fibers
{
    class fiber_thread : public hardware_thread {
        friend class fiber_manager;
    public:
        explicit fiber_thread(size_t affinity) : hardware_thread(affinity) {}

        fiber* current = nullptr;
    };
}




#endif //FIBERS_FIBER_THREAD_H
