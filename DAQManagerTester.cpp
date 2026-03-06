#include<iostream>
#include"DAQManager.h"

int main()
{
    std::cout<<"DAQManagerTester"<<std::endl;

    DAQManager daq_manager;

    daq_manager.start_reading();
    auto data = daq_manager.read_data();
    std::cout<<data.size()<<std::endl;
    daq_manager.stop_reading();

    return 0;
}