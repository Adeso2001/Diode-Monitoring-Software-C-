#ifndef CALIBRATION_CLASS_H //protects from particle.h being defined twice
#define CALIBRATION_H

#include<iostream>
#include<cmath>
#include<string>
#include<vector>
#include<iomanip>
#include<fstream>
#include<sstream>
#include<regex>
#include"Eigen/SparseLU"
#include"Eigen/OrderingMethods"
#include"C_Spline_Class.h"

using std::string;
using std::vector;

bool test_if_file_present(const string &file_name_string)
{
  //declare variables
  bool is_file_present;
  std::string file_test_string;

  std::ifstream data_file_to_test{file_name_string}; //opens read only file

  //checks if file opened successfully and continues code if so
  if(data_file_to_test.good())
  {
    std::cout<<"File successfully opened"<<std::endl;

    //tests if data can be read successfully
    data_file_to_test>>file_test_string;
    if(data_file_to_test.fail())
    {
        std::cout<<"Error: File could not be sucessfully read."<<std::endl;
        is_file_present = false;
    }
    else
    {
      std::cout<<"File successfully read"<<std::endl<<std::endl;
      is_file_present = true;
    }
  }
  else
  {
    std::cout<<"Error: File could not be opened. Please check if file is present in same folder as program"<<std::endl;
    is_file_present = false;
  }
  
  data_file_to_test.close();

  return is_file_present;
}

class Calibration: public C_Spline
{
    protected:
    string name, sensor_model, serial_number, interpolation_method, setpoint_limit, data_format, file_name;
    int break_points;

    public:
    Calibration()
    {
        file_name = "";
        std::cout<<"Calibration Created"<<std::endl;
    }

    Calibration(const string &input_name)
    {
        name = input_name;
        file_name = "";
        std::cout<<"Calibration "<<name<<" created"<<std::endl;
    }
    ~Calibration(){std::cout<<"calibration "<<name<<" destroyed"<<std::endl;}
    
    void set_name(const string &input_name){name = input_name;}

    void fill_x_from_std_vector(const vector<double> &std_x_vector)
    {
        std::cout<<"changing x vector"<<std::endl;
        x_coordinates.resize((std_x_vector.size()));
        for(int i{0}; i < std_x_vector.size(); ++i){x_coordinates(i) = std_x_vector[i];
        std::cout<<x_coordinates(i)<<std::endl;
        std::cout<<std_x_vector[i]<<std::endl;}
    }

    void fill_y_from_std_vector(const vector<double> &std_y_vector)
    {
        std::cout<<"changing y vector"<<std::endl;
        y_coordinates.resize((std_y_vector.size()));
        for(int i{0}; i < std_y_vector.size(); ++i){y_coordinates(i) = std_y_vector[i];}   
    }

    void fill_from_file(const string &input_file_name)
    {
        if(test_if_file_present(input_file_name) == false)
        {
            std::cout<<"Calibration not performed."<<std::endl;
            return void();
        }

        file_name = input_file_name;

        string file_line_string;
        string sub_line_string;
        string temp_string;
            /* regex object needed to use replace 4 space delimiter with single character for getline, 
            as getline can only use single characters as delimiters */
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

        std::ifstream data_file_to_read{input_file_name}; // defines file stream to be read

        int file_line_counter{0};
        int string_fill_counter{0};

        while(std::getline(data_file_to_read,file_line_string)) // takes current line of file and moves onto next for next loop
        {
            if (file_line_string[0] != ' ')
            {
                file_line_string = " " + file_line_string;
            }
            file_line_string = regex_replace(file_line_string, data_file_delimiter_1, ","); // replaces ":" with "," in line
            file_line_string = regex_replace(file_line_string, data_file_delimiter_2, ","); // replaces "          " with "," in line
            file_line_string = regex_replace(file_line_string, data_file_delimiter_3, ","); // replaces "         " with "," in line
            file_line_string = regex_replace(file_line_string, data_file_delimiter_4, ","); // replaces "        " with "," in line
            file_line_string = regex_replace(file_line_string, data_file_delimiter_5, ","); // replaces "       " with "," in line
            file_line_string = regex_replace(file_line_string, data_file_delimiter_6, ","); // replaces "      " with "," in line
            file_line_string = regex_replace(file_line_string, data_file_delimiter_7, ","); // replaces "     " with "," in line
            file_line_string = regex_replace(file_line_string, data_file_delimiter_8, ","); // replaces "    " with "," in line
            file_line_string = regex_replace(file_line_string, data_file_delimiter_9, ","); // replaces "   " with "," in line
            file_line_string = regex_replace(file_line_string, data_file_delimiter_10, ",");// replaces "  " with "," in line
            file_line_string = regex_replace(file_line_string, data_file_delimiter_11, ",");// replaces " " with "," in line

            std::stringstream current_line_string_stream(file_line_string); // Uses file line to create string stream
            
            while(std::getline(current_line_string_stream,sub_line_string,',')) // takes current sub string using ',' as delimiter
            {
            temp_vector.push_back(sub_line_string); // stores sub-string entries in a temporary vector
            }
            
            if (file_line_counter < 6)
            {
                if (temp_vector.size()>3)
                {
                    for (string_fill_counter = 4;string_fill_counter<temp_vector.size();++string_fill_counter)
                    {
                        if(string_fill_counter == 4){temp_string += temp_vector[string_fill_counter];}
                        else
                        {
                            temp_string += " ";
                            temp_string += temp_vector[string_fill_counter];
                        } 
                    }

                    if(file_line_counter == 0){sensor_model = temp_string;}
                    else if(file_line_counter == 1){interpolation_method = temp_string;}
                    else if(file_line_counter == 2){serial_number = temp_string;}
                    else if(file_line_counter == 3){setpoint_limit = temp_string;}
                    else if(file_line_counter == 4){data_format= temp_string;}
                    else 
                    {
                        temp_string = regex_replace(temp_string, data_file_delimiter_11, "");// replaces " " with "" in line
                        break_points = std::stoi(temp_string);
                    }
                    temp_string = "";
                }
            }
            if (file_line_counter > 8) // point in the file where data points start being written
            {
                x_coordinates_stdvec.push_back((std::stod(temp_vector[2])));
                y_coordinates_stdvec.push_back((std::stod(temp_vector[3])));
            }
            
            temp_vector.clear(); //emptys temporary vector
            file_line_counter += 1;

        }
        data_file_to_read.close();

        this->fill_x_from_std_vector(x_coordinates_stdvec);
        this->fill_y_from_std_vector(y_coordinates_stdvec);
    }

    void const print_calibration_info()
    {
        if(file_name != "")
        {
            std::cout<<"Calibration is named: "<<name<<std::endl;
            std::cout<<"Sensor model: "<<sensor_model<<std::endl;
            std::cout<<"Serial number: "<<serial_number<<std::endl;
            std::cout<<"Interpolation method: "<<interpolation_method<<std::endl;
            std::cout<<"Setpoint limit: "<<setpoint_limit<<std::endl;
            std::cout<<"Data format: "<<data_format<<std::endl;
            std::cout<<"File name: "<<file_name<<std::endl;
            std::cout<<"Break points: "<<break_points<<std::endl;
        }
        else{std::cout<<"No file info present"<<std::endl;}

        print_spline_values();
    }
};


#endif