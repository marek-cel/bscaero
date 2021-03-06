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

#include <Wing.h>

#include <fstream>

#include <GaussJordan.h>

////////////////////////////////////////////////////////////////////////////////

Wing::Wing() :
    pi( 4 * atan( 1.0 ) )
{
    // setting file_name
    file_name[0] = '\0';
    
    // angle of attack iteration data
    aoa_start      = 0.0;
    aoa_finish     = 0.0;
    aoa_step       = 0.0;
    aoa_iterations = 1;
    
    // fluid data
    fluid_velocity = 0.0;
    fluid_density  = 1.225; // density of the air at 0 Celsius and 1013.25 hPa
    
    // sections data
    sections            = 0;
    sections_iterations = 2;
    
    // Fourier series data
    fourier_accuracy   = 1;
    old_accuracy       = 0;
    old_aoa_iterations = 0;
    
    // setting pointers at null address
    wingspan            = 0;
    chord_length        = 0;
    lift_curve_slope    = 0;
    angle_of_0_lift_deg = 0;
    angle_of_0_lift_rad = 0;
    phi = 0;
    mu  = 0;
    
    equations_lhs  = 0;
    equations_rhs  = 0;
    equations_span = 0;
    fourier_terms  = 0;
    gamma          = 0;
    lift_coefficient_distribution = 0;
    induced_velocity_distribution = 0;
    induced_drag_coefficient_distribution = 0;
    lift_coefficient         = 0;
    induced_drag_coefficient = 0;
    
    // set *sections_data pointer at NULL
    for ( int i = 0; i < 5; i++ )
    {
        sections_data[i] = 0;
    }
    // setting results_uptodate as FALSE on object start - no results at all
    results_uptodate = false;
    parameters_saved = false;
}

////////////////////////////////////////////////////////////////////////////////

Wing::~Wing()
{
    releaseResults() ;
}

////////////////////////////////////////////////////////////////////////////////

bool Wing::addSectionData( double span_y, double le_x, double te_x, double slope, double angle )
{    
    double* new_sections_data[5] ;
    double data_set[5] = { span_y, le_x, te_x, slope, angle } ;
    int i, j ;
    
    // first cross section must be wing root (Y Coordinate = 0)
    if ( sections == 0 && span_y != 0.0)
        return false ;
    
    // check if given Wing Span Y Coordinate is less than 0.0
    if ( span_y < 0.0 )
        // if so return FALSE on failure
        return false ;
    
    // check if given Trailing Edge X Coordinate is greater than given Leading Edge X Coordinate
    if ( te_x <= le_x )
        // if NOT return FALSE on failure
        return false ;
    
    // check if new Wing Span Y Coordinate already exists in Wing::sections_data[ ] array
    if ( checkIfSectionExists( span_y ) )
        // if exists return FALSE on failure
        return false ;
    
    // For every value in set (Wing Span Y Coordinate, Leading Edge X Coordinate, etc.)
    for ( i = 0; i < 5; i++ )
    {
        // create new array of length greater by one than current
        new_sections_data[i] = new double[sections + 1] ;

        // For every current existing sections
        for ( j = 0; j < sections; j++ )
        {
            // copy value into new array
            new_sections_data[i][j] = sections_data[i][j] ;
        }

        // copy new value into last element of the new array
        new_sections_data[i][sections] = data_set[i] ;

        // delete current array
        delete [] sections_data[i] ;
        // point at new array
        sections_data[i] = new_sections_data[i] ;
    }
    
    // Increment Wing::sections by one
    sections++ ;
    
    // sort new Wing::sections_data[ ]  array
    sortSectionsData() ;
    
    // recalculate Wing Geometric Data
    calculateGeometry() ;
    
    // write Wing::sections_data array to file
    writeWingInputToFile() ;
    
    // setting Wing::results_uptodate as FALSE
    results_uptodate = false ;
    
    // return TRUE on success
    return true ;
}

////////////////////////////////////////////////////////////////////////////////

bool Wing::compute( void )
{
    bool ret = true;
    
    // if too few sections
    if ( sections < 2 ) return false;
    
    // if there is no parameters saved
    if ( ! parameters_saved ) return false;
    
    // releasing results
    releaseResults() ;
    
#   ifdef _DEBUG
    // write parameters to file
    writeParametersToFile();
#   endif
    
    // iterate Angle of Attack at first
    iterateAoA() ;
    
#   ifdef _DEBUG
    // write angle of attack iterations to file
    writeAoAToFile();
#   endif
    
    // iterate Cross Sections
    interpolateSections() ;
    
#   ifdef _DEBUG
    // write interpolated sections arrays to file
    writeInterpolatedToFile();
#   endif
    
    // rewriting sections arrays for full ( NOT semi ) span
    rewriteSectionsForFullSpan() ;
    
    // sorting fulls pan sections
    sortFullSpanSections() ;
    
#   ifdef _DEBUG
    // write full span sections arrays to file
    writeFullspanToFile();
#   endif
    
    // create Equations Left-Hand-Side and Right-Hand-Sides (for every Angle of Attack)
    createEquations() ;
    
#   ifdef _DEBUG
    // write equations LHS and RHSs to file
    writeEquationsToFile();
#   endif
    
    // solve system of linear equations
    if ( ! solveEquations() ) ret = false ;
    
#   ifdef _DEBUG
    // write Fourier terms to file
    if ( ret ) writeFourierToFile();
#   endif
    
    // calculating wing aerodynamic characteristics
    if ( ret ) calculateCharacteristics() ;
    
    if ( ret )
        results_uptodate = true ;
    else
        results_uptodate = false ;
    
    return ret ;
}

////////////////////////////////////////////////////////////////////////////////

bool Wing::deleteSectionData( int row )
{
    double* new_sections_data[5] ;
    int i, j ;
    
    // if more than 1 section one cannot delete 1st cross section
    if ( sections > 1 && row == 0 ) return false ;
    
    // If row is smaller than length of Wing::sections_data[ ]
    if ( row < sections )
    {
        // For every value in set (Wing Span Y Coordinate, Leading Edge X Coordinate, etc.)
        for ( i = 0; i < 5; i++) {
            // create new array of length smaller by one than current
            new_sections_data[i] = new double[sections - 1] ;

            // For every current existing sections
            for ( j = 0; j < ( sections - 1 ); j++ )
            {
                switch ( j < row )
                {
                // till deleting row (excluding)
                case true :
                    new_sections_data[i][j] = sections_data[i][j] ;
                    break ;
                    // from deleting row (including)
                case false :
                    new_sections_data[i][j] = sections_data[i][j + 1] ;
                    break ;
                }
            }

            // delete current array
            delete [] sections_data[i] ;
            // point at new array
            sections_data[i] = new_sections_data[i] ;
        }

        // Decrement Wing::sections by one
        sections-- ;

        // recalculate Wing Geometric Data
        calculateGeometry() ;

        // write Wing::sections_data array to file
        writeWingInputToFile() ;

        // setting Wing::results_uptodate as FALSE
        results_uptodate = false ;

        // return TRUE on success
        return true ;
    }
    else // If row is greater than length of Wing::sections_data[ ]
    {
        // return FALSE on failure
        return false ;
    }
}

////////////////////////////////////////////////////////////////////////////////

