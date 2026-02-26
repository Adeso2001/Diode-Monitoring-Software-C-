#include "C_Spline_Class.h"

using Eigen::VectorXd;
using Eigen::SparseMatrix;
using Eigen::SparseLU;
using Eigen::COLAMDOrdering;

// --- Constructors / destructor ---

C_Spline::C_Spline()
{
    is_calibrated = false;
    previous_index = 0;
    std::cout << "default C_Spline constructor called" << std::endl;
}

C_Spline::C_Spline(const VectorXd &x_coordinate_eigen, const VectorXd &y_coordinate_eigen)
{
    x_coordinates.resize(x_coordinate_eigen.size());
    y_coordinates.resize(y_coordinate_eigen.size());
    x_coordinates = x_coordinate_eigen;
    y_coordinates = y_coordinate_eigen;
    is_calibrated = false;
    previous_index = 0;
    std::cout << "parametrised C_Spline constructor called" << std::endl;
}

C_Spline::C_Spline(const C_Spline &copied_spline)
{
    x_coordinates = copied_spline.get_x_coordinates();
    y_coordinates = copied_spline.get_y_coordinates();
    second_derivitive_vector = copied_spline.get_second_derivitives();
    differences_vector = copied_spline.get_differences();
    slope_vector = copied_spline.get_slopes();
    previous_index = copied_spline.get_previous_index();
    is_calibrated = copied_spline.get_calibration_state();
    std::cout << "copy constructor called" << std::endl;
}

C_Spline::C_Spline(C_Spline &&moved_spline)
{
    x_coordinates = std::exchange(moved_spline.x_coordinates, VectorXd());
    y_coordinates = std::exchange(moved_spline.y_coordinates, VectorXd());
    second_derivitive_vector = std::exchange(moved_spline.second_derivitive_vector, VectorXd());
    differences_vector = std::exchange(moved_spline.differences_vector, VectorXd());
    slope_vector = std::exchange(moved_spline.slope_vector, VectorXd());
    previous_index = std::exchange(moved_spline.previous_index, 0);
    is_calibrated = std::exchange(moved_spline.is_calibrated, false);
    std::cout << "move constructor called" << std::endl;
}

C_Spline::~C_Spline()
{
    std::cout << "Destroying C_Spline" << std::endl;
}

// --- operators ---

C_Spline C_Spline::operator=(const C_Spline &copied_spline)
{
    if (this == &copied_spline)
    {
        return *this;
    }
    else
    {
        x_coordinates = copied_spline.get_x_coordinates();
        y_coordinates = copied_spline.get_y_coordinates();
        second_derivitive_vector = copied_spline.get_second_derivitives();
        differences_vector = copied_spline.get_differences();
        slope_vector = copied_spline.get_slopes();
        previous_index = copied_spline.get_previous_index();
        is_calibrated = copied_spline.get_calibration_state();
        return *this;
    }
}

C_Spline C_Spline::operator=(C_Spline &&moved_spline)
{
    if (this == &moved_spline)
    {
        return *this;
    }
    else
    {
        x_coordinates = std::exchange(moved_spline.x_coordinates, VectorXd());
        y_coordinates = std::exchange(moved_spline.y_coordinates, VectorXd());
        second_derivitive_vector = std::exchange(moved_spline.second_derivitive_vector, VectorXd());
        differences_vector = std::exchange(moved_spline.differences_vector, VectorXd());
        slope_vector = std::exchange(moved_spline.slope_vector, VectorXd());
        previous_index = std::exchange(moved_spline.previous_index, 0);
        is_calibrated = std::exchange(moved_spline.is_calibrated, false);
        return *this;
    }
}

// --- getters ---
VectorXd C_spline::get_x_coordinates() const {return x_coordinates;}
VectorXd C_spline::get_y_coordinates() const {return y_coordinates;}
VectorXd C_spline::get_second_derivitives() const{return second_derivitive_vector;}
VectorXd C_spline::get_differences() const {return differences_vector;}
VectorXd C_spline::get_slopes() const {return slope_vector;}

int C_spline::get_previous_index() const {return previous_index}
bool C_spline::get_calibration_state() const{return is_calibrated;}

// --- Setters (for coordinates and internal coefficient vectors) ---

void C_Spline::set_x_coord(const VectorXd &input_vector_eigen)
{
    x_coordinates.resize(input_vector_eigen.size());
    x_coordinates = input_vector_eigen;
}

