#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <chrono>
#include <cmath>

#include "mcc128.h"
#include "Calibration_Class.h"
#include "DAQManager.h"
#include "gpio.h"
#include "util.h"
#include "nist.h"

using std::string;
using std::vector;
using std::pair;

// -----------------------------------------------------------------------------
// Constructor / Destructor
// -----------------------------------------------------------------------------

DAQManager::DAQManager() // constructor
{
    connect_daqs(); // Create connection with DAQs, throws error if connection fails
    configure_daqs(); // Configure DAQs, throws error if configuration fails

    cleanup_scan_resources(); // clean up scan resources for all DAQs incase they werent closed properly last time
    expected_read_state = false;

    // Calculate number of DAQs, channels per DAQ, and total channels
    num_daqs = hat_address_vector.size();
    if (input_mode == A_IN_MODE_SE)
    {
        num_channels_single_daq = 8;
    }
    else
    {
        num_channels_single_daq = 4;
    }
    num_channels = num_daqs * num_channels_single_daq;
    temp_data_vector = std::vector<std::vector<double>>(100, std::vector<double>(num_channels));

    establish_calibration_vectors();
    apply_calibration_config();
}

DAQManager::~DAQManager() // destructor
{
    // check if DAQs are still scanning and stop reading if they are
    if (get_read_state())
    {
        stop_reading();
    }
}

// -----------------------------------------------------------------------------
// Getters
// -----------------------------------------------------------------------------

int DAQManager::get_num_hats() const {return num_daqs;}
vector<uint8_t> DAQManager::get_hat_address_vector() const {return hat_address_vector;}
bool DAQManager::get_single_ended_mode() const {return input_mode == A_IN_MODE_SE;}
int DAQManager::get_voltage_range_pm() const {return voltage_range == A_IN_RANGE_BIP_10V ? 10 : (voltage_range == A_IN_RANGE_BIP_5V ? 5 : (voltage_range == A_IN_RANGE_BIP_2V ? 2 : 1));}
double DAQManager::get_sample_rate() const {return sample_rate;}
double DAQManager::get_averaging_time() const {return averaging_time;}
int DAQManager::get_averaging_samples() const {return averaging_samples;}
long DAQManager::get_averages_performed() const {return averages_performed;}
vector<vector<double>> DAQManager::get_temp_data_vector() const {return temp_data_vector;}
bool DAQManager::get_read_state() const {return expected_read_state;}

// Function checks if physical connection with open DAQs still exists
vector<bool> DAQManager::check_read_state() const 
{
    vector<bool> read_state;

    // loop through each DAQ and check if it is still scanning
    for (auto hat_address_iterator = hat_address_vector.begin(); hat_address_iterator != hat_address_vector.end(); ++hat_address_iterator)
    {
        pair<uint16_t, uint32_t> scan_status = enquire_scan_status(*hat_address_iterator);
        if (scan_status.first & STATUS_RUNNING)
        {
            read_state.push_back(true); // add true to read_state vector if DAQ is still scanning
        }
        else
        {
            read_state.push_back(false); // add false to read_state vector if DAQ is not scanning
        }
    }
    return read_state;
}

vector<uint32_t> DAQManager::get_buffer_sizes() const
{
    vector<uint32_t> buffer_sizes;

    // loop through each DAQ and get buffer size
    for (auto hat_address_iterator = hat_address_vector.begin(); hat_address_iterator != hat_address_vector.end(); ++hat_address_iterator)
    {
        buffer_sizes.push_back(enquire_scan_status(*hat_address_iterator).second);
    }
    return buffer_sizes;
}

// -----------------------------------------------------------------------------
// Internal Methods
// -----------------------------------------------------------------------------

void DAQManager::connect_daqs()
{
    int hat_connection_result;

    // Loops through vector containing board addresses and opens connection with each board, throws error if connection fails
    for (auto hat_address_iterator = hat_address_vector.begin(); hat_address_iterator != hat_address_vector.end(); ++hat_address_iterator)
    {
        
        hat_connection_result = mcc128_open(*hat_address_iterator);
        if (hat_connection_result != RESULT_SUCCESS)
        {
            throw hat_connection_result;
        }
    }
}

void DAQManager::configure_daqs()
{
    int hat_initialization_error = 1; // error code for hat initialization failure
    
    // Loops through vector containing board addresses and sets input mode and voltage range for each board, throws error if setting fails
    for (auto hat_address_iterator = hat_address_vector.begin(); hat_address_iterator != hat_address_vector.end(); ++hat_address_iterator)
    {
        // Set input mode (single ended or differential)
        int hat_mode_result = mcc128_a_in_mode_write(*hat_address_iterator, input_mode);
        if (hat_mode_result != RESULT_SUCCESS)
        {
            throw hat_initialization_error;
        }

        // Set voltage range ( +/- 10V, +/- 5V, +/- 2V, +/- 1V)
        int hat_range_result = mcc128_a_in_range_write(*hat_address_iterator, voltage_range);
        if (hat_range_result != RESULT_SUCCESS)
        {
            throw hat_initialization_error;
        }
    }
}