bool Wing::editSectionData( double span_y, double le_x, double te_x, double slope, double angle, int row )
{
    double data_set[5] = { span_y, le_x, te_x, slope, angle };
    int i;
    
    // wing root section's Y coordinate cannot be modifies
    if ( row == 0 && span_y != 0.0 ) return false;
    
    // check if given Wing Span Y Coordinate is less than 0.0
    // if so return FALSE on failure
    if ( span_y < 0.0 ) return false;
    
    // check if given Trailing Edge X Coordinate is greater than given Leading Edge X Coordinate
    // if is NOT return FALSE on failure
    if ( te_x <= le_x ) return false;
    
    // If new value is different than old value of Wing Span Y Coordinate AND
    // new Wing Span Y Coordinate already exists in Wing::sections_data[ ] array
    // if exists return FALSE on failure
    if ( ( span_y != sections_data[0][row] ) &&  checkIfSectionExists( span_y ) ) return false;
    
    // If row is smaller than length of Wing::wing_cs_data[ ]
    if ( row < sections )
    {
        // For every value in set (Wing Span Y Coordinate, Leading Edge X Coordinate, etc.)
        for ( i = 0; i < 5; i++ )
        {
            // sets new values
            sections_data[i][row] = data_set[i] ;
        }

        // sort new Wing::sections_data[ ]  array
        sortSectionsData() ;

        // recalculate Wing Geometric Data
        calculateGeometry() ;

        // write Wing::sections_data array to file
        writeWingInputToFile() ;

        // setting Wing::results_uptodate as FALSE
        results_uptodate = false ;

        // return TRUE on success
        return true ;
    }
    else // If row is NOT smaller than length of Wing::sections_data[ ]
    {
        // return FALSE on failure
        return false ;
    }
}

////////////////////////////////////////////////////////////////////////////////

double Wing::getAoADeg( int aoa )
{
    if ( aoa < aoa_iterations )
        return angle_of_attack_deg[aoa];
    else
        return 0 ;
}

////////////////////////////////////////////////////////////////////////////////

double Wing::getAoA_Finish( void )
{
    return aoa_finish ;
}

////////////////////////////////////////////////////////////////////////////////

int Wing::getAoA_Iterations( void )
{
    return aoa_iterations ;
}

////////////////////////////////////////////////////////////////////////////////

double Wing::getAoA_Start( void )
{
    return aoa_start ;
}

////////////////////////////////////////////////////////////////////////////////

double Wing::getAoA_Step( void )
{
    return aoa_step ;
}

////////////////////////////////////////////////////////////////////////////////

double Wing::getAspectRatio( void )
{
    return aspect_ratio ;
}

////////////////////////////////////////////////////////////////////////////////

double Wing::getDragCoef( int aoa )
{
    if ( aoa < aoa_iterations )
        return induced_drag_coefficient[aoa] ;
    else
        return 0 ;
}

////////////////////////////////////////////////////////////////////////////////

double Wing::getDragCoefDist( int aoa, int section )
{
    if ( aoa < aoa_iterations && section < sections_fullspan )
        return induced_drag_coefficient_distribution[aoa][section] ;
    else
        return 0;
}

////////////////////////////////////////////////////////////////////////////////

double Wing::getGamma( int aoa, int section )
{
    if ( aoa < aoa_iterations && section < sections_fullspan )
        return gamma[aoa][section] ;
    else
        return 0 ;
}

////////////////////////////////////////////////////////////////////////////////

double Wing::getFluidDensity( void )
{
    return fluid_density ;
}

////////////////////////////////////////////////////////////////////////////////

double Wing::getFluidVelocity( void )
{
    return fluid_velocity ;
}

////////////////////////////////////////////////////////////////////////////////

int Wing::getFourierAccuracy( void )
{
    return fourier_accuracy ;
}

////////////////////////////////////////////////////////////////////////////////

double Wing::getLiftCoef( int aoa )
{
    if ( aoa < aoa_iterations )
        return lift_coefficient[aoa] ;
    else
        return 0 ;
}

////////////////////////////////////////////////////////////////////////////////

double Wing::getLiftCoefDist( int aoa, int section )
{
    if ( aoa < aoa_iterations && section < sections_fullspan )
        return lift_coefficient_distribution[aoa][section] ;
    else
        return 0 ;
}

////////////////////////////////////////////////////////////////////////////////

int Wing::getSections( void )
{
    return sections ;
}

////////////////////////////////////////////////////////////////////////////////

int Wing::getSectionsFullspan( void )
{
    return sections_fullspan ;
}

////////////////////////////////////////////////////////////////////////////////

int Wing::getSectionsIterations( void )
{
    return sections_iterations ;
}

////////////////////////////////////////////////////////////////////////////////

double Wing::getSectionAngle( int i )
{
    if ( i < sections )
        return sections_data[4][i] ;
    else
        return 0 ;
}

////////////////////////////////////////////////////////////////////////////////

double Wing::getSectionLEX( int i )
{
    if ( i < sections )
        return sections_data[1][i] ;
    else
        return 0 ;
}

////////////////////////////////////////////////////////////////////////////////

double Wing::getSectionSlope( int i )
{
    if ( i < sections )
        return sections_data[3][i] ;
    else
        return 0 ;
}

////////////////////////////////////////////////////////////////////////////////

double Wing::getSectionTEX( int i )
{
    if ( i < sections )
        return sections_data[2][i] ;
    else
        return 0 ;
}

////////////////////////////////////////////////////////////////////////////////

double Wing::getSectionY( int i )
{
    if ( i < sections )
        return sections_data[0][i] ;
    else
        return 0 ;
}

////////////////////////////////////////////////////////////////////////////////

double Wing::getTaperRatio( void )
{
    return taper_ratio ;
}

////////////////////////////////////////////////////////////////////////////////

double Wing::getVelocityDist( int aoa, int section )
{
    if ( aoa < aoa_iterations && section < sections_fullspan )
        return induced_velocity_distribution[aoa][section] ;
    else
        return 0 ;
}

////////////////////////////////////////////////////////////////////////////////

double Wing::getWingArea( void )
{
    return wing_area ;
}

////////////////////////////////////////////////////////////////////////////////

double Wing::getWingSpan( void )
{
    return wing_span ;
}

////////////////////////////////////////////////////////////////////////////////

double Wing::getWingSpanCoef( int section )
{
    if ( section < sections_fullspan )
        return ( 2 * wingspan[section] / wing_span ) ;
    else
        return 0 ;
}

////////////////////////////////////////////////////////////////////////////////

bool Wing::isParametersSaved( void )
{
    return parameters_saved ;
}

////////////////////////////////////////////////////////////////////////////////

bool Wing::isResultsUpToDate( void )
{
    return results_uptodate ;
}

////////////////////////////////////////////////////////////////////////////////

