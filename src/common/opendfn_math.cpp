/******************************************************************************
 * \file      opendfn_math.cpp
 * \brief     Basic maths defined in the project.
 *
 * \details   Implementation of basic maths defined in the project.
 *
 * \author    Xiaofeng Li
 * \email     xfli@whrsm.ac.cn
 * \date      Created:       June 24, 2020
 * \date      Last modified: 2026-07-20 01:04:48
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

#include <math.h>
#include <stdlib.h>
#include <string>
#include <cstring>
#include "opendfn_math.h"

namespace ns_common {
/**
 * \brief Generate a normally distributed random number.
 *
 * \detail Implements the polar (Marsaglia) form of the Box-Muller transform.
 * Two independent standard normal variates are produced per iteration; one is
 * returned immediately and the other cached in static storage for the next
 * call. Static counter \c call toggles between the two branches.
 *
 * \note Uses the C library rand() and internal static state, so it is not
 * reentrant/thread-safe.
 *
 * \param mu    Mean of the distribution.
 * \param sigma Standard deviation of the distribution.
 *
 * \return A pseudo-random value drawn from N(mu, sigma^2).
 */
/* generate gauss distribution */
Real random_normal(Real mu, Real sigma) {
    Real        U1, U2, W, mult;
    static Real X1, X2;
    static int  call = 0;

    if (call == 1) {
        call = !call;
        return (mu + sigma * (Real)X2);
    }

    do {
        U1 = -1 + ((Real)rand() / RAND_MAX) * 2;
        U2 = -1 + ((Real)rand() / RAND_MAX) * 2;
        W  = pow(U1, 2) + pow(U2, 2);
    } while (W >= 1 || W == 0);

    mult = sqrt((-2 * log(W)) / W);
    X1   = U1 * mult;
    X2   = U2 * mult;

    call = !call;

    return (mu + sigma * (Real)X1);
}

/**
 * \brief Reduce a symmetric 3x3 matrix to tridiagonal form via a Householder
 *        transformation.
 *
 * \detail Applies a single (unitary) Householder reflection so that
 * A = Q . T . Q^T, where T is the symmetric tridiagonal matrix whose diagonal
 * is stored in \p d and whose off-diagonal is stored in \p e. Only the diagonal
 * and upper-triangular part of \p A are read; \p A is not modified.
 *
 * \param A Input symmetric matrix (read-only, diagonal + upper triangle only).
 * \param Q Output orthogonal transformation matrix (initialized to identity
 *          then filled with the inverse Householder transform).
 * \param d Output diagonal entries of the tridiagonal matrix (length 3).
 * \param e Output off-diagonal entries of the tridiagonal matrix (length 2).
 *
 * \return void. Results are written into \p Q, \p d and \p e.
 */
void HouseholderTransform(Real A[3][3], Real Q[3][3], Real d[3], Real e[2])
// Reduces a symmetric 3x3 matrix to tridiagonal form by applying
// (unitary) Householder transformations:
//            [ d[0]  e[0]       ]
//    A = Q . [ e[0]  d[1]  e[1] ] . Q^T
//            [       e[1]  d[2] ]
// The function accesses only the diagonal and upper triangular parts of
// A. The access is read-only.

{
    const Int n = 3;
    Real      u[n], q[n];
    Real      omega, f;
    Real      K, h, g;

    // Initialize Q to the identitity matrix
    for (Int i = 0; i < n; i++) {
        Q[i][i] = 1.0;
        for (Int j = 0; j < i; j++)
            Q[i][j] = Q[j][i] = 0.0;
    }

    // Bring first row and column to the desired form
    h = opendfn_square(A[0][1]) + opendfn_square(A[0][2]);
    if (A[0][1] > 0)
        g = -sqrt(h);
    else
        g = sqrt(h);
    e[0] = g;
    f    = g * A[0][1];
    u[1] = A[0][1] - g;
    u[2] = A[0][2];

    omega = h - f;
    if (omega > 0.0) {
        omega = 1.0 / omega;
        K     = 0.0;
        for (Int i = 1; i < n; i++) {
            f    = A[1][i] * u[1] + A[i][2] * u[2];
            q[i] = omega * f;  // p
            K += u[i] * f;     // u* A u
        }
        K *= 0.5 * opendfn_square(omega);

        for (Int i = 1; i < n; i++)
            q[i] = q[i] - K * u[i];

        d[0] = A[0][0];
        d[1] = A[1][1] - 2.0 * q[1] * u[1];
        d[2] = A[2][2] - 2.0 * q[2] * u[2];

        // Store inverse Householder transformation in Q
        for (Int j = 1; j < n; j++) {
            f = omega * u[j];
            for (Int i = 1; i < n; i++)
                Q[i][j] = Q[i][j] - f * u[i];
        }

        // Calculate updated A[1][2] and store it in e[1]
        e[1] = A[1][2] - q[1] * u[2] - u[1] * q[2];
    } else {
        for (Int i = 0; i < n; i++)
            d[i] = A[i][i];
        e[1] = A[1][2];
    }
}

