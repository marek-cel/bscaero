/****************************************************************************//*
 * Copyright (C) 2021 Marek M. Cel
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 ******************************************************************************/
#ifndef WING_H
#define WING_H

////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <math.h>
#include <stdio.h>
#include <string.h>

////////////////////////////////////////////////////////////////////////////////

/** @author Marek Cel (mcel@mcel.pl) */
class BSc_Wing {

private:


    
public:

    /**
     * BSc_BSc_Wing::BSc_Wing()
     * Wing class constructor.
     */
    BSc_Wing( void );

    /**
     * BSc_BSc_Wing::~BSc_Wing()
     * Destructor. Releases results.
     */
    ~BSc_Wing( void );

    /**
     * bool BSc_Wing::addSectionData( double, double, double, double, double )
     * Adds new wing cross section data set to BSc_Wing::sections_data[ ] array
     * @return TRUE on success, FALSE on failure (given Wing Span Y Coordinate is NOT unique OR given
     * Leading Edge X Coordinate is greater than given Trailing Edge X Coordinate OR given
     * Wing Span Y Coordinate is less than 0.0)
     */
    bool addSectionData( double span_y, double le_x, double te_x, double slope, double angle );

    /**
     * bool BSc_Wing::compute()
     * Computes aerodynamic characteristics of the wing
     * @return TRUE on success, FALSE on failure
     */
    bool compute( void );

    /**
     * bool BSc_Wing::deleteSectionData( int )
     * Deletes wing cross section data set from BSc_Wing::sections_data[ ]
     * @return TRUE on success, FALSE on failure (row greater than length of BSc_Wing::sections_data[ ] array)
     */
    bool deleteSectionData( int row );

    /**
     * bool BSc_Wing::editSectionData( double, double, double, double, double, int )
     * Edits wing cross section data set in BSc_Wing::sections_data[ ]
     * @return TRUE on success, FALSE on failure (row greater than length of BSc_Wing::sections_data[ ] array OR
     * new Wing Span Y Coordinate is NOT unique OR given Leading Edge X Coordinate is
     * greater than given Trailing Edge X Coordinate OR given Wing Span Y Coordinate is
     * less than 0.0)
     */
    bool editSectionData( double span_y, double le_x, double te_x, double slope, double angle, int row );

    /**
     * double BSc_Wing::getAoADeg( int aoa )
     * @return BSc_Wing::angle_of_attack_deg[aoa]
     */
    double getAoADeg( int aoa );

    /**
     * double BSc_Wing::getAoA_Finish()
     * @return BSc_Wing::aoa_finish
     */
    double getAoA_Finish( void );

    /**
     * double BSc_Wing::getAoA_Iterations()
     * @return BSc_Wing::aoa_iterations
     */
    int getAoA_Iterations( void );

    /**
     * double BSc_Wing::getAoA_Start()
     * @returns BSc_Wing::aoa_start
     */
    double getAoA_Start( void );

    /**
     * double BSc_Wing::getAoA_Step()
     * @return BSc_Wing::aoa_step
     */
    double getAoA_Step( void );

    /**
     * ouble BSc_Wing::getAspectRatio()
     * @return BSc_Wing::aspect_ratio
     */
    double getAspectRatio( void );

    /**
     * double BSc_Wing::getDragCoef( int aoa )
     * @return BSc_Wing::induced_drag_coefficient[aoa]
     */
    double getDragCoef( int aoa );

    /**
     * double BSc_Wing::getDragCoefDist( int aoa, int section )
     * @return BSc_Wing::induced_drag_coefficient_distribution[aoa][section]
     */
    double getDragCoefDist( int aoa, int section );

    /**
     * double BSc_Wing::getGamma( int aoa, int section )
     * @return BSc_Wing::gamma[aoa][section]
     */
    double getGamma( int aoa, int section );

    /**
     * double BSc_Wing::getFluidDensity()
     * @return BSc_Wing::fluid_density
     */
    double getFluidDensity( void );

    /**
     * double BSc_Wing::getFluidVelocity()
     * @return BSc_Wing::fluid_velocity
     */
    double getFluidVelocity( void );

    /**
     * int BSc_Wing::getFourierAccuracy()
     * @return BSc_Wing::fourier_accuracy
     */
    int getFourierAccuracy( void );

    /**
     * double BSc_Wing::getLiftCoef( int aoa )
     * @return BSc_Wing::lift_coefficient
     */
    double getLiftCoef( int aoa );

    /**
     * double BSc_Wing::getLiftCoefDist( int aoa, int section )
     * @returns BSc_Wing::lift_coefficient_distribution[aoa][section]
     */
    double getLiftCoefDist( int aoa, int section );