bool Wing::readFromFile( const char* file )
{
    int a, i, n ;
    std::fstream fs( file, std::ios_base::in );
    
    if ( fs.is_open() )
    {
        // reading number of input cross sections
        fs >> sections;

        // creating wing geometry input arrays
        for ( i = 0; i < 5; i++ )
        {
            sections_data[i] = new double [sections] ;
        }

        // reading wing geometry input
        for ( i = 0; i < sections; i++ )
        {
            fs >> sections_data[0][i];
            fs >> sections_data[1][i];
            fs >> sections_data[2][i];
            fs >> sections_data[3][i];
            fs >> sections_data[4][i];
        }

        // calculating geometry
        calculateGeometry() ;

        // reading parameters
        fs >> aoa_start;
        fs >> aoa_finish;
        fs >> aoa_iterations;
        fs >> aoa_step;
        fs >> fluid_velocity;
        fs >> fluid_density;
        fs >> sections_iterations;
        fs >> fourier_accuracy;

        parameters_saved = true ;

        sections_fullspan = 2 * sections_iterations - 1 ;

        // checkiing if results exists
        double are_results ;
        fs >> are_results;

        // if so reading results
        if ( are_results > 0 ) {

            results_uptodate = true ;

            // creating angle of attach arrays
            angle_of_attack_deg = new double [aoa_iterations] ;
            angle_of_attack_rad = new double [aoa_iterations] ;

            // reading angle of attack iterations
            for ( a = 0; a < aoa_iterations; a++ )
            {
                fs >> angle_of_attack_deg[a];
                fs >> angle_of_attack_rad[a];
            }

            // creating full span arrays
            wingspan            = new double [sections_fullspan] ;
            chord_length        = new double [sections_fullspan] ;
            lift_curve_slope    = new double [sections_fullspan] ;
            angle_of_0_lift_deg = new double [sections_fullspan] ;
            angle_of_0_lift_rad = new double [sections_fullspan] ;
            phi = new double [sections_fullspan] ;
            mu  = new double [sections_fullspan] ;

            // reading full span section data to file
            for ( i = 0; i < sections_fullspan; i++ )
            {
                fs >> wingspan[i];
                fs >> chord_length[i];
                fs >> lift_curve_slope[i];
                fs >> angle_of_0_lift_deg[i];
                fs >> angle_of_0_lift_rad[i];
                fs >> phi[i];
                fs >> mu[i];
            }

            // creating equations' span array
            equations_span = new double [fourier_accuracy] ;

            // reading equations' span
            for ( i = 0; i < fourier_accuracy; i++ )
            {
                fs >> equations_span[i];
            }

            // creating Left-Hand-Sides of equations arrays
            equations_lhs = new double* [fourier_accuracy] ;
            for ( i = 0; i < fourier_accuracy; i++ )
            {
                equations_lhs[i] = new double [fourier_accuracy] ;
            }

            // reading Left-Hand-Sides of equations
            for ( i = 0; i < fourier_accuracy; i++ )
            {
                for ( n = 0; n < fourier_accuracy; n++ )
                {
                    fs >> equations_lhs[i][n];
                }
            }

            // creating Right-Hand-Sides of equations arrays and Fouerier series terms array
            equations_rhs = new double* [aoa_iterations] ;
            fourier_terms = new double* [aoa_iterations] ;
            for ( a = 0; a < aoa_iterations; a++ )
            {
                equations_rhs[a] = new double [fourier_accuracy] ;
                fourier_terms[a] = new double [fourier_accuracy] ;
            }

            // reading Right-Hand-Sides of equations
            for ( i = 0; i < fourier_accuracy; i++ )
            {
                for ( a = 0; a < aoa_iterations; a++ )
                {
                    fs >> equations_rhs[a][i];
                }
            }

            // reading Fourier series terms
            for ( n = 0; n < fourier_accuracy; n++ )
            {
                for ( a = 0; a < aoa_iterations; a++ )
                {
                    fs >> fourier_terms[a][n];
                }
            }

            // creating spanwise distribution characteristics' arrays
            gamma = new double* [aoa_iterations] ;
            lift_coefficient_distribution = new double* [aoa_iterations] ;
            induced_velocity_distribution = new double* [aoa_iterations] ;
            induced_drag_coefficient_distribution = new double* [aoa_iterations] ;
            for ( a = 0; a < aoa_iterations; a++ )
            {
                gamma[a] = new double [sections_fullspan] ;
                lift_coefficient_distribution[a] = new double [sections_fullspan] ;
                induced_velocity_distribution[a] = new double [sections_fullspan] ;
                induced_drag_coefficient_distribution[a] = new double [sections_fullspan] ;
            }

            // reading Wing::gamma (circulation)
            for ( i = 0; i < sections_fullspan; i++ )
            {
                for ( a = 0; a < aoa_iterations; a++ )
                {
                    fs >> gamma[a][i];
                }
            }

            // reading Wing::lift_coefficient_distribution
            for ( i = 0; i < sections_fullspan; i++ )
            {
                for ( a = 0; a < aoa_iterations; a++ )
                {
                    fs >> lift_coefficient_distribution[a][i];
                }
            }

            // reading Wing::induced_velocity_distribution
            for ( i = 0; i < sections_fullspan; i++ )
            {
                for ( a = 0; a < aoa_iterations; a++ )
                {
                    fs >> induced_velocity_distribution[a][i];
                }
            }

            // reading Wing::induced_drag_coefficient_distribution
            for ( i = 0; i < sections_fullspan; i++ )
            {
                for ( a = 0; a < aoa_iterations; a++ )
                {
                    fs >> induced_drag_coefficient_distribution[a][i];
                }
            }
        }

        // creating arrays of angle of attack dependent characteristics
        lift_coefficient = new double [aoa_iterations] ;
        induced_drag_coefficient = new double [aoa_iterations] ;

        // reading Wing::lift_coefficient
        for ( a = 0; a < aoa_iterations; a++ )
        {
            fs >> lift_coefficient[a];
        }

        // reading Wing::induced_drag_coefficient
        for ( a = 0; a < aoa_iterations; a++ )
        {
            fs >> induced_drag_coefficient[a];
        }

        fs.close();

        strcpy( file_name, file ) ;

        return true ;
    }
    else
    {
        return false ;
    }
}

////////////////////////////////////////////////////////////////////////////////

bool Wing::setParameters( double a_start, double a_finish, int a_iterations, double f_velocity, double f_density, int s_iterations, int f_accuracy )
{
    bool input_valid = true;
    
    // AoA Iteration Start Value cannot be greater than AoA Iteration Finish Value
    if ( a_start == a_finish )
    {
        // if AoA Iteration Start Value eq. to AoA Iteration Finish Value
        // AoA Number of Iterations should be 1
        if ( ! (a_iterations == 1) ) input_valid =  false;
    }
    else if ( a_start < a_finish )
    {

        // if AoA Iteration Start Value less than AoA Iteration Finish Value
        // AoA Number of Iterations should be greater than 1
        if ( ! (a_iterations > 1) ) input_valid =  false;
    }
    else
    {
        input_valid =  false;
    }
    
    // Fluid Velocity should be greater than 0.0
    if ( ! (f_velocity > 0.0) ) input_valid =  false ;
    
    // Fluid Density should be greater than 0.0
    if ( ! (f_density > 0.0) ) input_valid =  false ;
    
    // Number of Cross Sections should be greater or equal to Wing::sections
    if ( ! (s_iterations >= sections) ) input_valid =  false ;
    
    // Fourier Series Accuracy should be smaller than Number of Cross Section less by 1
    if ( ! (s_iterations > f_accuracy) ) input_valid = false ;
    
    // If all parameters are valid delete Wing::equations_lhs and save parameters
    if ( input_valid )
    {
        aoa_start = a_start ;
        aoa_finish = a_finish ;
        aoa_iterations = a_iterations ;
        fluid_velocity = f_velocity ;
        fluid_density = f_density ;
        sections_iterations = s_iterations ;
        fourier_accuracy = 2 * f_accuracy - 1 ;
        parameters_saved = true ;
    }
    
    return input_valid ;
}

////////////////////////////////////////////////////////////////////////////////

bool Wing::writeToDefaultFile( void )
{
    if ( file_name )
    {
        return writeToFile(file_name);
    }
    else
    {
        return false;
    }
}

////////////////////////////////////////////////////////////////////////////////

