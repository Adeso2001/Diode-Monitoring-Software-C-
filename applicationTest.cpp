#include "DiodeProgram.h"
#include <conio.h>

int main()
{
    DiodeProgram program;
    program.run();
    char input = ' ';
    while(input != 'q' && input != 'Q')
    {
        if(_kbhit())
        {
            input = _getch();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    program.quitApp();
    return 0;
}   