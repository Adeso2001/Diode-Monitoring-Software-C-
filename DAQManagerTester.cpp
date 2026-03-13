#include<iostream>
#include"DAQManager.h"
#include<thread>
#include<chrono>
#include<vector>

int main()
{
    std::cout<<"DAQManagerTester"<<std::endl;

    DAQManager daq_manager;
    vector<uint16_t> read_state;

    std::cout<<"Starting reading..."<<std::endl;
    daq_manager.start_reading();
    std::cout<<"actual sample rate is "<<daq_manager.check_sample_rate()<<std::endl;
    std::cout<<"checking read state"<<std::endl;
    read_state = daq_manager.check_read_state();
    for(auto read_state_iterator = read_state.begin(); read_state_iterator != read_state.end(); ++read_state_iterator)
    {
        std::cout<< "read state for board is: "<< *read_state_iterator <<std::endl;
    }



    for (int i{0}; i < 100; i++)
    {
        std::cout<<"loop "<<i<<std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        auto data = daq_manager.read_data();

        if (data.size() != 0)
        {
            std::vector<double> data_first_line{data[0]};

            for(auto result_iterator = data_first_line.begin(); result_iterator != data_first_line.end(); ++result_iterator)
            {
                std::cout<<*result_iterator<<", ";
            }
        }
        std::cout<<std::endl;
    }
    read_state = daq_manager.check_read_state();
    for(auto read_state_iterator = read_state.begin(); read_state_iterator != read_state.end(); ++read_state_iterator)
    {
        std::cout<< "read state for board is: "<< *read_state_iterator <<std::endl;
    }
    
    daq_manager.stop_reading();

    return 0;
}