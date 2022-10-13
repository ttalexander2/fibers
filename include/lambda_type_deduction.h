#ifndef FIBERS_LAMBDA_TYPE_DEDUCTION_H
#define FIBERS_LAMBDA_TYPE_DEDUCTION_H

// Adapted from this: https://stackoverflow.com/questions/69362554/why-cant-i-deduce-the-function-signature-for-a-mutable-lambda

#include <functional>

namespace fibers
{
    template <typename T>
    struct memfun_type
    {
        using type = void;
    };

    template<typename Ret, typename Class, typename... Args>
    struct memfun_type<Ret(Class::*)(Args...)>
    {
        using type = std::function<Ret(Args...)>;
    };

    template<typename Ret, typename Class, typename... Args>
    struct memfun_type<Ret(Class::*)(Args...) const>
    {
        using type = std::function<Ret(Args...)>;
    };

    template<typename F>
    typename memfun_type<decltype(&F::operator())>::type
    FFL(F const &func)
    { // Function from lambda !
        return func;
    }
}

#endif //FIBERS_LAMBDA_TYPE_DEDUCTION_H
