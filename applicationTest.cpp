#include "DiodeProgram.h"
#include <iostream>

#ifdef _WIN32
    #include <conio.h>
#else
    #include <termios.h>
    #include <unistd.h>
#endif

int getch_nonblocking() {
    #ifdef _WIN32
        if (_kbhit()) {
            return _getch();
        }
        return -1;
    #else
        struct termios oldt, newt;
        int ch;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        newt.c_cc[VMIN] = 0;
        newt.c_cc[VTIME] = 0;
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        ch = getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return (ch == EOF) ? -1 : ch;
    #endif
}

int main()
{
    DiodeProgram program;
    program.run();
    char input = ' ';
    while(input != 'q' && input != 'Q')
    {
        int key = getch_nonblocking();
        if (key != -1) {
            input = (char)key;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    program.quitApp();
    return 0;
}   