#include <utility>
#include "Calibration_Class.h"

// --- File presence check (used by fill_from_file) ---

bool test_if_file_present(const string &file_name_string)
{
    bool is_file_present;
    std::string file_test_string;

    std::ifstream data_file_to_test{file_name_string};

    if (data_file_to_test.good())
    {
        std::cout << "File successfully opened" << std::endl;

        data_file_to_test >> file_test_string;
        if (data_file_to_test.fail())
        {
            std::cout << "Error: File could not be sucessfully read." << std::endl;
            is_file_present = false;
        }
        else
        {
            std::cout << "File successfully read" << std::endl << std::endl;
            is_file_present = true;
        }
    }
    else
    {
        std::cout << "Error: File could not be opened. Please check if file is present in same folder as program" << std::endl;
        is_file_present = false;
    }

    data_file_to_test.close();

    return is_file_present;
}

// --- Constructors / destructor ---

Calibration::Calibration()
{
    file_name = "";
    std::cout << "Calibration Created" << std::endl;
}

Calibration::Calibration(const string &input_name)
{
    name = input_name;
    file_name = "";
    std::cout << "Calibration " << name << " created" << std::endl;
}

Calibration::Calibration(const Calibration &copied_calibration)
{
    x_coordinates = copied_calibration.get_x_coordinates();
    y_coordinates = copied_calibration.get_y_coordinates();
    second_derivitive_vector = copied_calibration.get_second_derivitives();
    differences_vector = copied_calibration.get_differences();
    slope_vector = copied_calibration.get_slopes();
    previous_index = copied_calibration.get_previous_index();
    is_calibrated = copied_calibration.get_calibration_state();
    name = copied_calibration.get_name();
    sensor_model = copied_calibration.get_sensor_model();
    serial_number = copied_calibration.get_serial_number();
    interpolation_method = copied_calibration.get_interpolation_method();
    setpoint_limit = copied_calibration.get_setpoint_limit();
    data_format = copied_calibration.get_data_format();
    file_name = copied_calibration.get_file_name();
    break_points = copied_calibration.get_break_points();
    std::cout << "copy constructor called" << std::endl;
}

Calibration::Calibration(Calibration &&moved_calibration)
{
    x_coordinates = std::exchange(moved_calibration.x_coordinates, VectorXd());
    y_coordinates = std::exchange(moved_calibration.y_coordinates, VectorXd());
    second_derivitive_vector = std::exchange(moved_calibration.second_derivitive_vector, VectorXd());
    differences_vector = std::exchange(moved_calibration.differences_vector, VectorXd());
    slope_vector = std::exchange(moved_calibration.slope_vector, VectorXd());
    previous_index = std::exchange(moved_calibration.previous_index, 0);
    is_calibrated = std::exchange(moved_calibration.is_calibrated, false);
    name = std::exchange(moved_calibration.name, "");
    sensor_model = std::exchange(moved_calibration.sensor_model, "");
    serial_number = std::exchange(moved_calibration.serial_number, "");
    interpolation_method = std::exchange(moved_calibration.interpolation_method, "");
    setpoint_limit = std::exchange(moved_calibration.setpoint_limit, "");
    data_format = std::exchange(moved_calibration.data_format, "");
    file_name = std::exchange(moved_calibration.file_name, "");
    break_points = std::exchange(moved_calibration.break_points, 0);
    std::cout << "move constructor called" << std::endl;
}

Calibration::~Calibration()
{
    std::cout << "calibration " << name << " destroyed" << std::endl;
}

// --- operators ---

Calibration Calibration::operator=(const Calibration &copied_calibration)
{
    if (this == &copied_calibration)
    {
        return *this;
    }
    else
    {
        x_coordinates = copied_calibration.get_x_coordinates();
        y_coordinates = copied_calibration.get_y_coordinates();
        second_derivitive_vector = copied_calibration.get_second_derivitives();
        differences_vector = copied_calibration.get_differences();
        slope_vector = copied_calibration.get_slopes();
        previous_index = copied_calibration.get_previous_index();
        is_calibrated = copied_calibration.get_calibration_state();
        name = copied_calibration.get_name();
        sensor_model = copied_calibration.get_sensor_model();
        serial_number = copied_calibration.get_serial_number();
        interpolation_method = copied_calibration.get_interpolation_method();
        setpoint_limit = copied_calibration.get_setpoint_limit();
        data_format = copied_calibration.get_data_format();
        file_name = copied_calibration.get_file_name();
        break_points = copied_calibration.get_break_points();
        return *this;
    }
}

