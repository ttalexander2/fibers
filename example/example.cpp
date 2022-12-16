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
       //fibers::yield_io([&](){
       //    std::cout << "I am a big dummy.\n";
       //});

       //std::cout << "I am a big dummy.\n";

        return v + 5;
    };

    auto&& result = fibers::queue_job(func);
    auto&& result2 = fibers::queue_job(func);

    std::cout << "handle after: " << std::hex << &result << "\n";

    //result.wait_for_completion();

    std::this_thread::sleep_for(2s);

    //std::cout << "val: " << result.get_result<int>() << "\n";

    std::cout << "done\n";

    fibers::shutdown();

}
