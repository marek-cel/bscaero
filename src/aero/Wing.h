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

/** @author Marek Cel (marekcel@marekcel.pl) */
class Wing
{
public:

    /**
     * Wing::Wing()
     * Wing class constructor.
     */
    Wing( void );

    /**
     * Wing::~Wing()
     * Destructor. Releases results.
     */
    ~Wing( void );

    /**
     * bool Wing::addSectionData( double, double, double, double, double )
     * Adds new wing cross section data set to Wing::sections_data[ ] array
     * @return TRUE on success, FALSE on failure (given Wing Span Y Coordinate is NOT unique OR given
     * Leading Edge X Coordinate is greater than given Trailing Edge X Coordinate OR given
     * Wing Span Y Coordinate is less than 0.0)
     */
    bool addSectionData( double span_y, double le_x, double te_x, double slope, double angle );

    /**
     * bool Wing::compute()
     * Computes aerodynamic characteristics of the wing
     * @return TRUE on success, FALSE on failure
     */
    bool compute( void );

    /**
     * bool Wing::deleteSectionData( int )
     * Deletes wing cross section data set from Wing::sections_data[ ]
     * @return TRUE on success, FALSE on failure (row greater than length of Wing::sections_data[ ] array)
     */
    bool deleteSectionData( int row );

    /**
     * bool Wing::editSectionData( double, double, double, double, double, int )
     * Edits wing cross section data set in Wing::sections_data[ ]
     * @return TRUE on success, FALSE on failure (row greater than length of Wing::sections_data[ ] array OR
     * new Wing Span Y Coordinate is NOT unique OR given Leading Edge X Coordinate is
     * greater than given Trailing Edge X Coordinate OR given Wing Span Y Coordinate is
     * less than 0.0)
     */
    bool editSectionData( double span_y, double le_x, double te_x, double slope, double angle, int row );

    /**
     * double Wing::getAoADeg( int aoa )
     * @return Wing::angle_of_attack_deg[aoa]
     */
    double getAoADeg( int aoa );

    /**
     * double Wing::getAoA_Finish()
     * @return Wing::aoa_finish
     */
    double getAoA_Finish( void );

    /**
     * double Wing::getAoA_Iterations()
     * @return Wing::aoa_iterations
     */
    int getAoA_Iterations( void );

    /**
     * double Wing::getAoA_Start()
     * @returns Wing::aoa_start
     */
    double getAoA_Start( void );

    /**
     * double Wing::getAoA_Step()
     * @return Wing::aoa_step
     */
    double getAoA_Step( void );

    /**
     * ouble Wing::getAspectRatio()
     * @return Wing::aspect_ratio
     */
    double getAspectRatio( void );

    /**
     * double Wing::getDragCoef( int aoa )
     * @return Wing::induced_drag_coefficient[aoa]
     */
    double getDragCoef( int aoa );

    /**
     * double Wing::getDragCoefDist( int aoa, int section )
     * @return Wing::induced_drag_coefficient_distribution[aoa][section]
     */
    double getDragCoefDist( int aoa, int section );

    /**
     * double Wing::getGamma( int aoa, int section )
     * @return Wing::gamma[aoa][section]
     */
    double getGamma( int aoa, int section );

    /**
     * double Wing::getFluidDensity()
     * @return Wing::fluid_density
     */
    double getFluidDensity( void );

    /**
     * double Wing::getFluidVelocity()
     * @return Wing::fluid_velocity
     */
    double getFluidVelocity( void );

    /**
     * int Wing::getFourierAccuracy()
     * @return Wing::fourier_accuracy
     */
    int getFourierAccuracy( void );

    /**
     * double Wing::getLiftCoef( int aoa )
     * @return Wing::lift_coefficient
     */
    double getLiftCoef( int aoa );

    /**
     * double Wing::getLiftCoefDist( int aoa, int section )
     * @returns Wing::lift_coefficient_distribution[aoa][section]
     */
    double getLiftCoefDist( int aoa, int section );

    /**
     * int Wing::getSections()
     * @return Wing::sections
     */
    int getSections( void );

    /**
     * int Wing::getSectionsFullspan()
     * @return Wing::sections_fullspan
     */
    int getSectionsFullspan( void );

    /**
     * int Wing::getSectionsIterations()
     * @return Wing::sections_iterations
     */
    int getSectionsIterations( void );

    /**
     * double Wing::getSectionAngle( int )
     * @return Wing::sections_data[4][i]
     */
    double getSectionAngle( int i );

    /**
     * double Wing::getSectionsLEX( int )
     * @return Wing::sections_data[1][i]
     */
    double getSectionLEX( int i );

    /**
     * double Wing::getSectionSlope( int )
     * @return Wing::sections_data[3][i]
     */
    double getSectionSlope( int i );

    /**
     * double Wing::getSectionsTEX( int )
     * @return Wing::sections_data[2][i]
     */
    double getSectionTEX( int i );

    /**
     * double Wing::getSectionY( int )
     * @return Wing::sections_data[0][i]
     */
    double getSectionY( int i );

    /**
     * double Wing::getTaperRatio()
     * @return Wing::taper_ratio
     */
    double getTaperRatio( void );

    /**
     * double Wing::getVelocityDistribution( int aoa, int section )
     * @return Wing::induced_velocity_distribution[aoa][section]
     */
    double getVelocityDist( int aoa, int section );