void DAQManager::establish_calibration_vectors()
{
    // use loop to create smart pointers for each channel and set is_calibrated to false
    for (int channel_number = 0; channel_number < num_channels; ++channel_number)
    {
        calibration_vector.push_back(std::make_unique<Calibration>()); // create smart pointer and add to vector
        is_calibrated_vector.push_back(false); // add false to is_calibrated vector
    }
}

void DAQManager::apply_calibration_config()
{
    // TODO: Implement calibration from config file
}

pair<uint16_t, uint32_t> DAQManager::enquire_scan_status(uint8_t const &hat_address) const
{
    uint16_t status;
    uint32_t buffer_size;

    // send command to DAQ to enquire about scan status
    int status_result = mcc128_a_in_scan_status(hat_address, &status, &buffer_size);
    if (status_result != RESULT_SUCCESS)
    {
        throw status_result;
    }

    return pair<uint16_t, uint32_t>(status, buffer_size);
}

void DAQManager::cleanup_scan_resources()
{
    for (auto hat_address_iterator = hat_address_vector.begin(); hat_address_iterator != hat_address_vector.end(); ++hat_address_iterator)
    {
        mcc128_a_in_scan_cleanup(*hat_address_iterator);
    }
}

void DAQManager::reshape_data(vector<vector<double>> &data, int const &num_readings)
{
    // create a vector of vectors to store the reshaped data
    std::vector<std::vector<double>> reshaped_data(num_readings, std::vector<double>(num_channels));

    // loop through each reading in a single time step
    for (int reading{0}; reading < num_readings; reading++)
    {
        // loop through each DAQ
        for (int daq_iterator{0}; daq_iterator < num_daqs; daq_iterator++)
        {
            //loop through each channel in a single DAQ
            for (int channel_iterator{0}; channel_iterator < num_channels_single_daq; channel_iterator++)
            {
                // add data to reshaped_data
                reshaped_data[reading][daq_iterator * num_channels_single_daq + channel_iterator] = data[reading][daq_iterator * num_channels_single_daq + channel_iterator];
            }
        }
    }
    data = std::move(reshaped_data);
}

void DAQManager::average_data(vector<vector<double>> &data, int const &num_readings, int &averages_count)
{
    int temp_data_size;
    int collective_num_readings;
    int leftover_data_size;
    double average_sum{0.0};

    temp_data_size = temp_data_vector.size();
    collective_num_readings = num_readings + temp_data_size;
    leftover_data_size = collective_num_readings % averaging_samples; // number of readings that will be left over after averaging, so is moved to temp_data_vector

    averages_count = (collective_num_readings - leftover_data_size) / averaging_samples;

    // create a vector of vectors to store the averaged data, with columns for time, voltage1, temperature1, voltage2, temperature2, etc.
    std::vector<std::vector<double>> averaged_data(averages_count, std::vector<double>(num_channels * 2 + 1));

    // add temp_data_vector to start of new data
    data.insert(data.begin(), temp_data_vector.begin(), temp_data_vector.end());

    // move leftover data to temp_data_vector
    temp_data_vector = std::vector<std::vector<double>>(data.end() - leftover_data_size, data.end());

    // loop through each data sample which will be averaged
    for (int average_iterator{0}; average_iterator < averages_count; average_iterator++)
    {
        // loop through each channel in the data sample
        for (int channel_iterator{0}; channel_iterator < num_channels; channel_iterator++)
        {
            // loop through each reading in the single channel sample 
            for (int reading_iterator{0}; reading_iterator < averaging_samples; reading_iterator++)
            {
                // sum all the readings in the single channel sample
                average_sum += data[average_iterator * averaging_samples + reading_iterator][channel_iterator];
            }
            // calculate the average of the single channel sample, and assign into to averaged_data at the correct position
            averaged_data[average_iterator][channel_iterator * 2 + 1] = average_sum / averaging_samples;
            average_sum = 0.0;
        }
        // add time to averaged_data
        averaged_data[average_iterator][0] = (average_iterator + averages_performed) * averaging_time;
        averages_performed++;
    }

    data = std::move(averaged_data);
}

void DAQManager::translate_data(vector<vector<double>> &data, int const &averages_count)
{
    // loop through each average
    for (int average_iterator{0}; average_iterator < averages_count; average_iterator++)
    {
        // loop through each channel
        for (int channel_iterator{0}; channel_iterator < num_channels; channel_iterator++)
        {
            // interpolate temperature from voltage if channel is calibrated, otherwise leave blank
            if (is_calibrated_vector[channel_iterator] == true)
            {
                data[average_iterator][channel_iterator * 2 + 2] = calibration_vector[channel_iterator]->interpolate_value(data[average_iterator][channel_iterator * 2 + 1]);
            }
            else
            {
                data[average_iterator][channel_iterator * 2 + 2] = std::nan("");
            }
        }
    }
}

// -----------------------------------------------------------------------------
// Calibration
// -----------------------------------------------------------------------------

