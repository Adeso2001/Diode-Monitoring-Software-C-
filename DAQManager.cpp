#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include "mcc128.h"
#include "Calibration_Class.h"

using std::string;
using std::vector;

// -----------------------------------------------------------------------------
// Constructor / Destructor
// -----------------------------------------------------------------------------

DAQManager::DAQManager()
{
    const int hat_connection_error{0};
    const int hat_initialization_error{1};

    // Loops through vector containing board addresses and opens connection with each board, throws error if connection fails
    for (hat_address_iterator = hat_address_vector.begin(); hat_address_iterator != hat_address_vector.end(); ++hat_address_iterator)
    {
        ResultCode hat_connection_result = mcc128_open(*hat_address_iterator);
        if (hat_connection_result != RESULT_SUCCESS)
        {
            throw hat_connection_error;
        }
    }

    // Loops through vector containing board addresses and sets input mode and voltage range for each board, throws error if setting fails
    for (hat_address_iterator = hat_address_vector.begin(); hat_address_iterator != hat_address_vector.end(); ++hat_address_iterator)
    {
        // Set input mode (single ended or differential)
        ResultCode hat_mode_result = mcc128_a_in_mode_write(*hat_address_iterator, input_mode);
        if (hat_mode_result != RESULT_SUCCESS)
        {
            throw hat_initialization_error;
        }

        // Set voltage range ( +/- 10V, +/- 5V, +/- 2V, +/- 1V)
        ResultCode hat_range_result = mcc128_a_in_range_write(*hat_address_iterator, voltage_range);
        if (hat_range_result != RESULT_SUCCESS)
        {
            throw hat_initialization_error;
        }
    }

    expected_read_state = false;

    //establish calibration vectors for each channel
    //establish is_calibrated vector
    //set expected_read_state to false
}

DAQManager::~DAQManager()
{
    //if daqs still scanning stop
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
    //send command to DAQs to check if they are still scanning
}

// -----------------------------------------------------------------------------
// Calibration
// -----------------------------------------------------------------------------

void DAQManager::calibrate_from_vector(int const &channel_number, vector<double> const &temperature_vector, vector<double> const &voltage_vector)
{
    //use calibration class function to enter temp + voltage data into calibration object
    //use c_spline class function to apply calibration
}

void DAQManager::calibrate_from_file(int const &channel_number, string const &file_name)
{
    //use calibration class function to enter temp + voltage data into calibration object from file
    //use c_spline class function to apply calibration
}

bool DAQManager::is_calibrated(int const &channel_number) const
{
    // check id chanel number in question is calibrated
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