/**
 * \brief Solve the eigensystem of a symmetric 3x3 matrix using the QL algorithm.
 *
 * \detail Computes eigenvalues and normalized eigenvectors of \p A using the QL
 * algorithm with implicit shifts, preceded by a Householder reduction to
 * tridiagonal form. Only the diagonal and upper-triangular part of \p A are
 * read (read-only access).
 *
 * \param A Input symmetric matrix (read-only).
 * \param Q Output storage buffer for the normalized eigenvectors (columns).
 * \param w Output storage buffer for the eigenvalues.
 *
 * \return 0 on success, -1 on error (no convergence).
 *
 * \dependence HouseholderTransform()
 */
Int solveEigenusingQL(Real A[3][3], Real Q[3][3], Real w[3])
// Calculates the eigenvalues and normalized eigenvectors of a symmetric 3x3
// matrix A using the QL algorithm with implicit shifts, preceded by a
// Householder reduction to tridiagonal form.
// The function accesses only the diagonal and upper triangular parts of A.
// The access is read-only.

// Parameters:
//   A: The symmetric input matrix
//   Q: Storage buffer for eigenvectors
//   w: Storage buffer for eigenvalues

// Return value:
//   0: Success
//  -1: Error (no convergence)

// Dependencies:
//   HouseholderTransform()

{
    const Int n = 3;
    Real      e[3];                    // The third element is used only as temporary workspace
    Real      g, r, p, f, b, s, c, t;  // Intermediate storage
    Int       nIter;
    Int       m;

    // Transform A to real tridiagonal form by the Householder method
    HouseholderTransform(A, Q, w, e);

    // Calculate eigensystem of the remaining real symmetric tridiagonal matrix
    // with the QL method
    //
    // Loop over all off-diagonal elements
    for (Int l = 0; l < n - 1; l++) {
        nIter = 0;
        while (1) {
            // Check for convergence and exit iteration loop if off-diagonal
            // element e(l) is zero
            for (m = l; m <= n - 2; m++) {
                g = fabs(w[m]) + fabs(w[m + 1]);
                if (fabs(e[m]) + g == g)
                    break;
            }
            if (m == l)
                break;

            if (nIter++ >= 30)
                return -1;

            // Calculate g = d_m - k
            g = (w[l + 1] - w[l]) / (e[l] + e[l]);
            r = sqrt(opendfn_square(g) + 1.0);
            if (g > 0)
                g = w[m] - w[l] + e[l] / (g + r);
            else
                g = w[m] - w[l] + e[l] / (g - r);

            s = c = 1.0;
            p     = 0.0;
            for (Int i = m - 1; i >= l; i--) {
                f = s * e[i];
                b = c * e[i];
                if (fabs(f) > fabs(g)) {
                    c        = g / f;
                    r        = sqrt(opendfn_square(c) + 1.0);
                    e[i + 1] = f * r;
                    c *= (s = 1.0 / r);
                } else {
                    s        = f / g;
                    r        = sqrt(opendfn_square(s) + 1.0);
                    e[i + 1] = g * r;
                    s *= (c = 1.0 / r);
                }

                g        = w[i + 1] - p;
                r        = (w[i] - g) * s + 2.0 * c * b;
                p        = s * r;
                w[i + 1] = g + p;
                g        = c * r - b;

                // Form eigenvectors
                for (Int k = 0; k < n; k++) {
                    t           = Q[k][i + 1];
                    Q[k][i + 1] = s * Q[k][i] + c * t;
                    Q[k][i]     = c * Q[k][i] - s * t;
                }
            }
            w[l] -= p;
            e[l] = g;
            e[m] = 0.0;
        }
    }
    return 0;
}