bool Wing::writeToFile( const char* file )
{
    int a, i, n ;
    std::fstream fs( file, std::ios_base::out );
    
    if ( fs.is_open() )
    {

        // writing number of input cross sections
        fs << sections << std::endl;

        // writing wing geometry inptu
        for ( i = 0; i < sections; i++ )
        {
            fs << sections_data[0][i];
            fs << " ";
            fs << sections_data[1][i];
            fs << " ";
            fs << sections_data[2][i];
            fs << " ";
            fs << sections_data[3][i];
            fs << " ";
            fs << sections_data[4][i];
            fs << std::endl;
        }

        // writing parameters
        fs << aoa_start;
        fs << " ";
        fs << aoa_finish;
        fs << " ";
        fs << aoa_iterations;
        fs << " ";
        fs << aoa_step;
        fs << " ";
        fs << fluid_velocity;
        fs << " ";
        fs << fluid_density;
        fs << " ";
        fs << sections_iterations;
        fs << " ";
        fs << fourier_accuracy;
        fs << std::endl;

        // writing results
        if ( results_uptodate )
        {

            // results exists
            fs << 1 << std::endl;

            // writing angle of attack iterations
            for ( a = 0; a < aoa_iterations; a++ )
            {
                fs << angle_of_attack_deg[a];
                fs << " ";
                fs << angle_of_attack_rad[a];
                fs << std::endl;
            }

            // writing full span section data to file
            for ( i = 0; i < sections_fullspan; i++ )
            {
                fs << wingspan[i];
                fs << " ";
                fs << chord_length[i];
                fs << " ";
                fs << lift_curve_slope[i];
                fs << " ";
                fs << angle_of_0_lift_deg[i];
                fs << " ";
                fs << angle_of_0_lift_rad[i];
                fs << " ";
                fs << phi[i];
                fs << " ";
                fs << mu[i];
                fs << std::endl;
            }

            // writing equations' span
            for ( i = 0; i < fourier_accuracy; i++ )
            {
                fs << equations_span[i];
                fs << std::endl;
            }

            // writing Left-Hand-Sides of equations
            for ( i = 0; i < fourier_accuracy; i++ )
            {
                for ( n = 0; n < fourier_accuracy; n++ )
                {
                    if ( n == fourier_accuracy -1 )
                        fs << equations_lhs[i][n];
                    else
                        fs << equations_lhs[i][n] << " ";
                }
                fs << std::endl;
            }

            // writing Right-Hand-Sides of equations
            for ( i = 0; i < fourier_accuracy; i++ )
            {
                for ( a = 0; a < aoa_iterations; a++ )
                {
                    if ( a == aoa_iterations -1 )
                        fs << equations_rhs[a][i];
                    else
                        fs << equations_rhs[a][i] << " ";
                }
                fs << std::endl;
            }

            // writing Fourier series terms
            for ( n = 0; n < fourier_accuracy; n++ )
            {
                for ( a = 0; a < aoa_iterations; a++ )
                {
                    if ( a == aoa_iterations -1 )
                        fs << fourier_terms[a][n];
                    else
                        fs << fourier_terms[a][n] << " ";
                }
                fs << std::endl;
            }

            // writing Wing::gamma (circulation)
            for ( i = 0; i < sections_fullspan; i++ )
            {
                for ( a = 0; a < aoa_iterations; a++ )
                {
                    if ( a == aoa_iterations -1 )
                        fs << gamma[a][i];
                    else
                        fs << gamma[a][i] << " ";
                }
                fs << std::endl;
            }

            // writing Wing::lift_coefficient_distribution
            for ( i = 0; i < sections_fullspan; i++ )
            {
                for ( a = 0; a < aoa_iterations; a++ )
                {
                    if ( a == aoa_iterations -1 )
                        fs << lift_coefficient_distribution[a][i];
                    else
                        fs << lift_coefficient_distribution[a][i] << " ";
                }
                fs << std::endl;
            }

            // writing Wing::induced_velocity_distribution
            for ( i = 0; i < sections_fullspan; i++ )
            {
                for ( a = 0; a < aoa_iterations; a++ )
                {
                    if ( a == aoa_iterations -1 )
                        fs << induced_velocity_distribution[a][i];
                    else
                        fs << induced_velocity_distribution[a][i] << " ";
                }
                fs << std::endl;
            }

            // writing Wing::induced_drag_coefficient_distribution
            for ( i = 0; i < sections_fullspan; i++ )
            {
                for ( a = 0; a < aoa_iterations; a++ )
                {
                    if ( a == aoa_iterations -1 )
                        fs << induced_drag_coefficient_distribution[a][i];
                    else
                        fs << induced_drag_coefficient_distribution[a][i] << " ";
                }
                fs << std::endl;
            }

            // writing Wing::lift_coefficient
            for ( a = 0; a < aoa_iterations; a++ )
            {
                fs << lift_coefficient[a];
                fs << std::endl;
            }

            // writing Wing::induced_drag_coefficient
            for ( a = 0; a < aoa_iterations; a++ )
            {
                fs << induced_drag_coefficient[a];
                fs << std::endl;
            }

        } else {
            // results doesnt exist
            fs << 0 << std::endl;
        }

        fs.close();

        strcpy( file_name, file );

        return true;
    }
    else
    {
        return false;
    }
}

////////////////////////////////////////////////////////////////////////////////

void Wing::calculateCharacteristics()
{
    int a, i, n ;
    double sum_nAn2, sum_nAnsin, tmp_gamma ;

    sum_nAn2 = 0.0;

    // creating arrays for every Angle of Attack
    gamma = new double* [aoa_iterations] ;
    lift_coefficient_distribution = new double* [aoa_iterations] ;
    induced_velocity_distribution = new double* [aoa_iterations] ;
    induced_drag_coefficient_distribution = new double* [aoa_iterations] ;

    lift_coefficient = new double [aoa_iterations] ;
    induced_drag_coefficient = new double [aoa_iterations] ;

    // creating wing span distributions arrays second dimension
    for ( a = 0; a < aoa_iterations; a++ )
    {

        // creating arrays for every Cross Section
        gamma[a] = new double [sections_fullspan ] ;
        lift_coefficient_distribution[a] = new double [ sections_fullspan ] ;
        induced_velocity_distribution[a] = new double [ sections_fullspan ] ;
        induced_drag_coefficient_distribution[a] = new double [ sections_fullspan ] ;
    }

    // calculating characteristics for every angle of attack
    for ( a = 0; a < aoa_iterations; a++ )
    {
        // start value of temporary coefficients
        lift_coefficient[a] = 0.0 ;

        for ( i = 0; i < sections_fullspan; i++ )
        {
            // start values
            tmp_gamma = 0.0 ;
            sum_nAn2 = 0.0 ;
            sum_nAnsin = 0.0 ;

            // for every Fourier series term
            for ( n = 0; n < fourier_accuracy; n++ )
            {
                // calculating gamma (circulation) for single Cross Section and Angle of Attack
                tmp_gamma = tmp_gamma + sin( ( (double)n + 1 ) * phi[i] ) * fourier_terms[a][n] ;

                // calculating temporary coefficients
                sum_nAnsin += ( (double)n + 1 ) * sin( ( (double)n + 1 ) * phi[i] ) * fourier_terms[a][n] ;
                sum_nAn2 += ( (double)n + 1 ) * pow( fourier_terms[a][n], 2 ) ;
            }

            // finishing calculating gamma (circulation)
            tmp_gamma = tmp_gamma * ( 2 * wing_span * fluid_velocity ) ;
            gamma[a][i] = tmp_gamma ;

            // calculating lift coef. distribution for single Cross Section and Angle of Attack
            lift_coefficient_distribution[a][i] = 2 * gamma[a][i] / ( fluid_velocity * chord_length[i] ) ;

            // calculating induced valocity distribution distribution for single Cross Section and Angle of Attack
            induced_velocity_distribution[a][i] = ( i != 0 && i != sections_fullspan - 1 ) ? ( ( -1 ) / sin( phi[i] ) * sum_nAnsin ) : ( 0 ) ;

            // calculating induced drag coef. distribution for single Cross Section and Angle of Attack
            induced_drag_coefficient_distribution[a][i] = ( -induced_velocity_distribution[a][i] ) * gamma[a][i] / ( fluid_velocity * fluid_velocity * chord_length[i] ) ;

            // calculating lift coef. for single Angle of Attack and whole wing
            if ( i > 0 ) lift_coefficient[a] += ( wingspan[i] - wingspan[i-1] ) * ( lift_coefficient_distribution[a][i] + lift_coefficient_distribution[a][i - 1] ) / 2 ;
            //lift_coefficient[a] = sum_nAnsin
        }

        lift_coefficient[a] /= wing_span ;

        // calcualting induced drag coef. for single Angle of Attack and whole wing
        induced_drag_coefficient[a] = pi * aspect_ratio * sum_nAn2 ;

    }

}

