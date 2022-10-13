
#include "../../include/primitives/condition_variable.h"


namespace fibers
{

    void condition_variable::notify_one() noexcept {

    }

    void condition_variable::notify_all() noexcept {

    }

    void condition_variable::wait(mutex lock) {

    }

    template<class Predicate>
    void condition_variable::wait(mutex lock, Predicate stop_waiting) {

    }

    template<class Rep, class Period>
    void condition_variable::wait_for(mutex lock, const std::chrono::duration<Rep, Period> &rel_time) {

    }

    template<class Rep, class Period, class Predicate>
    void condition_variable::wait_for(mutex lock, const std::chrono::duration<Rep, Period> &rel_time,
                                      Predicate stop_waiting) {

    }

    template<class Clock, class Duration>
    void condition_variable::wait_until(mutex lock, const std::chrono::time_point<Clock, Duration> &timeout_time) {

    }

    template<class Clock, class Duration, class Predicate>
    void condition_variable::wait_until(mutex lock, const std::chrono::time_point<Clock, Duration> &timeout_time,
                                        Predicate stop_waiting) {

    }
}