/**
 * \brief Solve the secular equation arising in Cuppen's divide & conquer step.
 *
 * \detail Finds the three roots w_j of f(w_j) = 1 + Sum[ z_i / (d_i - w_j) ] = 0.
 * Assumes d_0 <= d_1 <= d_2 and that all z_i share the same sign. Roots are
 * first estimated analytically then refined with a combined
 * bisection/Newton-Raphson iteration for accuracy. The differences
 * P_ij = d_i - w_j needed for the eigenvector computation are stored in \p R.
 *
 * \param d  Diagonal coefficients of the secular equation (length 3, ascending).
 * \param z  Numerators of the secular equation (length 3, same sign).
 * \param w  Output eigenvalues / roots (length 3).
 * \param R  Output matrix of differences R[i][j] = d_i - w_j.
 * \param i0 Index selecting the bracketing interval for the first root.
 * \param i1 Index selecting the bracketing interval for the second root.
 * \param i2 Index selecting the bracketing interval for the third root.
 *
 * \return void. Results are written into \p w and \p R.
 */
void solveSecularquation(Real d[3], Real z[3], Real w[3], Real R[3][3], Int i0, Int i1, Int i2)
// Finds the three roots w_j of the secular equation
//   f(w_j) = 1 + Sum[ z_i / (d_i - w_j) ]  ==  0.
// It is assumed that d_0 <= d_1 <= d_2, and that all z_i have the same sign.
// The arrays P_i will contain the information required for the calculation
// of the eigenvectors:
//   P_ij = d_i - w_j.
// These differences can be obtained with better accuracy from intermediate
// results.
{
    Real a[4];                     // Bounds of the intervals bracketing the roots
    Real delta;                    // Shift of the d_i which ensures better accuracy
    Real dd[3];                    // Shifted coefficients dd_i = d_i - delta
    Real xl, xh;                   // Interval which straddles the current root. f(xl) < 0, f(xh) > 0
    Real x;                        // Current estimates for the root
    Real x0[3];                    // Analytically calculated roots, used as starting values
    Real F, dF;                    // Function value f(x) and derivative f'(x)
    Real dx, dxold;                // Current and last stepsizes
    Real error;                    // Numerical error estimate, used for termination condition
    Real t[3];                     // Temporary storage used for evaluating f
    Real alpha, beta, gamma;       // Coefficients of polynomial f(x) * Product [ d_i - x ]
    Real p, sqrt_p, q, c, s, phi;  // Intermediate results of analytical calculation

    // Determine intervals which must contain the roots
    if (z[0] > 0) {
        a[0] = d[i0];
        a[1] = d[i1];
        a[2] = d[i2];
        a[3] = fabs(d[0] + 3.0 * z[0]) + fabs(d[1] + 3.0 * z[1]) + fabs(d[2] + 3.0 * z[2]);
    } else {
        a[0] = -fabs(d[0] + 3.0 * z[0]) - fabs(d[1] + 3.0 * z[1]) - fabs(d[2] + 3.0 * z[2]);
        a[1] = d[i0];
        a[2] = d[i1];
        a[3] = d[i2];
    }

    // Calculate roots of f(x) = 0 analytically (analogous to ZHEEVC3)
    t[0]  = d[1] * d[2];
    t[1]  = d[0] * d[2];
    t[2]  = d[0] * d[1];
    gamma = t[0] * d[0] + (z[0] * t[0] + z[1] * t[1] + z[2] * t[2]);  // Coefficients
    beta  = (z[0] * (d[1] + d[2]) + z[1] * (d[0] + d[2]) + z[2] * (d[0] + d[1])) + (t[0] + t[1] + t[2]);
    alpha = (z[0] + z[1] + z[2]) + (d[0] + d[1] + d[2]);

    p      = opendfn_square(alpha) - 3.0 * beta;  // Transformation that removes the x^2 term
    q      = alpha * (p - (3.0 / 2.0) * beta) + (27.0 / 2.0) * gamma;
    sqrt_p = sqrt(fabs(p));

    phi = 27.0 * (0.25 * opendfn_square(beta) * (p - beta) - gamma * (q - 27.0 / 4.0 * gamma));
    phi = (1.0 / 3.0) * atan2(sqrt(fabs(phi)), q);
    c   = sqrt_p * cos(phi);
    s   = (1.0 / OpenDFN_Sqrt3) * sqrt_p * fabs(sin(phi));

    x0[0] = x0[1] = x0[2] = (1.0 / 3.0) * (alpha - c);
    if (c > s)  // Make sure the roots are in ascending order
    {
        x0[0] -= s;
        x0[1] += s;
        x0[2] += c;
    } else if (c < -s) {
        x0[0] += c;
        x0[1] -= s;
        x0[2] += s;
    } else {
        x0[0] -= s;
        x0[1] += c;
        x0[2] += s;
    }

    // Refine roots with a combined Bisection/Newton-Raphson method
    for (Int i = 0; i < 3; i++) {
        xl = a[i];      // Lower bound of bracketing interval
        xh = a[i + 1];  // Upper bound of bracketing interval
        dx = dxold = 0.5 * (xh - xl);

        // Make sure that xl != xh
        if (dx == 0.0) {
            w[i] = xl;
            for (Int j = 0; j < 3; j++)
                R[j][i] = d[j] - xl;
            continue;
        }

        // Shift the root close to zero to achieve better accuracy
        if (x0[i] >= xh) {
            delta = xh;
            x     = -dx;
            for (Int j = 0; j < 3; j++) {
                dd[j]   = d[j] - delta;
                R[j][i] = dd[j] - x;
            }
        } else if (x0[i] <= xl) {
            delta = xl;
            x     = dx;
            for (Int j = 0; j < 3; j++) {
                dd[j]   = d[j] - delta;
                R[j][i] = dd[j] - x;
            }
        } else {
            delta = x0[i];
            x     = 0.0;
            for (Int j = 0; j < 3; j++)
                R[j][i] = dd[j] = d[j] - delta;
        }
        xl -= delta;
        xh -= delta;

        // Make sure that f(xl) < 0 and f(xh) > 0
        if (z[0] < 0.0) {
            Real t = xh;
            xh     = xl;
            xl     = t;
        }

        // Main iteration loop
        for (Int nIter = 0; nIter < 500; nIter++) {
            // Evaluate f and f', and calculate an error estimate
            F     = 1.0;
            dF    = 0.0;
            error = 1.0;
            for (Int j = 0; j < 3; j++) {
                t[0] = 1.0 / R[j][i];
                t[1] = z[j] * t[0];
                t[2] = t[1] * t[0];
                F += t[1];
                error += fabs(t[1]);
                dF += t[2];
            }

            // Check for convergence
            if (fabs(F) <= OpenDFN_pos_epsilon * (8.0 * error + fabs(x * dF)))
                break;

            // Adjust interval boundaries
            if (F < 0.0)
                xl = x;
            else
                xh = x;

            // Check, whether Newton-Raphson would converge fast enough. If so,
            // give it a try. If not, or if it would run out of bounds, use bisection
            if (fabs(2.0 * F) < fabs(dxold * dF)) {
                dxold = dx;
                dx    = F / dF;
                x     = x - dx;
                if ((x - xh) * (x - xl) >= 0.0) {
                    dx = 0.5 * (xh - xl);
                    x  = xl + dx;
                }
            } else {
                dx = 0.5 * (xh - xl);
                x  = xl + dx;
            }

            // Prepare next iteration
            for (Int j = 0; j < 3; j++)
                R[j][i] = dd[j] - x;
        }

        // Un-shift result
        w[i] = x + delta;
    }
}

