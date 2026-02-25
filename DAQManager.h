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
    const static int num_hats{2}; // number of DAQ HATs connected (assumed to be 2)
    const static vector<uint8_t> hat_address_vector {0, 1}; // address of each DAQ HAT
    const static bool single_ended_mode{true}; // true if the DAQ HAT is in single ended mode, false if it is in differential mode
    const static int voltage_range_pm{10}; // voltage range in volts (assumed to be +/- 10V)
    bool is_reading; // true if the DAQ HAT is reading data, false if it is not

    // Channel information
    vector<std::unique_ptr<Calibration>> calibration_vector; // vector of calibration objects for each channel
    vector<bool> is_calibrated; // true if the channel is calibrated, false if it is not
    
    // Temp data storage
    double time_begin; // time the reading began
    long averages_performed; // number of averages performed, used to calculate time values
    vector<vector<double>> temp_data_vector; // stores data if there arent enough reading for averaging

    // sampling constants
    const static double sample_rate = 12500; // samples per second
    const static double averaging_time = 0.5; // seconds
    const static int averaging_samples = sample_rate * averaging_time; // number of samples wait before averaging

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

    // calibration functions
    void calibrate_from_vector(int channel_number, vector<double> temperature_vector, vector<double> voltage_vector);
    void calibrate_from_file(int channel_number, string file_name);
    bool is_calibrated(int channel_number) const;

    // operational functions
    void start_reading(); // start reading data from the DAQ HATs
    void stop_reading(); // stop reading data from the DAQ HATs and perform cleanup

    vector<vector<double>> read_data() const; // return averaged data from set number of DAQ HAT readings

};
#endif