////////////////////////////////////////////////////////////////////////////////

void Wing::calculateGeometry()
{
    int i ;
    // geometric data
    wing_span = .0 ;
    wing_area = .0 ;
    aspect_ratio = .0 ;
    taper_ratio = .0 ;

    // If at least 2 sections coduct calculations
    if ( sections > 1 )
    {
        // Calculating Wing Span (Wing::wing_span)
        wing_span = 2 * ( sections_data[0][sections - 1] - sections_data[0][0] ) ;

        // Calculating Wing Area (Wing::wing_area)
        for ( i = 0; i < ( sections - 1 ); i++ ) {
            wing_area += ( ( sections_data[2][i] - sections_data[1][i] ) + ( sections_data[2][i + 1] - sections_data[1][i + 1] )  ) / 2 * ( sections_data[0][i + 1] - sections_data[0][i] ) ;
        }
        wing_area *= 2 ;

        // Calculating Aspect Ratio (Wing::aspect_ratio)
        aspect_ratio = wing_span * wing_span / wing_area ;

        // Calculating Taper Ratio (Wing::taper_ratio)
        taper_ratio = ( sections_data[2][sections - 1] - sections_data[1][sections - 1] ) / ( sections_data[2][0] - sections_data[1][0] ) ;
    }

}

////////////////////////////////////////////////////////////////////////////////

bool Wing::checkIfSectionExists( double span_y )
{
    int i ;
    bool cs_exists = false ;
    double* haystack_span ;
    haystack_span = sections_data[0];

    for ( i = 0; i < sections; i++ )
    {
        if ( span_y == *haystack_span )
        {
            cs_exists = true ;
            break ;
        }
        else
        {
            haystack_span++ ;
            continue ;
        }
    }

    return cs_exists ;
}

////////////////////////////////////////////////////////////////////////////////

void Wing::createEquations()
{
    int i, a, n, cur_section ;
    double sin_phi, rhs ;
    int step = (int)floor( ( sections_iterations - 2.0 ) / ( ( fourier_accuracy + 1.0 ) / 2.0 ) ) ;

    if ( step < 1 )
        step = 1 ;

    // creating first dimension
    equations_lhs = new double* [fourier_accuracy] ;
    equations_rhs = new double* [aoa_iterations] ;

    equations_span = new double [fourier_accuracy] ;

    // creating second dimension
    for ( i = 0; i < fourier_accuracy; i++ )
    {
        equations_lhs[i] = new double [fourier_accuracy] ;
    }

    // creating second dimension
    for ( a = 0; a < aoa_iterations; a++ )
    {
        equations_rhs[a] = new double [fourier_accuracy] ;
    }

    cur_section = step ;

    // calculating linear equations system's matrices for left wing
    //for ( i = 1; i < ( ( fourier_accuracy + 1) / 2 ) - 1; i++ )
    for ( i = 0; i < ( ( fourier_accuracy + 1) / 2 ) - 1; i++ )
    {
        for ( n = 1; n <= fourier_accuracy; n++ )
        {
            equations_lhs[i][ n - 1 ] = ( sin( phi[cur_section] ) + ( (double)n ) * mu[cur_section] ) * sin( ( (double)n ) * phi[cur_section] ) ;
        }

        for ( a = 0; a < aoa_iterations; a++ )
        {
            sin_phi = sin( phi[cur_section] ) ;
            rhs = sin_phi * mu[cur_section] *  ( angle_of_attack_rad[a] - angle_of_0_lift_rad[cur_section] );
            equations_rhs[a][i] = rhs ;
        }

        equations_span[i] = wingspan[cur_section] ;

        cur_section += step ;
    }

    // calculating linear equations system's matrices for plane of simetricity
    for ( n = 1; n <= fourier_accuracy; n++ )
    {
        equations_lhs[( ( fourier_accuracy + 1) / 2 ) - 1][ n-1 ] = ( sin( phi[sections_iterations - 1] ) + ( (double)n ) * mu[sections_iterations - 1] ) * sin( ( (double)n ) * phi[sections_iterations - 1] ) ;
    }

    for ( a = 0; a < aoa_iterations; a++ )
    {
        sin_phi = sin( phi[sections_iterations-1] ) ;
        rhs = sin_phi * mu[sections_iterations - 1] *  ( angle_of_attack_rad[a] - angle_of_0_lift_rad[sections_iterations - 1] );
        equations_rhs[a][( ( fourier_accuracy + 1) / 2 ) - 1] = rhs ;
    }

    //equations_span[( ( fourier_accuracy + 1) / 2 ) - 1] = wingspan[sections_iterations - 1] ;
    equations_span[( ( fourier_accuracy + 1) / 2 ) - 1] = wingspan[sections_iterations - 1] ;

    cur_section = sections_fullspan - step - 1 ;

    // calculating linear equations system's matrices for right wing
    for ( i = fourier_accuracy - 1; i > ( ( fourier_accuracy + 1) / 2 ) - 1; i-- )
    {
        for ( n = 1; n <= fourier_accuracy; n++ )
        {
            equations_lhs[i][ n-1 ] = ( sin( phi[cur_section] ) + ( (double)n ) * mu[cur_section] ) * sin( ( (double)n ) * phi[cur_section] ) ;
        }

        for ( a = 0; a < aoa_iterations; a++ )
        {
            sin_phi = sin( phi[cur_section] ) ;
            rhs = sin_phi * mu[cur_section] *  ( angle_of_attack_rad[a] - angle_of_0_lift_rad[cur_section] ) ;
            equations_rhs[a][i] = rhs ;
        }

        equations_span[i] = wingspan[cur_section] ;

        cur_section = cur_section - step ;
    }
}

////////////////////////////////////////////////////////////////////////////////

