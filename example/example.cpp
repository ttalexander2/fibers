#include <iostream>
#include <filesystem>
#include "../include/fibers.h"



#if __has_include(<Windows.h>)
#include <Windows.h>
#endif

#if __has_include(<winnt.h>)
#include <winnt.h>
#include <fstream>

#endif

int main()
{
    fibers::initialize();


    int val = 34;
    auto func = [](int v)
    {
        fibers::yield_io([&](){
            std::cout << "I am a big dummy.\n";
        });

        return v + 5;
    };

    auto&& result = fibers::queue_job(func, val);

    result.wait_for_completion();

    std::cout << "val: " << result.get_result<int>() << "\n";
    

}

//void foo() {
//    printf("you called foo\n");
//
//    // Need to exit since there's no return address on the stack after coming
//    // here from |set_context| in main.
//    exit(1);
//}
//
//
//int main() {
//    // This will behave as our stack when we call foo.
//    char data[4096];
//
//    // Remember, stacks grow downwards, so we need a pointer to the end of the stack.
//    char* sp = (char*)(data + sizeof data);
//
//    // Align stack pointer on 16-byte boundary.
//    sp = (char*)((uintptr_t)sp & -16L);
//
//    // Make 128 byte scratch space for the Red Zone. This arithmetic will not unalign
//    // our stack pointer because 128 is a multiple of 16. The Red Zone must also be
//    // 16-byte aligned.
//    sp -= 128;
//
//    fibers::context c {};
//    c.rip = (void*)foo;
//    c.rsp = (void*)sp;
//
//// TODO: This is how you set the StackBase and StackLimit, but I have no clue how to actually obtain these values on context switch
//#ifdef WIN32
//    //PNT_TIB ptib = (PNT_TIB)NtCurrentTeb();
//    //ptib->StackBase = sp;
//    //ptib->StackLimit = &data;
//#endif
//
//    fibers::set_context(&c);
//}