Calibration Calibration::operator=(Calibration &&moved_calibration)
{
    if (this == &moved_calibration)
    {
        return *this;
    }
    else
    {
        x_coordinates = std::exchange(moved_calibration.x_coordinates, VectorXd());
        y_coordinates = std::exchange(moved_calibration.y_coordinates, VectorXd());
        second_derivitive_vector = std::exchange(moved_calibration.second_derivitive_vector, VectorXd());
        differences_vector = std::exchange(moved_calibration.differences_vector, VectorXd());
        slope_vector = std::exchange(moved_calibration.slope_vector, VectorXd());
        previous_index = std::exchange(moved_calibration.previous_index, 0);
        is_calibrated = std::exchange(moved_calibration.is_calibrated, false);
        name = std::exchange(moved_calibration.name, "");
        sensor_model = std::exchange(moved_calibration.sensor_model, "");
        serial_number = std::exchange(moved_calibration.serial_number, "");
        interpolation_method = std::exchange(moved_calibration.interpolation_method, "");
        setpoint_limit = std::exchange(moved_calibration.setpoint_limit, "");
        data_format = std::exchange(moved_calibration.data_format, "");
        file_name = std::exchange(moved_calibration.file_name, "");
        break_points = std::exchange(moved_calibration.break_points, 0);
        return *this;
    }
}

// --- Setters ---

void Calibration::set_name(const string &input_name)
{
    name = input_name;
}

// --- getters ---

string Calibration::get_name() const {return name;}
string Calibration::get_sensor_model() const {return sensor_model;}
string Calibration::get_serial_number() const{return serial_number;}
string Calibration::get_interpolation_method() const{return interpolation_method;}
string Calibration::get_setpoint_limit() const{return setpoint_limit;}
string Calibration::get_data_format() const{return data_format;}
string Calibration::get_file_name() const{return file_name;}
int Calibration::get_break_points() const{return break_points;}

// --- Fill x/y from std::vector (used after parsing file data) ---

void Calibration::fill_x_from_std_vector(const vector<double> &std_x_vector)
{
    std::cout << "changing x vector" << std::endl;
    x_coordinates.resize(std_x_vector.size());
    for (int i{0}; i < std_x_vector.size(); ++i)
    {
        x_coordinates(i) = std_x_vector[i];
        std::cout << x_coordinates(i) << std::endl;
        std::cout << std_x_vector[i] << std::endl;
    }
}

void Calibration::fill_y_from_std_vector(const vector<double> &std_y_vector)
{
    std::cout << "changing y vector" << std::endl;
    y_coordinates.resize(std_y_vector.size());
    for (int i{0}; i < std_y_vector.size(); ++i)
    {
        y_coordinates(i) = std_y_vector[i];
    }
}

// --- Load calibration from file: parse header and data, fill x/y vectors ---

