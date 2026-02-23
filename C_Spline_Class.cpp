#include<iostream>
#include<cmath>
#include"Eigen/SparseLU"
#include"Eigen/OrderingMethods"

using Eigen::VectorXd;
using Eigen::SparseMatrix;
using Eigen::SparseLU;
using Eigen::COLAMDOrdering;

class C_Spline
{
    protected:
    VectorXd x_coordinates;
    VectorXd y_coordinates;
    VectorXd second_derivitive_vector;
    VectorXd differences_vector;
    VectorXd slope_vector;
    int previous_index;
    bool is_calibrated;

    public:
     
    C_Spline() 
    {
        is_calibrated = false;
        previous_index = 0;
        std::cout<<"default C_Spline constructor called"<<std::endl;
    }

    C_Spline(const VectorXd &x_coordinate_eigen, const VectorXd &y_coordinate_eigen) 
    {
        x_coordinates = x_coordinate_eigen;
        y_coordinates = y_coordinate_eigen;
        is_calibrated = false;
        previous_index = 0;
        std::cout<<"parametrised C_Spline constructor called"<<std::endl;
    }


    void calculate_spline()
    {
        SparseMatrix<double> matrix_of_equations((x_coordinates.size()),(x_coordinates.size()));
        VectorXd rhs_vector((x_coordinates.size())); // for use in matrix calculation
        VectorXd temp_second_derivitive_vector((x_coordinates.size()));
        VectorXd temp_differences_vector((x_coordinates.size()-1));
        VectorXd temp_slope_vector((x_coordinates.size()-1));
        SparseLU<SparseMatrix<double>, COLAMDOrdering<int> >   solver;

        if((x_coordinates.size()<3) || (y_coordinates.size()<3) || (x_coordinates.size()!=y_coordinates.size()))
        {
            std::cout<<"x and y coordinates dont meet requirements"<<std::endl;
            return void(); //end function early
        }

        if(is_calibrated == true)
        {
            std::cout<<"spline already calculated"<<std::endl;
            return void(); //end function early
        }

        for(int i{0}; (i+1)<x_coordinates.size(); ++i) //fill vectors
        {
            temp_differences_vector(i) = x_coordinates((i+1)) - x_coordinates(i);
            temp_slope_vector(i) = (y_coordinates((i+1)) - y_coordinates(i)) / (x_coordinates((i+1)) - x_coordinates(i));
        }

        for(int h{0}; h<x_coordinates.size(); ++h) // fill matrix + vector used to solve for second derivitive
        {
            if(h==0) // for first row of matrix...
            {
                matrix_of_equations.insert(0,0) = temp_differences_vector((h+1));
                matrix_of_equations.insert(0,1) = - temp_differences_vector(h) - temp_differences_vector((h+1));
                matrix_of_equations.insert(0,2) = temp_differences_vector(h);
                rhs_vector(0) = 0;
            }
            else if((h+1) == x_coordinates.size()) //for final row of matrix
            {
                rhs_vector(h) = 0;
                matrix_of_equations.insert(h,(h-2)) = temp_differences_vector((h-1));
                matrix_of_equations.insert(h,(h-1)) = - temp_differences_vector((h-1)) - temp_differences_vector((h-2));
                matrix_of_equations.insert(h,h) = temp_differences_vector((h-2));
            }
            else
            {
                rhs_vector(h) = 6 * (temp_slope_vector(h) - temp_slope_vector((h-1)));
                matrix_of_equations.insert(h,(h-1)) = differences_vector((h-1));
                matrix_of_equations.insert(h,h) = 2 * (differences_vector(h) + differences_vector((h-1)));
                matrix_of_equations.insert(h,(h+1)) = differences_vector(h);
            }
        }

        solver.analyzePattern(matrix_of_equations);
        solver.factorize(matrix_of_equations);
        temp_second_derivitive_vector = solver.solve(rhs_vector);

        second_derivitive_vector = temp_second_derivitive_vector;
        differences_vector = temp_differences_vector;
        slope_vector = temp_slope_vector;
    }

    double interpolate_value()
    {
        return 1;
    }
};