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
#ifndef GAUSSJORDAN_H
#define GAUSSJORDAN_H

////////////////////////////////////////////////////////////////////////////////

#include <cmath>

#include <defs.h>

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Gauss-Jordan numerical method of solving systems of linear equations.
 *
 * @see Press W., et al.: Numerical Recipes: The Art of Scientific Computing, 2007, p.41
 * @see Baron B., Piatek L.: Metody numeryczne w C++ Builder, 2004, p.34. [in Polish]
 * @see https://en.wikipedia.org/wiki/Gaussian_elimination
 */
class GaussJordan
{
public:

    /**
     * @brief Solves system of linear equations using Gauss-Jordan method.
     * @param mtr left hand side matrix
     * @param rhs right hand size vector
     * @param x result vector
     * @param eps minimum value treated as not-zero
     * @return FDM_SUCCESS on success and FDM_FAILURE on failure
     */
    static int solve( const int size, const double *mtr, const double *rhs,
                      double *x, double eps = 1.0e-14 );

    /** @brief Swaps matrix rows. */
    static void swapRowsMatr( const int size, double *mtr, int row1, int row2 );

    /** @brief Swaps vector rows. */
    static void swapRowsVect( const int size, double *vec, int row1, int row2 );
};

////////////////////////////////////////////////////////////////////////////////

#endif // GAUSSJORDAN_H
