#ifndef C_SPLINE_CLASS_H //protects from particle.h being defined twice
#define C_SPLINE_CLASS_H

#include<iostream>
#include<cmath>
#include"Eigen/SparseLU"
#include"Eigen/OrderingMethods"

using Eigen::VectorXd;
using Eigen::SparseMatrix;
using Eigen::SparseLU;
using Eigen::COLAMDOrdering;

/**
 * Cubic spline interpolator for (x,y) control points.
 * Fits a natural cubic spline through the points, then interpolate_value(x) returns
 * the corresponding y at any x within the data range (e.g. voltage -> temperature from
 * a calibration curve). Typical use: set x/y coordinates (or use parametrised constructor),
 * call calculate_spline() once, then call interpolate_value(x) as needed.
 */
class C_Spline
{
    protected:
    VectorXd x_coordinates;           ///< Knot x values (independent variable)
    VectorXd y_coordinates;           ///< Knot y values (dependent variable)
    VectorXd second_derivitive_vector;///< Second derivatives at each knot (from spline solve)
    VectorXd differences_vector;     ///< x[i+1]-x[i] for each segment
    VectorXd slope_vector;           ///< Secant slopes (y[i+1]-y[i])/(x[i+1]-x[i]) per segment
    int previous_index;              ///< Cached segment index for repeated interpolations
    bool is_calibrated;              ///< True after calculate_spline() has been called

    public:

    /** Default constructor. Spline is not calibrated; set coordinates and call calculate_spline() before interpolate_value(). */
    C_Spline();

    /** Construct from knot vectors. Spline is not calibrated until calculate_spline() is called. */
    C_Spline(const VectorXd &x_coordinate_eigen, const VectorXd &y_coordinate_eigen);

    ~C_Spline();

    /** Set the x (independent) knot coordinates. Must match size of y_coordinates before calculate_spline(). */
    void set_x_coord(const VectorXd &input_vector_eigen);

    /** Set the y (dependent) knot coordinates. Must match size of x_coordinates before calculate_spline(). */
    void set_y_coord(const VectorXd &input_vector_eigen);

    /** Set differences vector (normally internal; used when storing spline coefficients). */
    void set_differences_vector(const VectorXd &input_vector_eigen);

    /** Set second-derivative vector (normally internal; used when storing spline coefficients). */
    void set_second_derivitive_vector(const VectorXd &input_vector_eigen);

    /** Set slope vector (normally internal; used when storing spline coefficients). */
    void set_slope_vector(const VectorXd &input_vector_eigen);

    /**
     * Fit the natural cubic spline through the current x/y coordinates.
     * Requires at least 3 points and equal-sized x and y vectors. Idempotent:
     * does nothing if already calibrated. Call this once after setting coordinates
     * and before calling interpolate_value().
     */
    void calculate_spline();

    /**
     * Interpolate: return the spline y value at the given x.
     * x_value_entered must lie within [min(x_coordinates), max(x_coordinates)].
     * Returns 0 and prints a message if the spline is not calibrated or x is out of range.
     * Efficient when many consecutive x values fall in the same segment (uses cached segment index).
     */
    double interpolate_value(const double& x_value_entered);

    /** Print all knot and spline coefficient vectors to stdout (for debugging). No-op if not calibrated. */
    void print_spline_values() const;
};

#endif