    /**
     * int BSc_Wing::getSections()
     * @return BSc_Wing::sections
     */
    int getSections( void );

    /**
     * int BSc_Wing::getSectionsFullspan()
     * @return BSc_Wing::sections_fullspan
     */
    int getSectionsFullspan( void );

    /**
     * int BSc_Wing::getSectionsIterations()
     * @return BSc_Wing::sections_iterations
     */
    int getSectionsIterations( void );

    /**
     * double BSc_Wing::getSectionAngle( int )
     * @return BSc_Wing::sections_data[4][i]
     */
    double getSectionAngle( int i );

    /**
     * double BSc_Wing::getSectionsLEX( int )
     * @return BSc_Wing::sections_data[1][i]
     */
    double getSectionLEX( int i );

    /**
     * double BSc_Wing::getSectionSlope( int )
     * @return BSc_Wing::sections_data[3][i]
     */
    double getSectionSlope( int i );

    /**
     * double BSc_Wing::getSectionsTEX( int )
     * @return BSc_Wing::sections_data[2][i]
     */
    double getSectionTEX( int i );

    /**
     * double BSc_Wing::getSectionY( int )
     * @return BSc_Wing::sections_data[0][i]
     */
    double getSectionY( int i );

    /**
     * double BSc_Wing::getTaperRatio()
     * @return BSc_Wing::taper_ratio
     */
    double getTaperRatio( void );

    /**
     * double BSc_Wing::getVelocityDistribution( int aoa, int section )
     * @return BSc_Wing::induced_velocity_distribution[aoa][section]
     */
    double getVelocityDist( int aoa, int section );

    /**
     * double BSc_Wing::getWingArea()
     * @return BSc_Wing::wing_area
     */
    double getWingArea( void );

    /**
     * double BSc_Wing::getWingSpan()
     * @return BSc_Wing::wing_span
     */
    double getWingSpan( void );

    /**
     * double BSc_Wing::getWingSpanCoef( int section )
     * @return 2 * BSc_Wing::wingspan[section] / BSc_Wing::wing_span ( 2 * y / L )
     */
    double getWingSpanCoef( int section );

    /**
     * bool BSc_Wing::isParametersSaved()
     * @return BSc_Wing::parameters_saved
     */
    bool isParametersSaved( void );

    /**
     * bool BSc_Wing::isResultsUpToDate()
     * @return BSc_Wing::results_uptodate
     */
    bool isResultsUpToDate( void );

    /**
     * bool BSc_Wing::readFromFile( const char* file )
     * Reads wing geometry, computation parameters and results from file.
     * @return TRUE on success, FALSE on failure
     */
    bool readFromFile( const char* file );

    /**
     * bool BSc_Wing::setParametres( double, double, int, double, double, int, int )
     * Sets parameters and deletes BSc_Wing::equation_lhs array.
     * @return TRUE on success, FALSE on failure
     */
    bool setParameters( double a_start, double a_finish, int a_iterations, double f_velocity, double f_density, int s_iterations, int f_accuracy );

    /**
     * bool BSc_Wing::writeToDefaultFile()
     * Rewrites wing geometry, computation parameters and results to file if file exists (file_name exists).
     * @returns TRUE on success, FALSE on failure
     */
    bool writeToDefaultFile( void );

    /**
     * bool BSc_Wing::writeToFile( const char* file )
     * Writes wing geometry, computation parameters and results to file.
     * @returns TRUE on success, FALSE on failure
     */
    bool writeToFile( const char* file );

private:

    const double pi;                ///< pi number 3.14....
    char     file_name[400];        ///<
    double   *sections_data[5];     ///<
    double   wing_span;             ///<
    double   wing_area;             ///<
    double   aspect_ratio;          ///<
    double   taper_ratio;           ///<
    double   aoa_start;             ///<
    double   aoa_finish;            ///<
    double   aoa_step;              ///<
    int      aoa_iterations;        ///<
    double   fluid_velocity;        ///<
    double   fluid_density;         ///<
    int      sections_iterations;   ///<
    int      sections_fullspan;     ///<
    int      fourier_accuracy;      ///<
    int      old_accuracy;          ///<
    int      old_aoa_iterations;    ///<
    int      sections;              ///<
    bool     results_uptodate;      ///<
    bool     parameters_saved;      ///<

    // COMPUTATIONS RESULTS

