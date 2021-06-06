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

#include <GaussJordan.h>

////////////////////////////////////////////////////////////////////////////////

int GaussJordan::solve( const int size, const double *mtr, const double *rhs,
                        double *x, double eps )
{
    int result = BSC_SUCCESS;

    double *mtr_temp = new double [ size * size ] ;
    double *rhs_temp = new double [ size ];

    for ( int i = 0; i < size * size; i++ )
    {
        mtr_temp[ i ]  = mtr[ i ];

        if ( i < size ) rhs_temp[ i ] = rhs[ i ];
    }

    for ( int r = 0; r < size; r++ )
    {
        // run along diagonal, swapping rows to move zeros (outside the diagonal) downwards
        if ( fabs( mtr_temp[ r * size + r ] ) < fabs( eps ) )
        {
            if ( r < size - 1 )
            {
                swapRowsMatr( size, mtr_temp, r, r+1 );
                swapRowsVect( size, rhs_temp, r, r+1 );
            }
            else
            {
                result = BSC_FAILURE;
            }
        }

        // value on diagonal A(r,r)
        double a_rr = mtr_temp[ r * size + r ];
        double a_rr_inv = 1.0 / a_rr;

        // deviding current row by value on diagonal
        for ( int c = 0; c < size; c++ )
        {
            mtr_temp[ r * size + c ] *= a_rr_inv;
        }

        rhs_temp[ r ] *= a_rr_inv;

        // substracting current row from others rows
        // for every row current row is multiplied by A(i,r)
        // where r stands for row that is substracted from other rows
        // and i stands for row that is substracting from
        for ( int i = 0; i < size; i++ )
        {
            if ( i != r )
            {
                double a_ir = mtr_temp[ i * size + r ];

                for ( int c = 0; c < size; c++ )
                {
                    mtr_temp[ i * size + c ] -= a_ir * mtr_temp[ r * size + c ];
                }

                rhs_temp[ i ] -= a_ir * rhs_temp[ r ];
            }
        }
    }

    // rewritting results
    for ( int i = 0; i < size; i++ )
    {
        x[ i ] = rhs_temp[ i ];
    }

    delete [] mtr_temp;
    delete [] rhs_temp;

    return result;
}

////////////////////////////////////////////////////////////////////////////////

void GaussJordan::swapRowsMatr( const int size, double *mtr, int row1, int row2 )
{
    if ( ( row1 < size ) && ( row2 < size ) )
    {
        double *temp = new double[ size ];

        for ( int c = 0; c < size; c++ )
        {
            temp[ c ] = mtr[ row1 * size + c ];
            mtr[ row1 * size + c ] = mtr[ row2 * size + c ];
            mtr[ row2 * size + c ] = temp[ c ];
        }

        delete [] temp;
    }
}

////////////////////////////////////////////////////////////////////////////////

void GaussJordan::swapRowsVect( const int size, double *vec, int row1, int row2 )
{
    if ( row1 < size && row2 < size )
    {
        double temp = vec[ row1 ];
        vec[ row1 ] = vec[ row2 ];
        vec[ row2 ] = temp;
    }
}
