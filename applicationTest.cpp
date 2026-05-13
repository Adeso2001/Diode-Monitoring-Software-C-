#include "DiodeProgram.h"

int main()
{
    DiodeProgram program;
    program.run();
    while(true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return 0;
}   