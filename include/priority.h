//
// Created by Thomas on 10/28/2022.
//

#ifndef FIBERS_PRIORITY_H
#define FIBERS_PRIORITY_H

#include <cstdint>

namespace fibers
{
    enum class priority : uint8_t
    {
        normal = 0,
        low = 1,
        high = 2,
    };
}

#endif //FIBERS_PRIORITY_H