/**
 * \brief Solve the eigensystem of a real symmetric 2x2 matrix.
 *
 * \detail Computes the eigenvalues and the rotation (cs, sn) that diagonalizes
 * the matrix [ A B ; B C ] such that rt1 >= rt2. Note this ordering convention
 * differs from LAPACK's DLAEV2, which orders by magnitude (|rt1| >= |rt2|).
 * Special cases (zero trace, small off-diagonal) are handled to avoid division
 * by zero.
 *
 * \param A   Top-left matrix entry.
 * \param B   Off-diagonal matrix entry.
 * \param C   Bottom-right matrix entry.
 * \param rt1 Output pointer to the larger eigenvalue.
 * \param rt2 Output pointer to the smaller eigenvalue.
 * \param cs  Output pointer to the cosine of the rotation angle.
 * \param sn  Output pointer to the sine of the rotation angle.
 *
 * \return void. Results are written through the output pointers.
 */
void solveEigen2(Real A, Real B, Real C, Real* rt1, Real* rt2, Real* cs, Real* sn)
// Calculates the eigensystem of a real symmetric 2x2 matrix
//    [ A  B ]
//    [ B  C ]
// in the form
//    [ A  B ]  =  [ cs  -sn ] [ rt1   0  ] [  cs  sn ]
//    [ B  C ]     [ sn   cs ] [  0   rt2 ] [ -sn  cs ]
// where rt1 >= rt2. Note that this convention is different from the one used
// in the LAPACK routine DLAEV2, where |rt1| >= |rt2|.

