/******************************************************************************
 * \file      opendfn_math.h
 * \brief     Basic maths defined in the project.
 *
 * \details   Declarations for basic maths defined in the project.
 *
 * \author    Xiaofeng Li
 * \email     xfli@whrsm.ac.cn
 * \date      Created:       June 24, 2020
 * \date      Last modified: 2026-07-20 01:03:08
 * \version   OpenDFN Version 1.0.0 (managed by CMake macro PRG_VERSION,
 *            generated into common/opendfn_config.h from opendfn_config.h.in)
 *
 * \see OpenDFN Project Website: https://xiaofengli-uoft.github.io/Mainpage/
 * \see Geomechanics Group Website: https://geogroup.utoronto.ca/
 *
 * The OpenDFN Project is maintained by the OpenDFN Foundation.
 *
 * Copyright (C) 2017-2026 Xiaofeng Li. OpenDFN Contributors.
 *
 *
 * OpenDFN is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * OpenDFN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with OpenDFN. If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

#ifndef OPENDFN_COMMOM_MATH
#define OPENDFN_COMMOM_MATH

#include <math.h>
#include "opendfn_macros.h"
#include "opendfn_error.h"

// Define the constant macro variables in OpenDFN
/* macro postive epsilon */
#define OpenDFN_pos_epsilon 1.0e-10
/* macro negative epsilon */
#define OpenDFN_neg_epsilon -1.0e-10
/* macro big value */
#define OpenDFN_max_value 1.0e+15
/* macro pi */
#define OpenDFN_PI 3.1415926535897932384626
/* macro exp */
#define d4d3 1.333333333333333
#define d2d3 0.6666666666666667
#define d1d3 0.333333333333333
/* macro exp */
#define OpenDFN_e 2.71828182845904523536
/* determine mesh quality */
#define MMG2D_ALPHAD 3.464101615137755 /* 6.0 / sqrt(3.0)  */

// Numerical value of the constant \f$ \frac{3}{2} \f$
#define OpenDFN_sqrt32 1.22474487139158894
// Numerical value of the constant \f$ 2\pi \f$
#define OpenDFN_2PI 6.28318530717958648
// Numerical value of the constant used to convert degree to radians \f$ \frac{\pi}{180} \f$
#define OpenDFN_Deg2Rad 0.0174532925199432958
// Numerical value of the constant \f$ \frac{1}{\sqrt{2}} \f$
#define OpenDFN_InvSqrt2 0.707106781186547524
// Numerical value of the constant \f$ \frac{1}{\sqrt{3}} \f$
#define OpenDFN_InvSqrt3 0.577350269189625764
// Numerical value of the constant used to convert radians to degree \f$ \frac{180}{\pi} \f$
#define OpenDFN_RadToDeg 57.2957795130823209
// Numerical value of the constant \f$ \sqrt{2} \f$
#define OpenDFN_Sqrt2 1.41421356237309505
// Numerical value of the constant \f$ \frac{2}{3} \f$
#define OpenDFN_Sqrt23 0.816496580927726034
// Numerical value of the constant \f$ \sqrt{3} \f$
#define OpenDFN_Sqrt3 1.73205080756887729
// Numerical value of the constant \f$ \frac{3}{2} \f$
#define OpenDFN_Sqrt32 1.22474487139158894
// Numerical value of the constant \f$ \sqrt{3} - 1\f$
#define OpenDFN_Sqrt3m1 0.732050807568877294
// Numerical value of the constant \f$ \sqrt{3} - 2\f$
#define OpenDFN_Sqrt3m2 -0.267949192431122706
// Numerical value of the constant \f$ \sqrt{3} - 3\f$
#define OpenDFN_Sqrt3m3 -1.26794919243112271
// Numerical value of the constant \f$ \sqrt{3} + 1\f$
#define OpenDFN_Sqrt3p1 2.73205080756887729
// Numerical value of the constant \f$ \sqrt{3} + 2\f$
#define OpenDFN_Sqrt3p2 3.73205080756887729
// Numerical value of the constant \f$ \sqrt{3} + 3\f$
#define OpenDFN_Sqrt3p3 4.73205080756887729

