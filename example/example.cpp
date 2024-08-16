#include <iostream>
#include <filesystem>

#include "../include/fibers.h"


#include <chrono>
using namespace std::chrono_literals;

int main()
{
    std::cout << "before init.\n";
    fibers::initialize();


    int val = 34;
    auto func = [v = val]()
    {

        std::cout << "Test_func.\n";
        double xf = 0;
        for (int i = 0; i < 10000000; ++i) {
            xf += 2.89748 * i;
        }
        std::cout << xf << "\n";
        return v + 5;
    };

    auto* result = fibers::queue_job(func);
    auto* result2 = fibers::queue_job(func);
    auto* result3 = fibers::queue_job(func);
    auto* result4 = fibers::queue_job(func);

    result->wait_for_completion();
    result2->wait_for_completion();
    result3->wait_for_completion();
    result4->wait_for_completion();


    std::this_thread::sleep_for(2s);

    std::cout << "expected: " << val + 5 << ", actual: " << result->get_result() << "\n";
    std::cout << "expected: " << val + 5 << ", actual: " << result2->get_result() << "\n";
    std::cout << "expected: " << val + 5 << ", actual: " << result3->get_result() << "\n";
    std::cout << "expected: " << val + 5 << ", actual: " << result4->get_result() << "\n";

    std::cout << "done\n";

    fibers::shutdown();

    return 0;

}