{
    Real sm = A + C;
    Real df = A - C;
    Real rt = sqrt(opendfn_square(df) + 4.0 * B * B);
    Real t;

    if (sm > 0.0) {
        *rt1 = 0.5 * (sm + rt);
        t    = 1.0 / (*rt1);
        *rt2 = (A * t) * C - (B * t) * B;
    } else if (sm < 0.0) {
        *rt2 = 0.5 * (sm - rt);
        t    = 1.0 / (*rt2);
        *rt1 = (A * t) * C - (B * t) * B;
    } else  // This case needs to be treated separately to avoid div by 0
    {
        *rt1 = 0.5 * rt;
        *rt2 = -0.5 * rt;
    }

    // Calculate eigenvectors
    if (df > 0.0) {
        *cs = df + rt;
    } else {
        *cs = df - rt;
    }
    if (fabs(*cs) > 2.0 * fabs(B)) {
        t   = -2.0 * B / *cs;
        *sn = 1.0 / sqrt(1.0 + opendfn_square(t));
        *cs = t * (*sn);
    } else if (fabs(B) == 0.0) {
        *cs = 1.0;
        *sn = 0.0;
    } else {
        t   = -0.5 * (*cs) / B;
        *cs = 1.0 / sqrt(1.0 + opendfn_square(t));
        *sn = t * (*cs);
    }

    if (df > 0.0) {
        t   = *cs;
        *cs = -(*sn);
        *sn = t;
    }
}

/**
 * \brief Solve the eigensystem of a symmetric 3x3 matrix using Cuppen's
 *        divide & conquer algorithm.
 *
 * \detail Reduces \p A to tridiagonal form (Householder), splits it into a 1x1
 * and a 2x2 block ("divide"), then reconstructs the full eigensystem by solving
 * the secular equation and undoing the transformations ("conquer"). Includes
 * fallbacks for factorizing/degenerate matrices to avoid duplicate eigenvalues
 * and division by zero. Only the diagonal and upper-triangular part of \p A are
 * read (read-only access).
 *
 * \param A Input symmetric matrix (read-only).
 * \param Q Output storage buffer for the normalized eigenvectors (columns).
 * \param w Output storage buffer for the eigenvalues.
 *
 * \return 0 on success, -1 on error.
 *
 * \dependence solveEigen2(), solveSecularquation(), HouseholderTransform()
 */
Int solveEigenusingCuppen(Real A[3][3], Real Q[3][3], Real w[3])

// Calculates the eigenvalues and normalized eigenvectors of a symmetric 3x3
// matrix A using Cuppen's Divide & Conquer algorithm.
// The function accesses only the diagonal and upper triangular parts of A.
// The access is read-only.

// Parameters:
//   A: The symmetric input matrix
//   Q: Storage buffer for eigenvectors
//   w: Storage buffer for eigenvalues

// Return value:
//   0: Success
//  -1: Error

// Dependencies:
//   solveEigen2(), solveSecularquation(), HouseholderTransform()

