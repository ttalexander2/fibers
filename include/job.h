#pragma once

#ifndef FIBERS_JOB_H
#define FIBERS_JOB_H

#include <functional>
#include "priority.h"

namespace fibers
{

    class job
    {
    public:
        explicit job(std::function<void()> f);
        std::function<void()> func;
        priority job_priority = priority::normal;
    };
}

#endif //FIBERS_JOB_H