void Wing::interpolateSections()
{
    int i ;

    // creating arrays of interpolating data
    wingspan = new double[sections_iterations] ;
    chord_length = new double[sections_iterations] ;
    lift_curve_slope = new double[sections_iterations] ;
    angle_of_0_lift_deg = new double[sections_iterations] ;
    angle_of_0_lift_rad = new double[sections_iterations] ;

    // calculating winspan_step
    double wingspan_step = ( wing_span / 2 ) / ( sections_iterations - 1 ) ;

    // setting start current section
    int current_section = 0 ;

    // setting start leading edge x coordinate, trailing edge x coordinate values,
    // airfoil lift curve slope and angle of 0 lift 'previous' value
    double i_wingspan = 0.0 ;
    double i_leading_edge = sections_data[1][0] ;
    double i_trailing_edge = sections_data[2][0] ;
    double i_lift_curve_slope = sections_data[3][0] ;
    double i_angle_of_0_lift = sections_data[4][0] ;

    // setting start 'per_step_' values
    double per_step_leading_edge = ( sections_data[1][current_section + 1] - sections_data[1][current_section] ) / ( sections_data[0][current_section + 1] - sections_data[0][current_section] ) * wingspan_step ;
    double per_step_trailing_edge = ( sections_data[2][current_section + 1] - sections_data[2][current_section] ) / ( sections_data[0][current_section + 1] - sections_data[0][current_section] ) * wingspan_step ;
    double per_step_lift_curve_slope = ( sections_data[3][current_section + 1] - sections_data[3][current_section] ) / ( sections_data[0][current_section + 1] - sections_data[0][current_section] ) * wingspan_step ;
    double per_step_angle_of_0_lift = ( sections_data[4][current_section + 1] - sections_data[4][current_section] ) / ( sections_data[0][current_section + 1] - sections_data[0][current_section] ) * wingspan_step ;

    // setting values of the root section
    chord_length[0] = sections_data[2][0] - sections_data[1][0] ;
    lift_curve_slope[0] = sections_data[3][0] ;
    angle_of_0_lift_deg[0] = sections_data[4][0] ;
    angle_of_0_lift_rad[0] = ( 2 * pi * angle_of_0_lift_deg[0] ) / 360 ;

    // interpolating loop
    for ( i = 1; i < sections_iterations - 1; i++ )
    {
        // root span allready known... so first interpolation made for greater by one
        i_wingspan += wingspan_step ;
        wingspan[i] = i_wingspan ;

        // checking if values 'per_step_' are 'up to date'
        if ( wingspan[i] < sections_data[0][current_section + 1] + 0.0001 ) {

            // calculating chord_length
            i_leading_edge += per_step_leading_edge ;
            i_trailing_edge += per_step_trailing_edge ;
            chord_length[i] = i_trailing_edge - i_leading_edge ;

            // calculating lift_curve_slope
            lift_curve_slope[i] = i_lift_curve_slope + per_step_lift_curve_slope ;
            i_lift_curve_slope = lift_curve_slope[i] ;

            // calculating angle_of_0_lift_deg and angle_of_0_lift_rad
            angle_of_0_lift_deg[i] = i_angle_of_0_lift + per_step_angle_of_0_lift ;
            angle_of_0_lift_rad[i] = ( 2 * pi * angle_of_0_lift_deg[i] ) / 360 ;
            i_angle_of_0_lift = angle_of_0_lift_deg[i] ;

        }
        else
        {
            std::cerr << "span= " << wingspan[i] ;

            // updating 'per_step_' values
            // looking for new gradients 'per_step_'
            while ( current_section < sections - 1 )
            {
                current_section++ ;

                // when find recalculate 'per_step_' values
                if ( wingspan[i] + 0.0001 > sections_data[0][current_section] && wingspan[i] < sections_data[0][current_section + 1] + 0.0001 )
                {
                    per_step_leading_edge = ( sections_data[1][current_section + 1] - sections_data[1][current_section] ) / ( sections_data[0][current_section + 1] - sections_data[0][current_section] ) * wingspan_step ;
                    per_step_trailing_edge = ( sections_data[2][current_section + 1] - sections_data[2][current_section] ) / ( sections_data[0][current_section + 1] - sections_data[0][current_section] ) * wingspan_step ;
                    per_step_lift_curve_slope = ( sections_data[3][current_section + 1] - sections_data[3][current_section] ) / ( sections_data[0][current_section + 1] - sections_data[0][current_section] ) * wingspan_step ;
                    per_step_angle_of_0_lift = ( sections_data[4][current_section + 1] - sections_data[4][current_section] ) / ( sections_data[0][current_section + 1] - sections_data[0][current_section] ) * wingspan_step ;
                    break;
                }
                else
                {
                    continue;
                }
            }

            std::cerr << "\tLE_perstep= " << per_step_leading_edge ;
            std::cerr << "\tcur_sec= " << current_section << std::endl ;

            // calculating chord_length
            i_leading_edge = sections_data[1][current_section] + per_step_leading_edge / ( wingspan[i] - sections_data[0][current_section] ) * wingspan_step ;
            i_trailing_edge = sections_data[2][current_section] + per_step_trailing_edge / ( wingspan[i] - sections_data[0][current_section] ) * wingspan_step ;
            chord_length[i] = i_trailing_edge - i_leading_edge ;

            // calculating lift_curve_slope
            lift_curve_slope[i] = sections_data[3][current_section] + per_step_lift_curve_slope / ( wingspan[i] - sections_data[0][current_section] ) * wingspan_step ;

            // calculating angle_of_0_lift_deg and angle_of_0_lift_rad
            angle_of_0_lift_deg[i] = sections_data[4][current_section] + per_step_angle_of_0_lift / ( wingspan[i] - sections_data[0][current_section] ) * wingspan_step ;
            angle_of_0_lift_rad[i] = 2 * pi * angle_of_0_lift_deg[i] / 360 ;

            // setting current leading edge x coordinate, trailing edge x coordinate values,
            // airfoil lift curve slope and angle of 0 lift interpolation 'previous' value
            i_lift_curve_slope = lift_curve_slope[i] ;
            i_angle_of_0_lift = angle_of_0_lift_deg[i] ;
        }
    }

    // setting values of the tip section
    wingspan[sections_iterations - 1] = sections_data[0][sections - 1] ;
    chord_length[sections_iterations - 1] = sections_data[2][sections - 1] - sections_data[1][sections - 1] ;
    lift_curve_slope[sections_iterations - 1] = sections_data[3][sections - 1] ;
    angle_of_0_lift_deg[sections_iterations - 1] = sections_data[4][sections - 1] ;
    angle_of_0_lift_rad[sections_iterations - 1] = ( 2 * pi * angle_of_0_lift_deg[sections_iterations - 1] ) / 360 ;


}

////////////////////////////////////////////////////////////////////////////////

void Wing::iterateAoA()
{
    int i ;

    // calculating angle of attack iteration step
    if ( aoa_iterations > 1 )
        aoa_step = ( aoa_finish - aoa_start ) / ( (double)aoa_iterations - 1 );
    else
        aoa_step = 0.0 ;

    // creating AoA deg and rad arrays
    angle_of_attack_deg = new double[aoa_iterations] ;
    angle_of_attack_rad = new double[aoa_iterations] ;

    // calculating AoAs arrays
    for ( i = 0; i < aoa_iterations; i++ )
    {
        angle_of_attack_deg[i] = aoa_start + aoa_step * (double)i ;
        angle_of_attack_rad[i] = 2 * pi * angle_of_attack_deg[i] / 360 ;
    }
}

////////////////////////////////////////////////////////////////////////////////