{
    const Int n = 3;
    Real      R[3][3];  // Householder transformation matrix
    Real      P[3][3];  // Unitary transformation matrix which diagonalizes D + w w^T
    Real      e[2];     // Off-diagonal elements after Householder transformation
    Real      d[3];     // Eigenvalues of split matrix in the "divide" step)
    Real      c, s;     // Eigenvector of 2x2 block in the "divide" step
    Real      z[3];     // Numerators of secular equation / Updating vector
    Real      t;        // Miscellaenous temporary stuff

    // Initialize Q
    memset(Q, 0.0, 9 * sizeof(Real));

    // Transform A to real tridiagonal form by the Householder method
    HouseholderTransform(A, R, w, e);

    // "Divide"
    // --------

    // Detect matrices that factorize to avoid multiple eigenvalues in the Divide/Conquer algorithm
    for (Int i = 0; i < n - 1; i++) {
        t = fabs(w[i]) + fabs(w[i + 1]);
        if (fabs(e[i]) <= 8.0 * OpenDFN_pos_epsilon * t) {
            if (i == 0) {
                solveEigen2(w[1], e[1], w[2], &d[1], &d[2], &c, &s);
                w[1]    = d[1];
                w[2]    = d[2];
                Q[0][0] = 1.0;
                for (Int j = 1; j < n; j++) {
                    Q[j][1] = s * R[j][2] + c * R[j][1];
                    Q[j][2] = c * R[j][2] - s * R[j][1];
                }
            } else {
                solveEigen2(w[0], e[0], w[1], &d[0], &d[1], &c, &s);
                w[0]    = d[0];
                w[1]    = d[1];
                Q[0][0] = c;
                Q[0][1] = -s;
                Q[1][0] = R[1][1] * s;
                Q[1][1] = R[1][1] * c;
                Q[1][2] = R[1][2];
                Q[2][0] = R[2][1] * s;
                Q[2][1] = R[2][1] * c;
                Q[2][2] = R[2][2];
            }

            return 0;
        }
    }

    // Calculate eigenvalues and eigenvectors of 2x2 block
    solveEigen2(w[1] - e[0], e[1], w[2], &d[1], &d[2], &c, &s);
    d[0] = w[0] - e[0];

    // "Conquer"
    // ---------

    // Determine coefficients of secular equation
    z[0] = e[0];
    z[1] = e[0] * opendfn_square(c);
    z[2] = e[0] * opendfn_square(s);

    // Call solveSecularquation with d sorted in ascending order. We make
    // use of the fact that solveEigen2 guarantees d[1] >= d[2].
    if (d[0] < d[2])
        solveSecularquation(d, z, w, P, 0, 2, 1);
    else if (d[0] < d[1])
        solveSecularquation(d, z, w, P, 2, 0, 1);
    else
        solveSecularquation(d, z, w, P, 2, 1, 0);

    // Calculate eigenvectors of matrix D + beta * z * z^t and store them in the
    // columns of P
    z[0] = sqrt(fabs(e[0]));
    z[1] = c * z[0];
    z[2] = -s * z[0];

    // Detect duplicate elements in d to avoid division by zero
    t = 8.0 * OpenDFN_pos_epsilon * (fabs(d[0]) + fabs(d[1]) + fabs(d[2]));
    if (fabs(d[1] - d[0]) <= t) {
        for (Int j = 0; j < n; j++) {
            if (P[0][j] * P[1][j] <= 0.0) {
                P[0][j] = z[1];
                P[1][j] = -z[0];
                P[2][j] = 0.0;
            } else
                for (Int i = 0; i < n; i++)
                    P[i][j] = z[i] / P[i][j];
        }
    } else if (fabs(d[2] - d[0]) <= t) {
        for (Int j = 0; j < n; j++) {
            if (P[0][j] * P[2][j] <= 0.0) {
                P[0][j] = z[2];
                P[1][j] = 0.0;
                P[2][j] = -z[0];
            } else
                for (Int i = 0; i < n; i++)
                    P[i][j] = z[i] / P[i][j];
        }
    } else {
        for (Int j = 0; j < n; j++)
            for (Int i = 0; i < n; i++) {
                if (P[i][j] == 0.0) {
                    P[i][j]           = 1.0;
                    P[(i + 1) % n][j] = 0.0;
                    P[(i + 2) % n][j] = 0.0;
                    break;
                } else
                    P[i][j] = z[i] / P[i][j];
            }
    }

    // Normalize eigenvectors of D + beta * z * z^t
    for (Int j = 0; j < n; j++) {
        t = opendfn_square(P[0][j]) + opendfn_square(P[1][j]) + opendfn_square(P[2][j]);
        t = 1.0 / sqrt(t);
        for (Int i = 0; i < n; i++)
            P[i][j] *= t;
    }

    // Undo diagonalization of 2x2 block
    for (Int j = 0; j < n; j++) {
        t       = P[1][j];
        P[1][j] = c * t - s * P[2][j];
        P[2][j] = s * t + c * P[2][j];
    }

    // Undo Householder transformation
    for (Int j = 0; j < n; j++)
        for (Int k = 0; k < n; k++) {
            t = P[k][j];
            for (Int i = 0; i < n; i++)
                Q[i][j] += t * R[i][k];
        }

    return 0;
}

