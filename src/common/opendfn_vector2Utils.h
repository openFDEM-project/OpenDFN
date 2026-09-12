/******************************************************************************
 * \file      opendfn_vector2Utils.h
 * \brief     Headers of the main symmetric tensor class used in OpenDFN. The
 *            subroutines and functions are in the <i>opendfn_matrix.cpp</i>
 *            file.
 *
 * \details   Declarations for headers of the main symmetric tensor class used
 *            in OpenDFN. The subroutines and functions are in the
 *            <i>opendfn_matrix.cpp</i> file.
 *
 * \author    Xiaofeng Li
 * \email     xfli@whrsm.ac.cn
 * \date      Created:       June 24, 2020
 * \date      Last modified: 2026-07-20 01:08:37
 * \version   OpenDFN Version 4.40 (managed by CMake macro PRG_VERSION,
 *            generated into common/opendfn_config.h from opendfn_config.h.in)
 *
 * \see OpenDFN Project Website: https://xiaofengli-uoft.github.io/Mainpage/
 * \see Geomechanics Group Website: https://geogroup.utoronto.ca/
 *
 * The OpenDFN Project is maintained by the OpenDFN Foundation.
 *
 * Copyright (C) 2017-2026 Xiaofeng Li. OpenDFN Contributors.
 *
 * OpenDFN is free for educational, research and non-profit purposes.
 * Any commerical or military use should be authorised by the developer.
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

#ifndef SRC_COMMON_OPENDFN_VECTOR2UTILS_H_
#define SRC_COMMON_OPENDFN_VECTOR2UTILS_H_
#include <math.h>
#include <stdlib.h>
#include "common/opendfn_math.h"
#include "common/opendfn_vector2.h"

namespace ns_common {
/**
 * \brief Computes the Euclidean norm (magnitude) of a 2D vector.
 * \param n Input vector.
 * \return sqrt(n.x^2 + n.y^2).
 */
/* compute vector magnitude */
inline Real VectorNorm(const Vector2& n) {
    return sqrt(n.x * n.x + n.y * n.y);
}

/**
 * \brief Computes the squared Euclidean norm of a 2D vector (avoids the sqrt).
 * \param n Input vector.
 * \return n.x^2 + n.y^2.
 */
/* compute vector magnitude */
inline Real VectorSquareNorm(const Vector2& n) {
    return (n.x * n.x + n.y * n.y);
}

/**
 * \brief Returns the component of a vector selected by index.
 * \param n Input vector.
 * \param indices Component selector: 0 for x, 1 for y; any other value yields 0.
 * \return The selected component value, or 0 when the index is out of range.
 */
/* compute vector magnitude */
inline Real VectorgetValue(const Vector2& n, const Int indices) {
    Real mag = 0.;
    switch (indices) {
        case 0: mag = n.x; break;
        case 1: mag = n.y; break;
        default: break;
    }
    return mag;
}

/**
 * \brief Computes a magnitude that only accounts for strictly positive components.
 *        Each component is squared and summed only when greater than zero.
 * \param n Input vector.
 * \return Square root of the sum of squares of the positive components.
 */
/* compute vector magnitude */
inline Real VectorPostiveMag2(const Vector2& n) {
    Real mag = 0.0;
    if (n.x > 0.0)
        mag += n.x * n.x;
    if (n.y > 0.0)
        mag += n.y * n.y;
    return sqrt(mag);
}

/**
 * \brief Computes the slope n.y/n.x of a vector.
 *        A small positive epsilon is added to x to avoid division by zero.
 * \param n Input vector.
 * \return Slope value n.y / (n.x + epsilon).
 */
inline Real VectorSlope(const Vector2& n) {
    return (n.y / (n.x + OpenDFN_pos_epsilon));
}
/**
 * \brief Returns the component-wise sum of two vectors.
 * \param n2 First addend vector.
 * \param n1 Second addend vector.
 * \return A new vector equal to n2 + n1.
 */
/* compute vector add opreator */
inline Vector2 VectorAdd(const Vector2& n2, const Vector2& n1) {
    Vector2 rv;

    rv.x = n2.x + n1.x;
    rv.y = n2.y + n1.y;

    return rv;
}