    double   *angle_of_attack_deg;  ///< [deg]
    double   *angle_of_attack_rad;  ///< [rad]
    double   *wingspan;             ///<
    double   *chord_length;         ///<
    double   *lift_curve_slope;     ///<
    double   *angle_of_0_lift_deg;  ///< [deg] angle of zero lift
    double   *angle_of_0_lift_rad;  ///< [rad] angle of zero lift
    double   *phi;                  ///<
    double   *mu;                   ///<
    double   **equations_lhs;       ///<
    double   **equations_rhs;       ///<
    double   *equations_span;       ///<
    double   **fourier_terms;       ///<
    double   **gamma;               ///<
    double   **lift_coefficient_distribution;         ///<
    double   **induced_velocity_distribution;         ///<
    double   **induced_drag_coefficient_distribution; ///<
    double   *lift_coefficient;         ///<
    double   *induced_drag_coefficient; ///<

    /**
     * void BSc_Wing::calculateCharacteristics()
     * calculates aerodynamic charasteristics of the wing
     */
    void calculateCharacteristics( void );

    /** */
    void calculateGeometry( void );

    /**
     * bool BSc_Wing::checkIfSectionExists( double )
     * Checks if given Wing Span Y Coordinate already exists in BSc_Wing::sections_data.
     * @return TRUE if exists, FALSE if does NOT
     */
    bool checkIfSectionExists( double span_y );

    /**
     * void BSc_Wing::createEquations()
     * Creates square matrix BSc_Wing::equations_lhs of Fourier series terms' coefficients of order
     * BSc_Wing::fourier_accuracy.
     * sin( n * fi ) * ( sin( fi ) + n * mi( fi ) ) * An, where An is uknown variable
     * and matrix of equations' right hand side mi( fi ) * alpha_0 * sin( fi ), BSc_Wing::equations_rhs.
     */
    void createEquations( void );

    /**
     * void BSc_Wing::interpolateCrossSections()
     * Creates arrays of Cross Sections data using linear interpolations of all data:
     * - Leading Edge X Coordinate
     * - Trailing Edge X Coordinate
     * - Airfoil Lift Curve Slope
     * - Airfoil Angle of 0 Lift
     */
    void interpolateSections( void );

    /**
     * void BSc_Wing::iterateAoA()
     * Creates array of Angle of Attack.
     */
    void iterateAoA( void );

    /**
     * void BSc_Wing::releaseResults()
     * Releases memmory containing computation results this function is called before every new results computations.
     */
    void releaseResults( void );

    /**
     * void BSc_Wing::rewriteSectionsForFullSpan()
     * Rewrties BSc_Wing::wingspan, Wing:chord_leng_of_0_liftope, BSc_Wing::angle_of_0_lift_deg
     * and BSc_Wing::angle_of_0_lift_rad for full ( NOT semi ) wingspana and creates fi (BSc_Wing::fi) array
     */
    void rewriteSectionsForFullSpan( void );

    /**
     * bool BSc_Wing::solveEquations()
     * Solves system of linear equation due to assign values to equations variables
     * which are coefficients of Fourier series terms and calculate aerodynamic characteristics
     * @return TRUE on success, FALSE on failure
     */
    bool solveEquations( void );

    /**
     * void BSc_Wing::sortFullSpanSections()
     * Sorts wingspan, BSc_Wing::chord_length, BSc_Wing::lift_curve_sl_of_0_liftand BSc_Wing::angle_of_0_lift_deg
     * and BSc_Wing::angle_of_0_lift_rad using bubble sort algorithm
     */
    void sortFullSpanSections( void );

    /**
     * void BSc_Wing::sortSectionsData()
     * Sorts unsorted BSc_Wing::sections_data[ ] array using bubble sort algorithm.
     */
    void sortSectionsData( void );

    /**
     * bool BSc_Wing::writeAoAToFile()
     * Writes full span sections arrays to file.
     */
    bool writeAoAToFile( void );

    /**
     * bool BSc_Wing::writeEquationsToFile()
     * Writes BSc_Wing::equations_lhs and BSc_Wing::equations_rhs to file.
     */
    bool writeEquationsToFile( void );

    /**
     * bool BSc_Wing::writeFourierToFile( void )
     * Writes BSc_Wing::fourier_terms to file.
     */
    bool writeFourierToFile( void );

    /**
     * bool BSc_Wing::writeFullspanToFile()
     * Writes full span sections arrays to file.
     */
    bool writeFullspanToFile( void );

    /**
     * bool BSc_Wing::writeInterpolatedToFile()
     * Writes interpolated sections arrays to file.
     */
    bool writeInterpolatedToFile( void );

    /**
     * bool BSc_Wing::writeParametersToFile()
     * Writes parametres to file. */
    bool writeParametersToFile( void );

    /**
     * bool BSc_Wing::writeWingInputToFile()
     * Writes BSc_Wing::sections_data array to file.
     */
    bool writeWingInputToFile( void );

};

////////////////////////////////////////////////////////////////////////////////

#endif // WING_H