void DAQManager::calibrate_from_vector(int const &channel_number, vector<double> const &temperature_vector, vector<double> const &voltage_vector)
{
    /* Future implementation TODO: check if calibration data is valid
    if (check_calibration_data_validity(temperature_vector, voltage_vector) == false)
    {
        std::cout << "Calibration data for channel " << channel_number << " is not valid" << std::endl;
        return;
    }
    */

    // use data from vectors to fill calibration object
    calibration_vector[channel_number]->fill_x_from_std_vector(temperature_vector);
    calibration_vector[channel_number]->fill_y_from_std_vector(voltage_vector);

    // apply calibration to data
    calibration_vector[channel_number]->calculate_spline();

    is_calibrated_vector[channel_number] = true; // reflect applied calibration in data member
}

void DAQManager::calibrate_from_file(int const &channel_number, string const &file_name)
{
    if (test_if_file_present(file_name) == false)
    {
        return;
    }
    calibration_vector[channel_number]->fill_from_file(file_name);
    calibration_vector[channel_number]->calculate_spline();
    is_calibrated_vector[channel_number] = true;
}

bool DAQManager::is_calibrated(int const &channel_number) const
{
    // check id chanel number in question is calibrated
}

void DAQManager::clear_calibration(int const &channel_number)
{
    if (channel_number < 0 || channel_number >= num_channels)
    {
        return;
    }
    calibration_vector[channel_number] = std::make_unique<Calibration>();
    is_calibrated_vector[channel_number] = false;
}

void DAQManager::copy_calibration(int const &source_channel_number, int const &target_channel_number)
{
    // verify parameters are valid
    if (source_channel_number < 0 || source_channel_number >= num_channels
        || target_channel_number < 0 || target_channel_number >= num_channels
        || is_calibrated_vector[source_channel_number] == false)
    {
        return;
    }

    // copy calibration from source to target
    calibration_vector[target_channel_number] = std::make_unique<Calibration>(*calibration_vector[source_channel_number]);
    is_calibrated_vector[target_channel_number] = true;
}

// -----------------------------------------------------------------------------
// Operational
// -----------------------------------------------------------------------------

void DAQManager::start_reading()
{
    int scan_result;
    uint8_t channel_mask = 0xFF; // scan all channels
    uint32_t options = OPTS_CONTINUOUS;

    if (expected_read_state == true)
    {
        return;
    }

    for (auto hat_address_iterator = hat_address_vector.begin(); hat_address_iterator != hat_address_vector.end(); ++hat_address_iterator)
    {
        scan_result = mcc128_a_in_scan_start(*hat_address_iterator, channel_mask, averaging_samples, sample_rate, options);
        if (scan_result != RESULT_SUCCESS)
        {
            std::cout << "Error starting scan for DAQ " << *hat_address_iterator << ": " << scan_result << std::endl;
            return;
        }
    }

    expected_read_state = true;
    time_begin = std::chrono::high_resolution_clock::now(); // set time_begin to current time
    averages_performed = 0;
    temp_data_vector.clear(); // clear temp data vector to start fresh
    
}

void DAQManager::stop_reading()
{
    //stop all DAQs reading
    for (auto hat_address_iterator = hat_address_vector.begin(); hat_address_iterator != hat_address_vector.end(); ++hat_address_iterator)
    {
        mcc128_a_in_scan_stop(*hat_address_iterator);
    }

    //clean up scan resources
    cleanup_scan_resources();

    //set expected_read_state to false
    expected_read_state = false;
}

vector<vector<double>> DAQManager::read_data()
{
    //check if expected_read_state is true
    if (expected_read_state == false)
    {
        return vector<vector<double>>();
    }

    vector<vector<double>> data;
    vector<double> temp_data;

    uint16_t status; // status of scan
    int32_t samples_to_read_per_channel{-1}; // number of samples to be read per channel, -1 means all available samples
    double timeout{0.0}; // timeout in seconds, 0.0 means no timeout
    double buffer; // buffer to store data
    uint32_t buffer_size{0}; // size of buffer
    uint32_t samples_read{0}; // number of samples read
    bool first_read{true}; // true if it is the first read, false if it is not

    // creates a vector of vectors, with each inner vector representing a DAQ and containing the unprocessed data read from that DAQ
    int read_result;
    for (auto hat_address_iterator = hat_address_vector.begin(); hat_address_iterator != hat_address_vector.end(); ++hat_address_iterator)
    {
        read_result = mcc128_a_in_scan_read(*hat_address_iterator, &status, samples_to_read_per_channel, timeout, &buffer, buffer_size, &samples_read);
        data.push_back(vector<double>());
        if (first_read == true)
        {
            samples_to_read_per_channel = samples_read;
            first_read = false;
        }
    }

    int num_readings_per_channel = samples_to_read_per_channel;
    int averages_count;

    reshape_data(data, num_readings_per_channel); // turn vector of result vectors into vectors containing voltages of ch1, ch2, ch3, ch4, etc. in that order
    average_data(data, num_readings_per_channel, averages_count); // turn vector of daq readings into vectors containing time, voltage1, temperature1, voltage2, temperature2, etc. in that order, for now leaving temperatures blank
    translate_data(data, averages_count); // fill in temperature columns in data
    return data;
}