/**
 * \brief Computes the Euclidean distance between two points/vectors.
 * \param n2 First vector.
 * \param n1 Second vector.
 * \return Norm of (n2 - n1).
 */
inline Real VectorDistance(const Vector2& n2, const Vector2& n1) {
    Vector2 rv;

    rv.x = n2.x - n1.x;
    rv.y = n2.y - n1.y;

    return VectorNorm(rv);
}

/**
 * \brief Computes the squared Euclidean distance between two points/vectors.
 * \param n2 First vector.
 * \param n1 Second vector.
 * \return Squared norm of (n2 - n1).
 */
inline Real VectorSquareDistance(const Vector2& n2, const Vector2& n1) {
    Vector2 rv;

    rv.x = n2.x - n1.x;
    rv.y = n2.y - n1.y;

    return VectorSquareNorm(rv);
}
/**
 * \brief Returns the component-wise average of two vectors.
 * \param n2 First vector.
 * \param n1 Second vector.
 * \return A new vector equal to 0.5 * (n2 + n1).
 */
/* compute vector average opreator */
inline Vector2 VectorAverage(const Vector2& n2, const Vector2& n1) {
    Vector2 rv;

    rv.x = 0.5 * (n2.x + n1.x);
    rv.y = 0.5 * (n2.y + n1.y);

    return rv;
}
/**
 * \brief Returns the component-wise difference of two vectors (n2 - n1).
 * \param n2 Minuend vector.
 * \param n1 Subtrahend vector.
 * \return A new vector equal to n2 - n1.
 */
/* compute vector sub opreator, n2 - n1 */
inline Vector2 VectorSub(const Vector2& n2, const Vector2& n1) {
    Vector2 rv;
    rv.x = n2.x - n1.x;
    rv.y = n2.y - n1.y;
    return rv;
}

/**
 * \brief Computes the inward normal of the edge from n1 to n2 using the right-hand
 *        rule, i.e. z x (n2 - n1). The result is not normalized to unit length.
 * \param n1 Start point of the edge.
 * \param n2 End point of the edge.
 * \return Inward normal vector (-(n2.y - n1.y), n2.x - n1.x).
 */
/* compute inward normal of two vectors, right-hand rule, z x (n2 - n1)  */
inline Vector2 VectorNormalInward(const Vector2& n1, const Vector2& n2) {
    Vector2 rv;
    // elx = n2.x - n1.x;
    // ely = n2.y - n1.y;
    // inward normal = (-ely, elx);
    rv.x = -n2.y + n1.y;
    rv.y = n2.x - n1.x;
    return rv;
}

/**
 * \brief Computes the outward normal of the edge from n1 to n2 using the right-hand
 *        rule, i.e. -z x (n2 - n1). The result is not normalized to unit length.
 * \param n1 Start point of the edge.
 * \param n2 End point of the edge.
 * \return Outward normal vector (n2.y - n1.y, -(n2.x - n1.x)).
 */
/* compute outward normal of two vectors, right-hand rule, -z x (n2 - n1) */
inline Vector2 VectorNormalOutward(const Vector2& n1, const Vector2& n2) {
    Vector2 rv;
    // elx = n2.x - n1.x;
    // ely = n2.y - n1.y;
    // inward normal = (ely, -elx);
    rv.x = n2.y - n1.y;
    rv.y = -n2.x + n1.x;
    return rv;
}
/**
 * \brief Returns the component-wise (Hadamard) product of two vectors.
 * \param n2 First vector.
 * \param n1 Second vector.
 * \return A new vector with components (n2.x * n1.x, n2.y * n1.y).
 */
/* compute vector compoment multiply opreator */
inline Vector2 VectorMul(const Vector2& n2, const Vector2& n1) {
    Vector2 rv;

    rv.x = n2.x * n1.x;
    rv.y = n2.y * n1.y;

    return rv;
}
/**
 * \brief Returns the component-wise division of two vectors.
 * \param n2 Numerator vector.
 * \param n1 Denominator vector.
 * \return A new vector with components (n2.x / n1.x, n2.y / n1.y).
 */