void Wing::releaseResults()
{
    int i ;
    // releasing memory
    for ( i = 0; i < old_accuracy; i++ )
    {
        delete [] equations_lhs[i] ;
        // and setting null address
        equations_lhs[i] = 0 ;
    }

    for ( i = 0; i < old_aoa_iterations; i++ )
    {
        delete [] equations_rhs[i] ;
        delete [] fourier_terms[i] ;
        delete [] gamma[i] ;
        delete [] lift_coefficient_distribution[i] ;
        delete [] induced_velocity_distribution[i] ;
        delete [] induced_drag_coefficient_distribution[i] ;
        equations_rhs[i] = 0 ;
        fourier_terms[i] = 0 ;
        gamma[i] = 0 ;
        lift_coefficient_distribution[i] = 0 ;
        induced_velocity_distribution[i] = 0 ;
        induced_drag_coefficient_distribution[i] = 0 ;
    }

    delete [] equations_lhs ;
    delete [] equations_rhs ;
    delete [] equations_span ;
    delete [] fourier_terms ;
    delete [] gamma ;
    delete [] lift_coefficient_distribution ;
    delete [] induced_velocity_distribution ;
    delete [] induced_drag_coefficient_distribution ;
    delete [] lift_coefficient ;
    delete [] induced_drag_coefficient ;
    equations_lhs = 0 ;
    equations_rhs = 0 ;
    equations_span = 0 ;
    fourier_terms = 0 ;
    gamma = 0 ;
    lift_coefficient_distribution = 0 ;
    induced_velocity_distribution = 0 ;
    induced_drag_coefficient_distribution = 0 ;
    lift_coefficient = 0 ;
    induced_drag_coefficient = 0 ;

    // releasing interpolated sections data
    delete [] wingspan  ;
    delete [] chord_length ;
    delete [] lift_curve_slope ;
    delete [] angle_of_0_lift_deg ;
    delete [] angle_of_0_lift_rad ;
    delete [] phi ;
    delete [] mu ;

    // setting null address
    wingspan            = 0;
    chord_length        = 0;
    lift_curve_slope    = 0;
    angle_of_0_lift_deg = 0;
    angle_of_0_lift_rad = 0;
    phi = 0;
    mu  = 0;

    // setting new Wing::old_accuracy and Wing::old_aoa_iterations
    old_accuracy = fourier_accuracy ;
    old_aoa_iterations = aoa_iterations ;
}

////////////////////////////////////////////////////////////////////////////////

void Wing::rewriteSectionsForFullSpan()
{
    double* f_wingspan ;
    double* f_chord_length ;
    double* f_lift_curve_slope ;
    double* f_angle_of_0_lift_deg ;
    double* f_angle_of_0_lift_rad ;
    int f_iterations = 2 * sections_iterations - 1 ;
    int i ;

    // creating full span arrays
    f_wingspan            = new double[f_iterations] ;
    f_chord_length        = new double[f_iterations] ;
    f_lift_curve_slope    = new double[f_iterations] ;
    f_angle_of_0_lift_deg = new double[f_iterations] ;
    f_angle_of_0_lift_rad = new double[f_iterations] ;
    phi = new double[f_iterations] ;
    mu  = new double[f_iterations] ;

    // left wing tip to wing root (excluded)
    for ( i = 0; i < sections_iterations; i++ )
    {
        f_wingspan[i] = wingspan[i] ;
        f_chord_length[i] = chord_length[i] ;
        f_lift_curve_slope[i] = lift_curve_slope[i] ;
        f_angle_of_0_lift_deg[i] = angle_of_0_lift_deg[i] ;
        f_angle_of_0_lift_rad[i] = angle_of_0_lift_rad[i] ;

        // calulating fi & mi
        phi[i] = acos( ( -2 ) * f_wingspan[i] / wing_span ) ;
        mu[i]  = ( 0.25 * f_chord_length[i] * f_lift_curve_slope[i] ) / wing_span ;
    }

    // wing root (included) to right wing tip
    for ( i = 1; i < sections_iterations; i++ )
    {
        f_wingspan[ i + sections_iterations - 1 ] = - wingspan[i] ;
        f_chord_length[ i + sections_iterations - 1 ] = chord_length[i] ;
        f_lift_curve_slope[ i + sections_iterations - 1 ] = lift_curve_slope[i] ;
        f_angle_of_0_lift_deg[ i + sections_iterations - 1 ] = angle_of_0_lift_deg[i] ;
        f_angle_of_0_lift_rad[ i + sections_iterations - 1 ] = angle_of_0_lift_rad[i] ;

        // calulating fi & mi
        phi[ i + sections_iterations - 1 ] = acos( ( -2 ) * f_wingspan[ i + sections_iterations - 1 ] / wing_span ) ;
        mu[ i + sections_iterations - 1 ]  = ( 0.25 * f_chord_length[ i + sections_iterations - 1 ] * f_lift_curve_slope[ i + sections_iterations - 1 ] ) / wing_span ;
    }

    // setting number of sections in full span Wing::sections_fullspan
    sections_fullspan = f_iterations ;

    // deleting semi span arrays
    delete [] wingspan  ;
    delete [] chord_length ;
    delete [] lift_curve_slope ;
    delete [] angle_of_0_lift_deg ;
    delete [] angle_of_0_lift_rad ;

    // pointing sections arrays to full span arrays
    wingspan = f_wingspan ;
    chord_length = f_chord_length ;
    lift_curve_slope = f_lift_curve_slope ;
    angle_of_0_lift_deg = f_angle_of_0_lift_deg ;
    angle_of_0_lift_rad = f_angle_of_0_lift_rad ;

}

////////////////////////////////////////////////////////////////////////////////

bool Wing::solveEquations()
{
    bool success = true ;
    int a, i, n = 0 ;
    double* lhs ;
    double* rhs ;

    // creating Wing::fourier_terms array's first dimension
    fourier_terms = new double* [aoa_iterations] ;
    // and second dimension
    for ( a = 0; a < aoa_iterations; a++ ) {
        fourier_terms[a] = new double [fourier_accuracy] ;
    }

    // creating temporary arrays
    lhs = new double [ fourier_accuracy * fourier_accuracy ] ;
    rhs = new double [ fourier_accuracy ] ;

    // iterating over Angle of Attack
    for ( a = 0; a < aoa_iterations; a++ )
    {
        // copying equations Right-Hand-Side to temporary array
        for ( i = 0; i < fourier_accuracy; i++ )
        {
            rhs[i] = equations_rhs[a][i] ;
            // copying equations Left-Hand-Side to temporary array
            for ( n = 0; n < fourier_accuracy; n++ )
            {
                lhs[fourier_accuracy*n+i] = equations_lhs[n][i] ;
            }
        }

        // solving system of linear equations for single Angle of Attack
        if ( GaussJordan::solve( fourier_accuracy, lhs, rhs, rhs ) == BSC_FAILURE )
        {
            success = false ;
        }

        // copying equation's result
        for ( i = 0; i < fourier_accuracy; i++ )
        {
            fourier_terms[a][i] = rhs[i] ;
        }

    }

    delete [] lhs ;
    delete [] rhs ;
    lhs = 0 ;
    rhs = 0 ;

    return success ;
}

////////////////////////////////////////////////////////////////////////////////

