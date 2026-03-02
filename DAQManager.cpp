#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <utility>

#include "mcc128.h"
#include "Calibration_Class.h"

using std::string;
using std::vector;

// -----------------------------------------------------------------------------
// Constructor / Destructor
// -----------------------------------------------------------------------------

DAQManager::DAQManager() // constructor
{
    connect_daqs(); // Create connection with DAQs, throws error if connection fails
    configure_daqs(); // Configure DAQs, throws error if configuration fails

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

    establish_calibration_vectors();
    apply_calibration_config();
}

DAQManager::~DAQManager() // destructor
{
    // check if DAQs are still scanning and stop reading if they are
    if (check_read_state() == true)
    {
        stop_reading();
    }
}

// -----------------------------------------------------------------------------
// Getters
// -----------------------------------------------------------------------------

int DAQManager::get_num_hats() const {return num_hats;}
vector<uint8_t> DAQManager::get_hat_address_vector() const {return hat_address_vector;}
bool DAQManager::get_single_ended_mode() const {return single_ended_mode;}
int DAQManager::get_voltage_range_pm() const {return voltage_range_pm;}
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
    for (hat_address_iterator = hat_address_vector.begin(); hat_address_iterator != hat_address_vector.end(); ++hat_address_iterator)
    {
        pair<uint16_t, uint32_t> scan_status = enquire_scan_status(*hat_address_iterator);
        if (scan_status.first == STATUS_RUNNING)
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
    for (hat_address_iterator = hat_address_vector.begin(); hat_address_iterator != hat_address_vector.end(); ++hat_address_iterator)
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
    for (hat_address_iterator = hat_address_vector.begin(); hat_address_iterator != hat_address_vector.end(); ++hat_address_iterator)
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
    // Loops through vector containing board addresses and sets input mode and voltage range for each board, throws error if setting fails
    for (hat_address_iterator = hat_address_vector.begin(); hat_address_iterator != hat_address_vector.end(); ++hat_address_iterator)
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
        is_calibrated.push_back(false); // add false to is_calibrated vector
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

// -----------------------------------------------------------------------------
// Calibration
// -----------------------------------------------------------------------------

void DAQManager::calibrate_from_vector(int const &channel_number, vector<double> const &temperature_vector, vector<double> const &voltage_vector)
{
    /* Future implementation: check if calibration data is valid
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
    calibration_vector[channel_number]->apply_calibration();

    is_calibrated[channel_number] = true; // reflect applied calibration in data member
}

void DAQManager::calibrate_from_file(int const &channel_number, string const &file_name)
{
    if (test_if_file_present(file_name) == false)
    {
        return;
    }
    calibration_vector[channel_number]->fill_from_file(file_name);
    calibration_vector[channel_number]->apply_calibration();
    is_calibrated[channel_number] = true;
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
    is_calibrated[channel_number] = false;
}

void DAQManager::copy_calibration(int const &source_channel_number, int const &target_channel_number)
{
    if (source_channel_number < 0 || source_channel_number >= num_channels
        || target_channel_number < 0 || target_channel_number >= num_channels
        || is_calibrated[source_channel_number] == false)
    {
        return;
    }
    calibration_vector[target_channel_number] = std::make_unique<Calibration>(*calibration_vector[source_channel_number]);
    is_calibrated[target_channel_number] = is_calibrated[source_channel_number];
}

// -----------------------------------------------------------------------------
// Operational
// -----------------------------------------------------------------------------

void DAQManager::start_reading()
{
    //begin both daqs scanning at given rate
    //set expected_read_state to true
    //set time_begin to current time
    //set averages_performed to 0
    //clear temp_data_vector
}

void DAQManager::stop_reading()
{
    //stop daqs reading
    //clean up scan resources
    //set expected_read_state to false
}

vector<vector<double>> DAQManager::read_data()
{
    //check if expected_read_state is true
    //if not return empty vector

    //read data from DAQs
    //concatenate temp_data_vector with new data
    //average data in given range and add to new vector
    //leftover data moved to temp data vector
    //return new vector
}
