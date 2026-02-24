#ifndef DAQMANAGER_H
#define DAQMANAGER_H

#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include "mcc128.h"
#include "Calibration_Class.h"

using std::string;
using std::vector;

/*

DAQManager class is responsible for managing the DAQ system.
It is responsible for establishing an initial connection to the DAQ system.
A command is used to instruct the DAQs to start reading data, which is sampled at
the maximum bandwidth, possible, and averaged after a set number of samples.
*/
class DAQManager
{
    private:

    // Hat information
    const static int num_hats{2};
    const static vector<uint8_t> hat_address_vector {0, 1};
    const static bool single_ended_mode{true};
    const static int voltage_range_pm{10}; // volts
    vector<bool> is_reading;

    // Channel information
    vector<std::unique_ptr<Calibration>> calibration_vector;
    vector<bool> is_calibrated;
    
    // Temp data storage
    double time_begin;
    long averages_performed;
    vector<vector<double>> temp_data_vector; // stores data less than 1000

    // sampling constants
    const static double sample_rate = 12500; // samples per second
    const static double averaging_time = 0.5; // seconds
    const static int averaging_samples = sample_rate * averaging_time; // number of samples wait before averaging

    public:








};
#endif