/**
 * \brief Solve the eigensystem of a symmetric 3x3 matrix using the Jacobi
 *        algorithm.
 *
 * \detail Iteratively zeroes off-diagonal entries with Jacobi rotations until
 * convergence. The upper-triangular part of \p A is destroyed during the
 * computation, the diagonal elements are read but not destroyed, and the lower
 * triangular elements are never referenced.
 *
 * \param A Input symmetric matrix; upper triangle is overwritten during the run.
 * \param Q Output storage buffer for the normalized eigenvectors (columns).
 * \param w Output storage buffer for the eigenvalues.
 *
 * \return 0 on success, -1 on error (no convergence within iteration limit).
 */
Int solveEigenusingJacobi(Real A[3][3], Real Q[3][3], Real w[3])
// Calculates the eigenvalues and normalized eigenvectors of a symmetric 3x3
// matrix A using the Jacobi algorithm.
// The upper triangular part of A is destroyed during the calculation,
// the diagonal elements are read but not destroyed, and the lower
// triangular elements are not referenced at all.
// Parameters:
//   A: The symmetric input matrix
//   Q: Storage buffer for eigenvectors
//   w: Storage buffer for eigenvalues

// Return value:
//   0: Success
//  -1: Error (no convergence)
{
    const Int n = 3;
    Real      sd, so;          // Sums of diagonal resp. off-diagonal elements
    Real      s, c, t;         // sin(phi), cos(phi), tan(phi) and temporary storage
    Real      g, h, z, theta;  // More temporary storage
    Real      thresh;

    // Initialize Q to the identitity matrix
    for (Int i = 0; i < n; i++) {
        Q[i][i] = 1.0;
        for (Int j = 0; j < i; j++)
            Q[i][j] = Q[j][i] = 0.0;
    }

    // Initialize w to diag(A)
    for (Int i = 0; i < n; i++)
        w[i] = A[i][i];

    // Calculate opendfn_square(tr(A))
    sd = 0.0;
    for (Int i = 0; i < n; i++)
        sd += fabs(w[i]);
    sd = opendfn_square(sd);

    // Main iteration loop
    for (Int nIter = 0; nIter < 50; nIter++) {
        // Test for convergence
        so = 0.0;
        for (Int p = 0; p < n; p++)
            for (Int q = p + 1; q < n; q++)
                so += fabs(A[p][q]);
        if (so == 0.0)
            return 0;

        if (nIter < 4)
            thresh = 0.2 * so / opendfn_square(n);
        else
            thresh = 0.0;

        // Do sweep
        for (Int p = 0; p < n; p++) {
            for (Int q = p + 1; q < n; q++) {
                g = 100.0 * fabs(A[p][q]);
                if (nIter > 4 && fabs(w[p]) + g == fabs(w[p]) && fabs(w[q]) + g == fabs(w[q])) {
                    A[p][q] = 0.0;
                } else if (fabs(A[p][q]) > thresh) {
                    // Calculate Jacobi transformation
                    h = w[q] - w[p];
                    if (fabs(h) + g == fabs(h)) {
                        t = A[p][q] / h;
                    } else {
                        theta = 0.5 * h / A[p][q];
                        if (theta < 0.0)
                            t = -1.0 / (sqrt(1.0 + opendfn_square(theta)) - theta);
                        else
                            t = 1.0 / (sqrt(1.0 + opendfn_square(theta)) + theta);
                    }
                    c = 1.0 / sqrt(1.0 + opendfn_square(t));
                    s = t * c;
                    z = t * A[p][q];

                    // Apply Jacobi transformation
                    A[p][q] = 0.0;
                    w[p] -= z;
                    w[q] += z;
                    for (Int r = 0; r < p; r++) {
                        t       = A[r][p];
                        A[r][p] = c * t - s * A[r][q];
                        A[r][q] = s * t + c * A[r][q];
                    }
                    for (Int r = p + 1; r < q; r++) {
                        t       = A[p][r];
                        A[p][r] = c * t - s * A[r][q];
                        A[r][q] = s * t + c * A[r][q];
                    }
                    for (Int r = q + 1; r < n; r++) {
                        t       = A[p][r];
                        A[p][r] = c * t - s * A[q][r];
                        A[q][r] = s * t + c * A[q][r];
                    }

                    // Update eigenvectors
                    for (Int r = 0; r < n; r++) {
                        t       = Q[r][p];
                        Q[r][p] = c * t - s * Q[r][q];
                        Q[r][q] = s * t + c * Q[r][q];
                    }
                }
            }
        }
    }
    return -1;
}
}  // namespace ns_common