// Define geometry macros
/* macro double abs */
#define OpenDFN_double_abs(x) (((x) < 0.0) ? -(x) : (x))
/* macro square */
#define opendfn_square(a) ((a) * (a))
/* macro maximuum function */
#define OpenDFN_max(x, y) (((x) < (y)) ? (y) : (x))
/* macro maximuum3 function */
#define OpenDFN_max3(a, b, c) (((OpenDFN_max(a, b)) > c) ? (OpenDFN_max(a, b)) : (c))
/* macro maximuum4 function */
#define OpenDFN_max4(a, b, c, d) (((OpenDFN_max(a, b)) > (OpenDFN_max(c, d))) ? (OpenDFN_max(a, b)) : (OpenDFN_max(c, d)))
/* macro mimnuum function */
#define OpenDFN_min(x, y) (((x) > (y)) ? (y) : (x))
/* macro minimuum3 function */
#define OpenDFN_min3(a, b, c) (((OpenDFN_min(a, b)) < c) ? (OpenDFN_min(a, b)) : (c))
/* macro sign function */
#define OpenDFN_sign(x) ((x > 0) ? 1 : ((x < 0) ? -1 : 0))
/* macro bracket function */
#define OpenDFN_bracket(x) ((OpenDFN_double_abs(x) + x) * 0.5)
// Returns the MacAuley operator defined by: \f[ \left\langle x\right\rangle
// =\frac{1}{2}\left[x+\left|x\right|\right]\f]
#define OpenDFN_MacAuley(x) ((x) <= 0 ? 0 : (x))
// Returns the surface of a triangle function of the 3 edges lengths from the Heron formula: \f[ \frac{1}{4}
// \sqrt{-(a-b-c) (a+b-c) (a-b+c) (a+b+c)}\f]
#define OpenDFN_SurfTriangle(a, b, c)                                                                                           \
    ((sqrt(((a) + (b) + (c)) * ((a) - (b) + (c)) * ((a) + (b) - (c)) * (-(a) + (b) + (c)))) / 4)
// Returns the surface of a triangle function of the coordinates of the 3 corners from the Heron formula
#define OpenDFN_SurfTrianglePts(xa, ya, xb, yb, xc, yc)                                                                         \
    (OpenDFN_double_abs((xb) * (ya) - (xc) * (ya) - (xa) * (yb) + (xc) * (yb) + (xa) * (yc) - (xb) * (yc)) / 2)
// Returns the interpolated value between 2 points from the following equation: \f[
// x=\lambda x_{1}+(1-\lambda)x_{2} \f]
#define OpenDFN_LinearInterp(x1, x2, dx) ((dx) * (x2) + (1 - (dx)) * (x1))
// Returns \ref true if the given parameter a is within the range [b,c], false if not
#define OpenDFN_BetweenValues(a, b, c) ((a) < (b) ? false : ((a) > (c) ? false : true))
// Returns \ref true if the given parameter a is within the range ]b,c[, false if not
#define OpenDFN_BetweenValuesExclude(a, b, c) ((a) <= (b) ? false : ((a) >= (c) ? false : true))
// Returns the Kronecker: (1) if i=j, (0) in all other cases
#define OpenDFN_Kronecker(i, j) ((i) == (j) ? 1 : 0)

/* macro inverse 2x2 matrix */
#define Matrix2Inverse(m, minv, det)                                                                                             \
    {                                                                                                                            \
        det        = m[0][0] * m[1][1] - m[1][0] * m[0][1];                                                                      \
        minv[0][0] = m[1][1] / det;                                                                                              \
        minv[1][0] = -m[1][0] / det;                                                                                             \
        minv[0][1] = -m[0][1] / det;                                                                                             \
        minv[1][1] = m[0][0] / det;                                                                                              \
    }
/* macro 2x1 vector dot */
#define Vector2DotProduct(s, x1, y1, x2, y2)                                                                                     \
    { (s) = ((x1) * (x2)) + ((y1) * (y2)); }
/* macro 2x1 vector normalized */
#define Vector2Normalize(s, x1, y1)                                                                                              \
    {                                                                                                                            \
        (s) = sqrt((x1) * (x1) + (y1) * (y1));                                                                                   \
        if ((s) > OpenDFN_pos_epsilon)                                                                                          \
            (x1) = (x1) / (s);                                                                                                   \
        if ((s) > OpenDFN_pos_epsilon)                                                                                          \
            (y1) = (y1) / (s);                                                                                                   \
    }
/* macro 2x1 vectore cross product */
#define Vector2Cro(s, x1, y1, x2, y2)                                                                                            \
    { s = (x1 * y2) - (y1 * x2); }
/* macro two points distance */
#define VectorPointDistance(l, x1, y1, x2, y2)                                                                                   \
    { l = std::sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)); }

/* macro vector length */
#define VectorLength(x1, y1) (sqrt((x1) * (x1) + (y1) * (y1)))

/* next vertex */
#define NextVertex(i, nNodes) (((i + 1) > (nNodes - 1)) ? (0) : (i + 1))