/* compute vector compoment divide opreator */
inline Vector2 VectorDiv(const Vector2& n2, const Vector2& n1) {
    Vector2 rv;

    rv.x = n2.x / n1.x;
    rv.y = n2.y / n1.y;

    return rv;
}
/**
 * \brief Returns a vector scaled by a scalar factor.
 * \param s Scalar multiplier.
 * \param n1 Input vector.
 * \return A new vector equal to s * n1.
 */
/* compute vector compoment multiply a scaler opreator */
inline Vector2 VectorMulscalar(Real s, const Vector2& n1) {
    Vector2 rv;

    rv.x = s * n1.x;
    rv.y = s * n1.y;

    return rv;
}
/**
 * \brief Computes the dot product of two vectors.
 * \param n2 First vector.
 * \param n1 Second vector.
 * \return n2.x * n1.x + n2.y * n1.y.
 */
/* compute vector dot opreator */
inline Real VectorDot(const Vector2& n2, const Vector2& n1) {
    return n2.x * n1.x + n2.y * n1.y;
}
/**
 * \brief Computes the scalar 2D cross product (z component) of two vectors, v1 x v2.
 * \param v1 First vector.
 * \param v2 Second vector.
 * \return v1.x * v2.y - v1.y * v2.x.
 */
/* compute vector cross opreator */
/* v1 x v2 */
inline Real VectorCross(const Vector2& v1, const Vector2& v2) {
    return v1.x * v2.y - v1.y * v2.x;
}
/**
 * \brief Returns a unit-length copy of the input vector.
 *        When the input length is below a tiny threshold, the vector (1, 0)
 *        is returned to avoid division by zero.
 * \param v1 Input vector to normalize.
 * \return A new normalized vector, or (1, 0) for a near-zero input.
 */
/* compute vector normalize opreator */
inline Vector2 VectorNormalize(const Vector2& v1) {
    Vector2 rv;

    Real length;
    Real factor;
    Real min_normal_length;

    rv = v1;

    length = VectorNorm(v1);

    min_normal_length = 0.000000000001f;

    if (length < min_normal_length) {
        rv.x = 1.0;
        rv.y = 0.0;

        return rv;
    }

    factor = 1.0 / length;

    rv.x *= factor;
    rv.y *= factor;

    return rv;
}
/**
 * \brief Creates a zero-initialized vector (0, 0).
 * \return A new vector with both components set to zero.
 */
/* create a initialized vector */
inline Vector2 VectorCreate() {
    Vector2 ret;
    ret.x = 0.0;
    ret.y = 0.0;
    return ret;
}

/**
 * \brief Creates a copy of the given vector.
 * \param n Source vector to copy.
 * \return A new vector equal to n.
 */
inline Vector2 VectorCreate(const Vector2& n) {
    Vector2 copy = n;
    return copy;
}

/**
 * \brief Creates a vector from two scalar components.
 * \param x Value for the x component.
 * \param y Value for the y component.
 * \return A new vector (x, y).
 */
inline Vector2 VectorCreate(Real x, Real y) {
    Vector2 copy;
    copy.x = x;
    copy.y = y;
    return copy;
}

/**
 * \brief Creates a vector with both components set to the same scalar value.
 * \param value Value assigned to both x and y.
 * \return A new vector (value, value).
 */
inline Vector2 VectorCreate(Real value) {
    Vector2 copy;
    copy.x = value;
    copy.y = value;
    return copy;
}
/**
 * \brief Returns a unit normal to the input vector, rotated +90 degrees.
 *        The input is first normalized (falling back to (1, 0) for a near-zero
 *        vector), then rotated so that v1 x result > 0 (inward orientation).
 * \param v1 Input vector.
 * \return A unit normal vector (-ny, nx) where (nx, ny) is the normalized input.
 */
// inward v1 x n > 0
inline Vector2 VectorNormalVector(const Vector2& v1) {
    Vector2 rv;

    Real length;
    Real factor;
    Real min_normal_length;

    rv = v1;

    length = VectorNorm(v1);

    min_normal_length = 0.000000000001f;

    if (length < min_normal_length) {
        rv.x = 1.0;
        rv.y = 0.0;

        return rv;
    }

    factor = 1.0 / length;

    rv.x *= factor;
    rv.y *= factor;

    return VectorCreate(-rv.y, rv.x);
}
}  // namespace ns_common

#endif  // !SRC_COMMON_OPENDFN_VECTOR2UTILS_H_
