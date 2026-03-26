#include<iostream>
#include"DAQManager.h"
#include<thread>
#include<chrono>
#include<vector>
#include<fstream>
#include<termios.h>
#include<unistd.h>
#include<fcntl.h>

int main()
{
    std::cout<<"DAQManagerTester"<<std::endl;

    DAQManager daq_manager;
    vector<uint16_t> read_state;
    std::ofstream csv_file("readings.csv", std::ios::out); // Clear the file
    csv_file.close();
    csv_file.open("readings.csv", std::ios::app); // Reopen for appending

    // Set up terminal for non-blocking key input
    struct termios old_tio, new_tio;
    tcgetattr(STDIN_FILENO, &old_tio);
    new_tio = old_tio;
    new_tio.c_lflag &= (~ICANON & ~ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);

    std::cout<<"Starting reading..."<<std::endl;
    daq_manager.start_reading();
    std::cout<<"actual sample rate is "<<daq_manager.check_sample_rate()<<std::endl;
    std::cout<<"checking read state"<<std::endl;
    read_state = daq_manager.check_read_state();
    for(auto read_state_iterator = read_state.begin(); read_state_iterator != read_state.end(); ++read_state_iterator)
    {
        std::cout<< "read state for board is: "<< *read_state_iterator <<std::endl;
    }



    int i = 0;
    while (true)
    {
        std::cout<<"loop "<<i<<std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        // Check for key press
        char c;
        if (read(STDIN_FILENO, &c, 1) > 0)
        {
            if (c == 'q' || c == 'Q')
            {
                std::cout << "Stopping due to key press." << std::endl;
                break;
            }
        }
        
        auto data = daq_manager.take_buffered_data();

        std::cout << std::fixed;
        std::cout.precision(6);
        
        if (data.size() != 0)
        {
            for (const auto& row : data)
            {
                for (size_t j = 0; j < row.size(); ++j)
                {
                    csv_file << row[j];
                    if (j < row.size() - 1) csv_file << ",";
                }
                csv_file << std::endl;
            }

            for (const auto& row : data)
            {
                for (size_t j = 0; j < row.size(); ++j)
                {
                    std::cout << row[j] << " ";
                }
                std::cout << std::endl;
            }
        }
        i++;
    }

    // Restore terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &old_tio);
    
    daq_manager.stop_reading();

    csv_file.close();

    return 0;
}