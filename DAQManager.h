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
    const vector<uint8_t> hat_address_vector{0, 1}; // vector containing address of each DAQ HAT
    AnalogInputMode input_mode{A_IN_MODE_SE}; // input mode (single ended or differential), specified using enum from mcc128.h
    AnalogInputRange voltage_range{A_IN_RANGE_BIP_10V}; // voltage range (assumed to be +/- 10V), specified using enum from mcc128.h
    bool expected_read_state; // true if the DAQ HAT is reading data, false if it is not

    // Channel information
    vector<std::unique_ptr<Calibration>> calibration_vector; // vector of calibration objects for each channel
    vector<bool> is_calibrated; // true if the channel is calibrated, false if it is not
    
    // Temp data storage
    double time_begin; // time the scan began
    long averages_performed; // number of averages performed, used to calculate time values
    vector<vector<double>> temp_data_vector; // stores data if there arent enough reading for averaging

    // sampling constants
    const double sample_rate = 12500; // samples per second
    const double averaging_time = 0.5; // seconds
    const int averaging_samples = sample_rate * averaging_time; // number of samples wait before averaging

    // Internal functions
    

    public:

    // constructor
    DAQManager();

    // destructor
    ~DAQManager();

    // getters
    int get_num_hats() const;
    vector<uint8_t> get_hat_address_vector() const;
    bool get_single_ended_mode() const;

    int get_voltage_range_pm() const;
    double get_sample_rate() const;
    double get_averaging_time() const;
    int get_averaging_samples() const;
    long get_averages_performed() const;
    vector<vector<double>> get_temp_data_vector() const;

    bool get_read_state() const;
    vector<bool> check_read_state() const;

    // calibration functions
    void calibrate_from_vector(int const &channel_number, vector<double> const &temperature_vector, vector<double> const &voltage_vector);
    void calibrate_from_file(int const &channel_number, string const &file_name);
    bool is_calibrated(int const &channel_number) const;

    // operational functions
    void start_reading(); // start reading data from the DAQ HATs
    void stop_reading(); // stop reading data from the DAQ HATs and perform cleanup

    vector<vector<double>> read_data(); // return averaged data from set number of DAQ HAT readings

};
#endif