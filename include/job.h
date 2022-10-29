#pragma once

#ifndef FIBERS_JOB_H
#define FIBERS_JOB_H

#include <functional>

namespace fibers
{

    class job
    {
    public:
        explicit job(std::function<void()> f);
        std::function<void()> func;
    };
}

#endif //FIBERS_JOB_H