/* previous vertex */
#define LastVertex(i, nNodes) ((((int)i - 1) < (0)) ? (nNodes - 1) : (i - 1))

// check it is line or not, if it is a line, the node count is set as 3
// otherwise the cout itself
#define OpenDFN_LineNode(x) ((x < 3) ? (3) : (x))

/* macro triangle area */
#define V2Darea(s, l1, l2, l3)                                                                                                   \
    {                                                                                                                            \
        s = 0.5 * (l1 + l2 + l3);                                                                                                \
        s = sqrt(s * (s - l1) * (s - l2) * (s - l3));                                                                            \
    }

namespace ns_common {
#if defined(__cplusplus)
extern "C" {
#endif
/**
 * \brief Generate a normally distributed random number.
 *
 * \detail Uses the polar (Marsaglia) form of the Box-Muller transform to draw
 * samples from a Gaussian distribution. The routine keeps static state between
 * calls so that the two variates produced by each Box-Muller iteration are
 * returned on consecutive invocations.
 *
 * \note Relies on the C library rand() generator and on internal static state,
 * so it is not reentrant/thread-safe.
 *
 * \param mu    Mean of the distribution.
 * \param sigma Standard deviation of the distribution.
 *
 * \return A pseudo-random value drawn from N(mu, sigma^2).
 */
Real random_normal(Real mu, Real sigma);

/**
 *  Calculates the eigensystem of a real symmetric 2x2 matrix
 *     [ A  B ]
 *     [ B  C ]
 *  in the form
 *     [ A  B ]  =  [ cs  -sn ] [ rt1   0  ] [  cs  sn ]
 *     [ B  C ]     [ sn   cs ] [  0   rt2 ] [ -sn  cs ]
 *  where rt1 >= rt2. Note that this convention is different from the one used
 *  in the LAPACK routine DLAEV2, where |rt1| >= |rt2|..
 */
void solveEigen2(Real A, Real B, Real C, Real* rt1, Real* rt2, Real* cs, Real* sn);

/**
 * \brief Solve eigen values of a symmetric tensor by using QL
 *
 * \detail  Calculates the eigenvalues and normalized eigenvectors of a
 * symmetric 3x3 matrix A using the QL algorithm with implicit shifts,
 *  preceded by a Householder reduction to tridiagonal form.
 *
 * \note The function accesses only the diagonal and upper triangular parts of A.
 *
 * \param A: input symmetric matrix
 * \param Q: return matrix for eigenvectors
 * \param w: return buffer for eigenvalues
 *
 * \return
 *  0: Success \n
 * -1: Error (no convergence)
 *
 * \dependence
 * HouseholderTransform()
 *
 */

Int solveEigenusingQL(Real A[3][3], Real Q[3][3], Real w[3]);

/**
 * \brief Solve eigen values of a symmetric tensor by using Jacobi
 *
 * \detail  Calculates the eigenvalues and normalized eigenvectors of a
 * symmetric 3x3 matrix A using the Jacobi algorithm,The upper triangular part
 * of A is destroyed during the calculation,
 * the diagonal elements are read but not destroyed, and the lower
 * triangular elements are not referenced at all.
 *
 * \note The function accesses only the diagonal and upper triangular parts of A.
 *
 * \param A: input symmetric matrix
 * \param Q: return matrix for eigenvectors
 * \param w: return buffer for eigenvalues
 *
 * \return
 *  0: Success \n
 * -1: Error (no convergence)
 *
 * \dependence
 * HouseholderTransform()
 *
 */

Int solveEigenusingJacobi(Real A[3][3], Real Q[3][3], Real w[3]);

/**
 * \brief Solve eigen values of a symmetric tensor by using Jacobi
 *
 * \detail  Calculates the eigenvalues and normalized eigenvectors of a
 * symmetric 3x3 matrix A using the Cuppen's Divide & Conquer algorithm,The upper triangular part
 * of A is destroyed during the calculation,
 * the diagonal elements are read but not destroyed, and the lower
 * triangular elements are not referenced at all.
 *
 * \note The function accesses only the diagonal and upper triangular parts of A.
 *
 * \param A: input symmetric matrix
 * \param Q: return matrix for eigenvectors
 * \param w: return buffer for eigenvalues
 *
 * \return
 *  0: Success \n
 * -1: Error (no convergence)
 *
 * \dependence
 * dsyev2(), solveSecularquation(), HouseholderTransform()
 *
 */
Int solveEigenusingCuppen(Real A[3][3], Real Q[3][3], Real w[3]);

#ifdef __cplusplus
}
#endif
}  // namespace ns_common
#endif  // !OPENDFN_COMMOM_MATH
