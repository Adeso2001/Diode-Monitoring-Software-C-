#ifndef DAQMANAGER_H
#define DAQMANAGER_H

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <utility>

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
    
    int num_daqs; // number of DAQs in the system
    int num_channels_single_daq; // number of channels in a single DAQ
    int num_channels; // number of channels in the system
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

    // Internal Methods
    void connect_daqs(); // connect to DAQs
    void configure_daqs(); // configure DAQs
    void establish_calibration_vectors(); // establish calibration vectors for each channel
    void apply_calibration_config(); // apply calibration configuration for each channel from a config file
    pair<uint16_t, uint32_t> enquire_scan_status(uint8_t const &hat_address) const; // call mcc128_a_in_scan_status to enquire about scan status of a DAQ
    void cleanup_scan_resources(); // clean up scan resources for all DAQs
    void reshape_data(vector<vector<double>> &data, int const &num_readings); // turn vector of result vectors into vectors containing voltages of ch1, ch2, ch3, ch4, etc. in that order
    void average_data(vector<vector<double>> &data, int const &num_readings, int &averages_count); // turn vector of daq readings into vectors containing time, voltage1, temperature1, voltage2, temperature2, etc. in that order, for now leaving temperatures blank
    void translate_data(vector<vector<double>> &data, int const &averages_count); // fill in temperature columns in data

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

    bool get_read_state() const; // get expected read state stored in class, not hardware
    vector<bool> check_read_state() const; // check hardware to see if DAQs are still scanning
    vector<uint32_t> get_buffer_sizes() const; // get buffer sizes for each DAQ
    

    // calibration functions
    void calibrate_from_vector(int const &channel_number, vector<double> const &temperature_vector, vector<double> const &voltage_vector);
    void calibrate_from_file(int const &channel_number, string const &file_name);
    bool is_calibrated(int const &channel_number) const;

    void clear_calibration(int const &channel_number); // clear calibration for a given channel
    void copy_calibration(int const &source_channel_number, int const &target_channel_number); // copy calibration from one channel to another

    // operational functions
    void start_reading(); // start reading data from the DAQ HATs
    void stop_reading(); // stop reading data from the DAQ HATs and perform cleanup

    vector<vector<double>> read_data(); // return averaged data from set number of DAQ HAT readings

};
#endif