void C_Spline::set_y_coord(const VectorXd &input_vector_eigen)
{
    y_coordinates.resize(input_vector_eigen.size());
    y_coordinates = input_vector_eigen;
}

void C_Spline::set_differences_vector(const VectorXd &input_vector_eigen)
{
    differences_vector = input_vector_eigen;
}

void C_Spline::set_second_derivitive_vector(const VectorXd &input_vector_eigen)
{
    second_derivitive_vector = input_vector_eigen;
}

void C_Spline::set_slope_vector(const VectorXd &input_vector_eigen)
{
    slope_vector = input_vector_eigen;
}

// --- Spline fitting: build tridiagonal system and solve for second derivatives ---

void C_Spline::calculate_spline()
{
    SparseMatrix<double> matrix_of_equations((x_coordinates.size()), (x_coordinates.size()));
    VectorXd rhs_vector((x_coordinates.size())); // for use in matrix calculation
    VectorXd temp_second_derivitive_vector((x_coordinates.size()));
    VectorXd temp_differences_vector((x_coordinates.size() - 1));
    VectorXd temp_slope_vector((x_coordinates.size() - 1));
    SparseLU<SparseMatrix<double>, COLAMDOrdering<int>> solver;

    if ((x_coordinates.size() < 3) || (y_coordinates.size() < 3) || (x_coordinates.size() != y_coordinates.size()))
    {
        std::cout << "x and y coordinates dont meet requirements" << std::endl;
        return; //end function early
    }

    if (is_calibrated == true)
    {
        std::cout << "spline already calculated" << std::endl;
        return; //end function early
    }

    for (int i{0}; (i + 1) < x_coordinates.size(); ++i) //fill vectors
    {
        temp_differences_vector(i) = x_coordinates((i + 1)) - x_coordinates(i); // difference between subsequent x value
        temp_slope_vector(i) = (y_coordinates((i + 1)) - y_coordinates(i)) / (x_coordinates((i + 1)) - x_coordinates(i)); //diffences between subsequent y values divided by diffences between subsequent x values
    }

    for (int h{0}; h < x_coordinates.size(); ++h) // fill matrix + vector used to solve for second derivitive (matrix solves for cubic spline coefficients)
    {
        if (h == 0) // for first row of matrix...
        {
            matrix_of_equations.insert(0, 0) = temp_differences_vector((h + 1));
            matrix_of_equations.insert(0, 1) = -temp_differences_vector(h) - temp_differences_vector((h + 1));
            matrix_of_equations.insert(0, 2) = temp_differences_vector(h);
            rhs_vector(0) = 0;
        }
        else if ((h + 1) == x_coordinates.size()) //for final row of matrix
        {
            rhs_vector(h) = 0;
            matrix_of_equations.insert(h, (h - 2)) = temp_differences_vector((h - 1));
            matrix_of_equations.insert(h, (h - 1)) = -temp_differences_vector((h - 1)) - temp_differences_vector((h - 2));
            matrix_of_equations.insert(h, h) = temp_differences_vector((h - 2));
        }
        else
        {
            rhs_vector(h) = 6 * (temp_slope_vector(h) - temp_slope_vector((h - 1)));
            matrix_of_equations.insert(h, (h - 1)) = temp_differences_vector((h - 1));
            matrix_of_equations.insert(h, h) = 2 * (temp_differences_vector(h) + temp_differences_vector((h - 1)));
            matrix_of_equations.insert(h, (h + 1)) = temp_differences_vector(h);
        }
    }

    solver.analyzePattern(matrix_of_equations);
    solver.factorize(matrix_of_equations); //puts matrix into factors which can then be solved
    temp_second_derivitive_vector = solver.solve(rhs_vector); //finds vector of second derivitives from rhs_vector and using matrix factors

    //resizes member vectors
    second_derivitive_vector.resize((x_coordinates.size()));
    slope_vector.resize((x_coordinates.size() - 1));
    differences_vector.resize((x_coordinates.size() - 1));

    //fills member vectors
    set_second_derivitive_vector(temp_second_derivitive_vector);
    set_differences_vector(temp_differences_vector);
    set_slope_vector(temp_slope_vector);

    is_calibrated = true;
}

// --- Interpolation: find segment and evaluate cubic polynomial ---