void Wing::sortFullSpanSections()
{
    bool sorted ;
    int i, j ;
    double temp ;

    for ( i = 1; i < sections_fullspan; i++ )
    {
        sorted = true ;

        for ( j = 0; j < sections_fullspan - i; j++ )
        {
            if ( wingspan[j] > wingspan[j+1] )
            {
                // wing span
                temp = wingspan[j] ;
                wingspan[j] = wingspan[j+1] ;
                wingspan[j+1] = temp ;
                // chord
                temp = chord_length[j] ;
                chord_length[j] = chord_length[j+1] ;
                chord_length[j+1] = temp ;
                // lift curve slop
                temp = lift_curve_slope[j] ;
                lift_curve_slope[j] = lift_curve_slope[j+1] ;
                lift_curve_slope[j+1] = temp ;
                // angle of _of_0_lift0 lift
                temp = angle_of_0_lift_deg[j] ;
                angle_of_0_lift_deg[j] = angle_of_0_lift_deg[j+1] ;
                angle_of_0_lift_deg[j+1] = temp ;
                temp = angle_of_0_lift_rad[j] ;
                angle_of_0_lift_rad[j] = angle_of_0_lift_rad[j+1] ;
                angle_of_0_lift_rad[j+1] = temp ;
                // phi
                temp     = phi[j];
                phi[j]   = phi[j+1];
                phi[j+1] = temp;
                // mu
                temp    = mu[j] ;
                mu[j]   = mu[j+1] ;
                mu[j+1] = temp ;
                // keep sorting
                sorted = false ;
            }
        }

        if ( sorted )
        {
            break ;
        }
    }

}

////////////////////////////////////////////////////////////////////////////////

void Wing::sortSectionsData()
{
    bool sorted ;
    int i, j, k ;
    double temp ;

    for ( i = 1; i < sections; i++ )
    {
        sorted = true ;

        for ( j = 0; j < sections - i; j++ )
        {
            if ( sections_data[0][j] > sections_data[0][j+1] )
            {
                for ( k=0; k<5; k++ )
                {
                    temp = sections_data[k][j] ;
                    sections_data[k][j] = sections_data[k][j+1] ;
                    sections_data[k][j+1] = temp ;
                }
                sorted = false ;
            }
        }
        if ( sorted ) {
            break ;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

bool Wing::writeAoAToFile()
{
    int a ;
    std::fstream fs( "../tmp/tmp.aoa", std::ios_base::out );

    if ( fs.is_open() )
    {
        for ( a = 0; a < aoa_iterations; a++ )
        {
            fs << angle_of_attack_deg[a];
            fs << "\t";
            fs << angle_of_attack_rad[a];
            fs << std::endl;
        }

        fs.close();
        return true;

    }
    else
    {
        return false ;
    }
}

////////////////////////////////////////////////////////////////////////////////

bool Wing::writeEquationsToFile()
{
    int a, i, n ;
    bool ret = true ;
    std::fstream fs( "../tmp/tmp.lhs", std::ios_base::out );

    if ( fs.is_open() )
    {
        for ( i = 0; i < fourier_accuracy; i++ )
        {
            fs << equations_span[i];
            fs << "\t";
            fs << "\t";

            for ( n = 0; n < fourier_accuracy; n++ )
            {
                fs << equations_lhs[i][n];
                fs << "\t";
            }

            fs << std::endl;
        }
    }
    else
    {
        ret = false ;
    }

    if ( fs.is_open() && ret )
    {
        for ( i = 0; i < fourier_accuracy; i++ )
        {
            fs << equations_span[i];
            fs << "\t";
            fs << "\t";

            for ( a = 0; a < aoa_iterations; a++ )
            {
                fs << equations_rhs[a][i];
                fs << "\t";
            }

            fs << std::endl;
        }

    }
    else
    {
        ret = false ;
    }

    if ( fs.is_open() )
    {
        fs.close();
    }

    return ret ;
}

////////////////////////////////////////////////////////////////////////////////

bool Wing::writeFourierToFile()
{
    int a, n ;
    std::fstream fs( "../tmp/tmp.terms", std::ios_base::out );

    if ( fs.is_open() )
    {
        for ( n = 0; n < fourier_accuracy; n++ )
        {
            for ( a = 0; a < aoa_iterations; a++ )
            {
                fs << fourier_terms[a][n];
                fs << "\t";
            }

            fs << std::endl;
        }

        fs.close();

        return true ;
    }
    else
    {
        return false ;
    }
}

////////////////////////////////////////////////////////////////////////////////

bool Wing::writeFullspanToFile()
{
    int i ;
    std::fstream fs( "../tmp/tmp.fullspan", std::ios_base::out );

    if ( fs.is_open() )
    {
        for ( i = 0; i < sections_fullspan; i++ )
        {
            fs << wingspan[i];
            fs << "\t";
            fs << chord_length[i];
            fs << "\t";
            fs << lift_curve_slope[i];
            fs << "\t";
            fs << angle_of_0_lift_deg[i];
            fs << "\t";
            fs << angle_of_0_lift_rad[i];
            fs << "\t";
            fs << phi[i];
            fs << "\t";
            fs << mu[i];
            fs << std::endl;
        }

        fs.close();

        return true ;
    }
    else
    {
        return false ;
    }
}

////////////////////////////////////////////////////////////////////////////////

bool Wing::writeInterpolatedToFile()
{
    int i ;
    std::fstream fs( "../tmp/tmp.interpolated", std::ios_base::out );

    if ( fs.is_open() )
    {
        for ( i = 0; i < sections_iterations; i++ )
        {
            fs << wingspan[i];
            fs << "\t";
            fs << chord_length[i];
            fs << "\t";
            fs << lift_curve_slope[i];
            fs << "\t";
            fs << angle_of_0_lift_deg[i];
            fs << "\t";
            fs << angle_of_0_lift_rad[i];
            fs << std::endl;
        }

        fs.close();

        return true ;
    }
    else
    {
        return false ;
    }
}

////////////////////////////////////////////////////////////////////////////////

bool Wing::writeParametersToFile()
{
    std::fstream fs( "../tmp/tmp.parameters", std::ios_base::out );

    if ( fs.is_open() )
    {
        fs << "#Angle of Attack Iteration Start Value [deg]" << std::endl;
        fs << "aoa_start =  " << aoa_start << std::endl;
        fs << "\n#Angle of Attack Iteration Finish Value [deg]" << std::endl;
        fs << "aoa_finish = " << aoa_finish << std::endl;
        fs << "\n#Angle of Attack Number of Iterations [-]" << std::endl;
        fs << "aoa_iterations = " << aoa_iterations << std::endl;
        fs << "\n#Angle of Attack Step [deg]" << std::endl;
        fs << "aoa_step = " << aoa_step << std::endl;
        fs << "\n#Fluid Velocity [m/s]" << std::endl;
        fs << "fluid_velocity = " << fluid_velocity << std::endl;
        fs << "\n#Fluid Density [kg/cu m]" << std::endl;
        fs << "fluid_density = " << fluid_density << std::endl;
        fs << "\n#Number of Cross Sections [-]" << std::endl;
        fs << "sections_iterations = " << sections_iterations << std::endl;
        fs << "\n#Fourier Series Accuracy [-]" << std::endl;
        fs << "fourier_accuracy = " << ( ( fourier_accuracy + 1 ) / 2 ) << std::endl;

        fs.close();

        return true ;
    }
    else
    {
        return false ;
    }
}

////////////////////////////////////////////////////////////////////////////////

bool Wing::writeWingInputToFile()
{
    int i ;
    std::fstream fs( "../tmp/tmp.winginput", std::ios_base::out );

    if ( fs.is_open() )
    {
        for ( i = 0; i < sections; i++ )
        {
            fs << sections_data[0][i];
            fs << "\t";
            fs << sections_data[1][i];
            fs << "\t";
            fs << sections_data[2][i];
            fs << "\t";
            fs << sections_data[3][i];
            fs << "\t";
            fs << sections_data[4][i];
            fs << std::endl;
        }

        fs.close();

        return true ;
    }
    else
    {
        return false ;
    }
}
