#pragma once
#ifndef FIBERS_CONDITION_VARIABLE_H
#define FIBERS_CONDITION_VARIABLE_H

#include <chrono>
#include "mutex.h"

namespace fibers
{

    class condition_variable
    {

        void notify_one() noexcept;
        void notify_all() noexcept;

        void wait(mutex lock);

        template<class Predicate>
        void wait(mutex lock, Predicate stop_waiting);

        template<class Rep, class Period>
        void wait_for(mutex lock, const std::chrono::duration<Rep, Period>& rel_time);

        template<class Rep, class Period, class Predicate>
        void wait_for(mutex lock, const std::chrono::duration<Rep, Period>& rel_time, Predicate stop_waiting);

        template<class Clock, class Duration>
        void wait_until(mutex lock, const std::chrono::time_point<Clock, Duration>& timeout_time);

        template<class Clock, class Duration, class Predicate>
        void wait_until(mutex lock, const std::chrono::time_point<Clock, Duration>& timeout_time, Predicate stop_waiting);
    };
}



#endif //FIBERS_CONDITION_VARIABLE_H