void Calibration::fill_from_file(const string &input_file_name)
{
    if (test_if_file_present(input_file_name) == false)
    {
        std::cout << "Calibration not performed." << std::endl;
        return;
    }

    file_name = input_file_name;

    string file_line_string;
    string sub_line_string;
    string temp_string;

    /* Regex objects to replace multi-space and colon delimiters with comma,
       so getline can split fields using ',' as the only delimiter. */
    std::regex data_file_delimiter_1(":");
    std::regex data_file_delimiter_2("          ");
    std::regex data_file_delimiter_3("         ");
    std::regex data_file_delimiter_4("        ");
    std::regex data_file_delimiter_5("       ");
    std::regex data_file_delimiter_6("      ");
    std::regex data_file_delimiter_7("     ");
    std::regex data_file_delimiter_8("    ");
    std::regex data_file_delimiter_9("   ");
    std::regex data_file_delimiter_10("  ");
    std::regex data_file_delimiter_11(" ");

    vector<string> temp_vector;
    vector<double> x_coordinates_stdvec;
    vector<double> y_coordinates_stdvec;

    std::ifstream data_file_to_read{input_file_name};

    int file_line_counter{0};
    int string_fill_counter{0};

    while (std::getline(data_file_to_read, file_line_string))
    {
        if (file_line_string[0] != ' ')
        {
            file_line_string = " " + file_line_string;
        }
        file_line_string = regex_replace(file_line_string, data_file_delimiter_1, ",");
        file_line_string = regex_replace(file_line_string, data_file_delimiter_2, ",");
        file_line_string = regex_replace(file_line_string, data_file_delimiter_3, ",");
        file_line_string = regex_replace(file_line_string, data_file_delimiter_4, ",");
        file_line_string = regex_replace(file_line_string, data_file_delimiter_5, ",");
        file_line_string = regex_replace(file_line_string, data_file_delimiter_6, ",");
        file_line_string = regex_replace(file_line_string, data_file_delimiter_7, ",");
        file_line_string = regex_replace(file_line_string, data_file_delimiter_8, ",");
        file_line_string = regex_replace(file_line_string, data_file_delimiter_9, ",");
        file_line_string = regex_replace(file_line_string, data_file_delimiter_10, ",");
        file_line_string = regex_replace(file_line_string, data_file_delimiter_11, ",");

        std::stringstream current_line_string_stream(file_line_string);

        while (std::getline(current_line_string_stream, sub_line_string, ','))
        {
            temp_vector.push_back(sub_line_string);
        }

        if (file_line_counter < 6)
        {
            if (temp_vector.size() > 3)
            {
                for (string_fill_counter = 4; string_fill_counter < temp_vector.size(); ++string_fill_counter)
                {
                    if (string_fill_counter == 4)
                    {
                        temp_string += temp_vector[string_fill_counter];
                    }
                    else
                    {
                        temp_string += " ";
                        temp_string += temp_vector[string_fill_counter];
                    }
                }

                if (file_line_counter == 0)
                {
                    sensor_model = temp_string;
                }
                else if (file_line_counter == 1)
                {
                    interpolation_method = temp_string;
                }
                else if (file_line_counter == 2)
                {
                    serial_number = temp_string;
                }
                else if (file_line_counter == 3)
                {
                    setpoint_limit = temp_string;
                }
                else if (file_line_counter == 4)
                {
                    data_format = temp_string;
                }
                else
                {
                    temp_string = regex_replace(temp_string, data_file_delimiter_11, "");
                    break_points = std::stoi(temp_string);
                }
                temp_string = "";
            }
        }
        if (file_line_counter > 8)
        {
            x_coordinates_stdvec.push_back(std::stod(temp_vector[2]));
            y_coordinates_stdvec.push_back(std::stod(temp_vector[3]));
        }

        temp_vector.clear();
        file_line_counter += 1;
    }
    data_file_to_read.close();

    fill_x_from_std_vector(x_coordinates_stdvec);
    fill_y_from_std_vector(y_coordinates_stdvec);
}

// --- Debug output: metadata and spline data ---

void Calibration::print_calibration_info() const
{
    if (file_name != "")
    {
        std::cout << "Calibration is named: " << name << std::endl;
        std::cout << "Sensor model: " << sensor_model << std::endl;
        std::cout << "Serial number: " << serial_number << std::endl;
        std::cout << "Interpolation method: " << interpolation_method << std::endl;
        std::cout << "Setpoint limit: " << setpoint_limit << std::endl;
        std::cout << "Data format: " << data_format << std::endl;
        std::cout << "File name: " << file_name << std::endl;
        std::cout << "Break points: " << break_points << std::endl;
    }
    else
    {
        std::cout << "No file info present" << std::endl;
    }

    print_spline_values();
}
