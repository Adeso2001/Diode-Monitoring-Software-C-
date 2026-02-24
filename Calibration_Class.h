#ifndef CALIBRATION_CLASS_H
#define CALIBRATION_CLASS_H

#include <iostream>
#include <cmath>
#include <string>
#include <vector>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <regex>
#include "C_Spline_Class.h"

using std::string;
using std::vector;

/**
 * Check whether a file exists and can be read.
 * Prints status messages to stdout. Used by fill_from_file() before loading.
 * \param file_name_string Path to the file to test
 * \return true if the file opened and could be read, false otherwise
 */
bool test_if_file_present(const string &file_name_string);

/**
 * Calibration curve that extends C_Spline with metadata and file loading.
 * Load calibration data from a sensor file (e.g. .330 format) via fill_from_file(),
 * then call calculate_spline() and interpolate_value(x) as with C_Spline (e.g. to
 * convert voltage or time to temperature).
 */
class Calibration : public C_Spline
{
    protected:
    string name;                  ///< User-facing calibration name
    string sensor_model;          ///< Sensor model from file header
    string serial_number;         ///< Serial number from file header
    string interpolation_method;  ///< Interpolation method from file header
    string setpoint_limit;        ///< Setpoint limit from file header
    string data_format;           ///< Data format from file header
    string file_name;             ///< Path of the file this calibration was loaded from
    int break_points;             ///< Number of break points (knots) from file header

    public:

    /** Default constructor. No file loaded; set name/metadata via fill_from_file() or setters. */
    Calibration();

    /** Construct with a calibration name. No file loaded until fill_from_file() is called. */
    Calibration(const string &input_name);

    ~Calibration();

    /** Set the calibration name (e.g. for display or logging). */
    void set_name(const string &input_name);

    /**
     * Copy x (independent) knot coordinates from a std::vector into the internal Eigen vector.
     * Used after parsing file data. Call calculate_spline() after setting both x and y.
     */
    void fill_x_from_std_vector(const vector<double> &std_x_vector);

    /**
     * Copy y (dependent) knot coordinates from a std::vector into the internal Eigen vector.
     * Used after parsing file data. Call calculate_spline() after setting both x and y.
     */
    void fill_y_from_std_vector(const vector<double> &std_y_vector);

    /**
     * Load calibration from a file (e.g. .330 format).
     * Checks that the file exists and is readable, then parses header lines for
     * sensor_model, serial_number, interpolation_method, setpoint_limit, data_format,
     * break_points, and data lines for (x,y) points. Fills internal x/y vectors and
     * stores file_name. Does not call calculate_spline(); call that after fill_from_file().
     * \param input_file_name Path to the calibration file
     */
    void fill_from_file(const string &input_file_name);

    /**
     * Print calibration metadata (name, sensor model, serial number, etc.) and then
     * the spline knot/coefficient data from the base class. No-op if no file has been
     * loaded (file_name empty).
     */
    void print_calibration_info() const;
};

#endif