    /**
     * double Wing::getWingArea()
     * @return Wing::wing_area
     */
    double getWingArea( void );

    /**
     * double Wing::getWingSpan()
     * @return Wing::wing_span
     */
    double getWingSpan( void );

    /**
     * double Wing::getWingSpanCoef( int section )
     * @return 2 * Wing::wingspan[section] / Wing::wing_span ( 2 * y / L )
     */
    double getWingSpanCoef( int section );

    /**
     * bool Wing::isParametersSaved()
     * @return Wing::parameters_saved
     */
    bool isParametersSaved( void );

    /**
     * bool Wing::isResultsUpToDate()
     * @return Wing::results_uptodate
     */
    bool isResultsUpToDate( void );

    /**
     * bool Wing::readFromFile( const char* file )
     * Reads wing geometry, computation parameters and results from file.
     * @return TRUE on success, FALSE on failure
     */
    bool readFromFile( const char* file );

    /**
     * bool Wing::setParametres( double, double, int, double, double, int, int )
     * Sets parameters and deletes Wing::equation_lhs array.
     * @return TRUE on success, FALSE on failure
     */
    bool setParameters( double a_start, double a_finish, int a_iterations, double f_velocity, double f_density, int s_iterations, int f_accuracy );

    /**
     * bool Wing::writeToDefaultFile()
     * Rewrites wing geometry, computation parameters and results to file if file exists (file_name exists).
     * @returns TRUE on success, FALSE on failure
     */
    bool writeToDefaultFile( void );

    /**
     * bool Wing::writeToFile( const char* file )
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
     * void Wing::calculateCharacteristics()
     * calculates aerodynamic charasteristics of the wing
     */
    void calculateCharacteristics( void );

    /** */
    void calculateGeometry( void );

    /**
     * bool Wing::checkIfSectionExists( double )
     * Checks if given Wing Span Y Coordinate already exists in Wing::sections_data.
     * @return TRUE if exists, FALSE if does NOT
     */
    bool checkIfSectionExists( double span_y );

    /**
     * void Wing::createEquations()
     * Creates square matrix Wing::equations_lhs of Fourier series terms' coefficients of order
     * Wing::fourier_accuracy.
     * sin( n * fi ) * ( sin( fi ) + n * mi( fi ) ) * An, where An is uknown variable
     * and matrix of equations' right hand side mi( fi ) * alpha_0 * sin( fi ), Wing::equations_rhs.
     */
    void createEquations( void );

    /**
     * void Wing::interpolateCrossSections()
     * Creates arrays of Cross Sections data using linear interpolations of all data:
     * - Leading Edge X Coordinate
     * - Trailing Edge X Coordinate
     * - Airfoil Lift Curve Slope
     * - Airfoil Angle of 0 Lift
     */
    void interpolateSections( void );

    /**
     * void Wing::iterateAoA()
     * Creates array of Angle of Attack.
     */
    void iterateAoA( void );

    /**
     * void Wing::releaseResults()
     * Releases memmory containing computation results this function is called before every new results computations.
     */
    void releaseResults( void );

    /**
     * void Wing::rewriteSectionsForFullSpan()
     * Rewrties Wing::wingspan, Wing:chord_leng_of_0_liftope, Wing::angle_of_0_lift_deg
     * and Wing::angle_of_0_lift_rad for full ( NOT semi ) wingspana and creates fi (Wing::fi) array
     */
    void rewriteSectionsForFullSpan( void );

    /**
     * bool Wing::solveEquations()
     * Solves system of linear equation due to assign values to equations variables
     * which are coefficients of Fourier series terms and calculate aerodynamic characteristics
     * @return TRUE on success, FALSE on failure
     */
    bool solveEquations( void );

    /**
     * void Wing::sortFullSpanSections()
     * Sorts wingspan, Wing::chord_length, Wing::lift_curve_sl_of_0_liftand Wing::angle_of_0_lift_deg
     * and Wing::angle_of_0_lift_rad using bubble sort algorithm
     */
    void sortFullSpanSections( void );

    /**
     * void Wing::sortSectionsData()
     * Sorts unsorted Wing::sections_data[ ] array using bubble sort algorithm.
     */
    void sortSectionsData( void );

    /**
     * bool Wing::writeAoAToFile()
     * Writes full span sections arrays to file.
     */
    bool writeAoAToFile( void );

    /**
     * bool Wing::writeEquationsToFile()
     * Writes Wing::equations_lhs and Wing::equations_rhs to file.
     */
    bool writeEquationsToFile( void );

    /**
     * bool Wing::writeFourierToFile( void )
     * Writes Wing::fourier_terms to file.
     */
    bool writeFourierToFile( void );

    /**
     * bool Wing::writeFullspanToFile()
     * Writes full span sections arrays to file.
     */
    bool writeFullspanToFile( void );

    /**
     * bool Wing::writeInterpolatedToFile()
     * Writes interpolated sections arrays to file.
     */
    bool writeInterpolatedToFile( void );

    /**
     * bool Wing::writeParametersToFile()
     * Writes parametres to file. */
    bool writeParametersToFile( void );

    /**
     * bool Wing::writeWingInputToFile()
     * Writes Wing::sections_data array to file.
     */
    bool writeWingInputToFile( void );

};

////////////////////////////////////////////////////////////////////////////////

#endif // WING_H
