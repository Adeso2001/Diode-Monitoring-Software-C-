#include "DiodeProgram.h"
#include <iostream>
#include <thread>
#include <chrono>

#ifdef _WIN32
    #include <conio.h>
#else
    #include <termios.h>
    #include <unistd.h>
    #include <fcntl.h>
#endif

int getch_nonblocking() {
    #ifdef _WIN32
        if (_kbhit()) {
            return _getch();
        }
        return -1;
    #else
        // On Linux, use read() with pre-configured non-blocking terminal
        char ch;
        if (read(STDIN_FILENO, &ch, 1) > 0) {
            return ch;
        }
        return -1;
    #endif
}

int main()
{
    #ifndef _WIN32
    // Set up terminal for non-blocking key input (Linux/Mac)
    struct termios old_tio, new_tio;
    tcgetattr(STDIN_FILENO, &old_tio);
    new_tio = old_tio;
    new_tio.c_lflag &= (~ICANON & ~ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
    #endif

    DiodeProgram program;
    program.run();
    std::cout << "Press 'q' to quit..." << std::endl;
    std::cout.flush();
    char input = ' ';
    while(input != 'q' && input != 'Q')
    {
        int key = getch_nonblocking();
        if (key != -1) {
            input = (char)key;
            std::cout << "Key pressed: " << input << std::endl;
            std::cout.flush();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    std::cout << "Q pressed - shutting down program..." << std::endl;
    std::cout.flush();
    program.quitApp();

    #ifndef _WIN32
    // Restore terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &old_tio);
    #endif

    return 0;
}   