double C_Spline::interpolate_value(const double &x_value_entered)
{
    if (is_calibrated == false) // ends function if not currently calibrated (would probably be better to do this using throw/exception at some point)
    {
        std::cout << "C_Spline object not currently calibrated" << std::endl;
        return 0;
    }

    int current_index{0};
    bool is_index_found{false};
    double s_0;
    double s_1;
    double s_2;
    double s_3;
    double xdif;

    double max_x_value;
    double min_x_value;

    max_x_value = x_coordinates.maxCoeff();
    min_x_value = x_coordinates.minCoeff();

    if ((x_value_entered > max_x_value) || (x_value_entered < min_x_value))
    {
        std::cout << "Value out of bounds: " << x_value_entered << " " << max_x_value << " " << min_x_value << std::endl;
        return 0;
    }

    if ((x_value_entered >= (x_coordinates(previous_index))) && (x_value_entered < (x_coordinates((previous_index + 1))))) //skips searching for index if identical to previous interpolation event
    {
        current_index = previous_index;
    }
    else
    {
        for (int index{0}; is_index_found == false; ++index) //runs loop to find index matching polynomial
        {
            if ((index + 1) == x_coordinates.size())
            {
                is_index_found = true;
                current_index = index + 1;
            }
            else if ((x_value_entered >= x_coordinates(index)) && (x_value_entered < x_coordinates((index + 1))))
            {
                is_index_found = true;
                previous_index = index;
                current_index = index;
            }
        }

        if (current_index == x_coordinates.size())
        {
            std::cout << "Value out of bounds" << std::endl;
            return 0;
        }
    }

    // Needs to calculate the 4 coefficients for the cubic equation defining the spline in question
    s_0 = y_coordinates(current_index);
    s_1 = slope_vector(current_index) - (differences_vector(current_index) / 6) * (2 * second_derivitive_vector(current_index) + second_derivitive_vector((current_index + 1)));
    s_2 = second_derivitive_vector(current_index) / 2;
    s_3 = (second_derivitive_vector((current_index + 1)) - second_derivitive_vector(current_index)) / (6 * differences_vector(current_index));
    xdif = x_value_entered - x_coordinates(current_index); // length along the spline

    return (s_0 + s_1 * xdif + s_2 * pow(xdif, 2) + s_3 * pow(xdif, 3));
}

// --- Debug output ---

void C_Spline::print_spline_values() const
{
    int index;

    if (is_calibrated == false)
    {
        std::cout << "Must calculate spline to print spline data" << std::endl;
        return;
    }

    std::cout << "x coordinate vector:" << std::endl << "(";

    for (index = 0; index < x_coordinates.size(); ++index)
    {
        if (index == 0)
        {
            std::cout << x_coordinates(index);
        }
        else
        {
            std::cout << "," << std::endl << x_coordinates(index);
        }
    }
    std::cout << ")" << std::endl << std::endl;

    std::cout << "y coordinate vector:" << std::endl << "(";

    for (index = 0; index < y_coordinates.size(); ++index)
    {
        if (index == 0)
        {
            std::cout << y_coordinates(index);
        }
        else
        {
            std::cout << "," << std::endl << y_coordinates(index);
        }
    }
    std::cout << ")" << std::endl << std::endl;

    std::cout << "slope vector:" << std::endl << "(";

    for (index = 0; index < slope_vector.size(); ++index)
    {
        if (index == 0)
        {
            std::cout << slope_vector(index);
        }
        else
        {
            std::cout << "," << std::endl << slope_vector(index);
        }
    }
    std::cout << ")" << std::endl << std::endl;

    std::cout << "differences vector:" << std::endl << "(";

    for (index = 0; index < differences_vector.size(); ++index)
    {
        if (index == 0)
        {
            std::cout << differences_vector(index);
        }
        else
        {
            std::cout << "," << std::endl << differences_vector(index);
        }
    }
    std::cout << ")" << std::endl << std::endl;

    std::cout << "second derivitive vector:" << std::endl << "(";

    for (index = 0; index < second_derivitive_vector.size(); ++index)
    {
        if (index == 0)
        {
            std::cout << second_derivitive_vector(index);
        }
        else
        {
            std::cout << "," << std::endl << second_derivitive_vector(index);
        }
    }
    std::cout << ")" << std::endl << std::endl;
}
