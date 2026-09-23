/******************************************************************************
 * \file      opendfn_vector2.h
 * \brief     Basic classes variables stored in common.
 *
 * \details   Declarations for basic classes variables stored in common.
 *
 * \author    Xiaofeng Li
 * \email     xfli@whrsm.ac.cn
 * \date      Created:       June 24, 2020
 * \date      Last modified: 2023-06-29 05:23:53
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

#ifndef _OPENDFN_LOCAL_VECTOR2_H
#define _OPENDFN_LOCAL_VECTOR2_H
#include <math.h>
#include "opendfn_math.h"
/**
 * The base vector2 class.
 */
struct Vector2 {
    Real x = 0.0;
    Real y = 0.0;
    // this.x
    Real normal() const;
    // this.y
    Real shear() const;

    // this.x
    Real& normal();
    // this.y
    Real& shear();

    // this.x
    Real n() const;
    // this.y
    Real s() const;

    // this.x
    Real& n();
    // this.y
    Real& s();

    // this = (x , y)
    void create(Real x, Real y);

    // this zero
    void create();

    // this = A
    void create(const Vector2& copy);

    // this = this + A
    void addby(const Vector2&);

    // this = scale * A
    void multiplyby(const Real scale, Vector2& vector);

    // this = A - B
    void subby(const Vector2& vectorA, const Vector2& vectorB);

    // this = A + B
    void addby(const Vector2& vectorA, const Vector2& vectorB);

    // this = 0.5 * (A + B)
    void averageby(const Vector2& vectorA, const Vector2& vectorB);

    // this = 1/3 * (A + B + C)
    void triaverageby(const Vector2& vectorA, const Vector2& vectorB, const Vector2& vectorC);

    // normalize ths vector
    // this = normalized( this )
    void normalized();

    // get the slope of this vector
    // ret = y / x
    Real getSlope();

    Real getTheta();

    // get the distance from b
    // ret = distance(this , A)
    Real getDistancefrom(const Vector2& n);

    // get the squared distance from b
    // ret = distance(this , A)^2
    Real getSquaredDistancefrom(const Vector2& n);

    // normalized the vector and store to this
    void normalizedby(const Vector2&);

    // returen sqrt(x*x + y*y)
    Real getNorm() const;

    // return x * x + y * y
    Real getSquaredNorm() const;

    // operator -=
    // this = this -b
    void operator-=(const Vector2& b);

    // operator +=
    // this = this + b
    void operator+=(const Vector2& b);

    // operator *=
    // this = this * b(Real)
    void operator*=(const Real& b);

    // operator /=
    // this = this / b(Real)
    void operator/=(const Real& b);

    // operator *=
    // this = this * b(Vector)
    void operator*=(const Vector2& b);

    // operator /=
    // this = this / b(Vector)
    void operator/=(const Vector2& b);

    // returen the inward nomral vector of line b-this
    // n = z x (b - this )
    Vector2 getNormalVectorInward(const Vector2& b);

    // returen the inward nomral vector of line b-a
    // this = z x (b - a )
    void getNormalVectorInwardfrom(const Vector2& a, const Vector2& b);

    // returen the outward nomral vector of line b-this
    // n = -z x (b - this )
    Vector2 getNormalVectorOutward(const Vector2& b);

    // returen the outward nomral vector of line b-a
    // this = -z x (b - a )
    void getNormalVectorOutwardfrom(const Vector2& a, const Vector2& b);

    // retured the suqared value if the commpoments is greater than 0.0
    Real getPostiveMag();

    /* compute vector dot opreator */
    // ret = this dot vec
    Real getdot(const Vector2& vector);

    /* compute vector cross opreator */
    /* ret = this x v2 */
    Real getcross(const Vector2& vector);

    // this = 0
    void Zero();
};


/**
 * return vector normal, read only.
 *
 * \return
 */
inline Real Vector2::normal() const {
    return x;
}
/**
 * return vector shear, read only.
 *
 * \return
 */
inline Real Vector2::shear() const {
    return y;
}
/**
 * return vector normal, read-write.
 *
 * \return
 */
inline Real& Vector2::normal() {
    return x;
}
/**
 * return vector shear, read-write.
 *
 * \return
 */
inline Real& Vector2::shear() {
    return y;
}
/**
 * return vector n, read only.
 *
 * \return
 */
inline Real Vector2::n() const {
    return x;
}
/**
 * return vector s, read only.
 *
 * \return
 */
inline Real Vector2::s() const {
    return y;
}
/**
 * return vector n, read-write.
 *
 * \return
 */
inline Real& Vector2::n() {
    return x;
}
/**
 * return vector s, read-write.
 *
 * \return
 */
inline Real& Vector2::s() {
    return y;
}
/**
 * this += B.
 *
 * \param B
 */
inline void Vector2::addby(const Vector2& B) {
    x += B.x;
    y += B.y;
}
/**
 * constructor by two real values.
 *
 * \param inputx
 * \param inputy
 */
inline void Vector2::create(Real inputx, Real inputy) {
    x = inputx;
    y = inputy;
}
/**
 * constructor from a vector.
 *
 * \param B
 */
inline void Vector2::create(const Vector2& B) {
    x = B.x;
    y = B.y;
}
/**
 * initialize.
 *
 */
inline void Vector2::create() {
    x = 0.0;
    y = 0.0;
}

/**
 * compute vector magnitude .
 *
 * \return
 */
inline Real Vector2::getPostiveMag() {
    Real mag = 0.0;
    if (x > 0.0)
        mag += x * x;
    if (y > 0.0)
        mag += y * y;
    return sqrt(mag);
}
//
/**
 *  get the slope of the vector, in y/x
 *
 * \return
 */
inline Real Vector2::getSlope() {
    return (y / (x + OpenDFN_pos_epsilon));
}

//
/**
 * get the slope of the vector, atan value, in rad
 *
 * \return
 */
inline Real Vector2::getTheta() {
    // atan range in c++ is : -pi / 2, pi / 2
    // theta range is : -pi / 2, pi / 2
    return atan(y / (x + OpenDFN_pos_epsilon));
}
/**
 * compute distance of two vectors.
 *
 * \param n
 * \return
 */
inline Real Vector2::getDistancefrom(const Vector2& n) {
    return sqrt((n.x - x) * (n.x - x) + (n.y - y) * (n.y - y));
}
/**
 * compute squared distance of two vectors.
 *
 * \param n
 * \return
 */
inline Real Vector2::getSquaredDistancefrom(const Vector2& n) {
    return ((n.x - x) * (n.x - x) + (n.y - y) * (n.y - y));
}

/**
 * compute inward normal of current and another vectors, right-hand rule, z x (b - this)
 * return the normal vector
 *
 * \param b
 * \return
 */
inline Vector2 Vector2::getNormalVectorInward(const Vector2& b) {
    Vector2 rv;
    // elx = b.x - this.x;
    // ely = b.y - this.y;
    // inward normal = (-ely, elx);
    rv.x = -b.y + y;
    rv.y = b.x - x;
    return rv;
}
/**
 * compute inward normal of two vectors, right-hand rule, z x (b - a).
 *
 * \param a
 * \param b
 */
inline void Vector2::getNormalVectorInwardfrom(const Vector2& a, const Vector2& b) {
    // elx = b.x - a.x;
    // ely = b.y - a.y;
    // inward normal = (-ely, elx);
    x = -b.y + a.y;
    y = b.x - a.x;
}
/**
 * compute outward normal of current and another vectors, right-hand rule, -z x (b - this)
 * return the normal vector
 *
 * \param b
 * \return
 */
inline Vector2 Vector2::getNormalVectorOutward(const Vector2& b) {
    Vector2 rv;
    // elx = b.x - this.x;
    // ely = b.y - this.y;
    // outward normal = (ely, -elx);
    rv.x = b.y - y;
    rv.y = -b.x + x;
    return rv;
}
/**
 *  compute outward normal of two vectors, right-hand rule, -z x (b - a).
 *
 * \param a
 * \param b
 */
inline void Vector2::getNormalVectorOutwardfrom(const Vector2& a, const Vector2& b) {
    // elx = b.x - a.x;
    // ely = b.y - a.y;
    // outward normal = (ely, -elx);
    x = b.y - a.y;
    y = -b.x + a.x;
}
/**
 * compute the vector multiplied by a scalar, and save to current vector.
 *
 * \param scale
 * \param vector
 */
inline void Vector2::multiplyby(const Real scale, Vector2& vector) {
    x = scale * vector.x;
    y = scale * vector.y;
}
/**
 * zero the vector.
 *
 */
inline void Vector2::Zero() {
    x = 0.0;
    y = 0.0;
}
/**
 * operator +=.
 *
 * \param b
 */
inline void Vector2::operator+=(const Vector2& b) {
    x += b.x;
    y += b.y;
}
/**
 * operator -=.
 *
 * \param b
 */
inline void Vector2::operator-=(const Vector2& b) {
    x -= b.x;
    y -= b.y;
}
/**
 * operator *= with another vector, by elementwise.
 *
 * \param b
 */
inline void Vector2::operator*=(const Vector2& b) {
    x *= b.x;
    y *= b.y;
}
/**
 * operator /= with another vector, by elementwise.
 * v.x = v.x / b.x
 *
 * \param b
 */
inline void Vector2::operator/=(const Vector2& b) {
    x /= b.x;
    y /= b.y;
}
/**
 * operator *= with a scalar.
 *
 * \param b
 */
inline void Vector2::operator*=(const Real& b) {
    x *= b;
    y *= b;
}
/**
 * operator /= with a scalar.
 *
 * \param b
 */
inline void Vector2::operator/=(const Real& b) {
    x /= b;
    y /= b;
}
/**
 * this = A -B .
 *
 * \param vectorA
 * \param vectorB
 */
inline void Vector2::subby(const Vector2& vectorA, const Vector2& vectorB) {
    x = vectorA.x - vectorB.x;
    y = vectorA.y - vectorB.y;
}
/**
 * this = A + B.
 *
 * \param vectorA
 * \param vectorB
 */
inline void Vector2::addby(const Vector2& vectorA, const Vector2& vectorB) {
    x = vectorA.x + vectorB.x;
    y = vectorA.y + vectorB.y;
}
/**
 * this = 0.5 *( A + B).
 *
 * \param vectorA
 * \param vectorB
 */
inline void Vector2::averageby(const Vector2& vectorA, const Vector2& vectorB) {
    x = 0.5 * (vectorA.x + vectorB.x);
    y = 0.5 * (vectorA.y + vectorB.y);
}
/**
 * this = 1/3 * (A + B + C).
 *
 * \param vectorA
 * \param vectorB
 * \param vectorC
 */
inline void Vector2::triaverageby(const Vector2& vectorA, const Vector2& vectorB, const Vector2& vectorC) {
    x = 1 / 3. * (vectorA.x + vectorB.x + vectorC.x);
    y = 1 / 3. * (vectorA.y + vectorB.y + vectorC.y);
}
/**
 * normalize this vector.
 *
 */
inline void Vector2::normalized() {
    Real length;
    Real min_normal_length;

    length            = sqrt(x * x + y * y);
    min_normal_length = 0.000000000001f;

    if (length < min_normal_length) {
        x = 1.0;
        y = 0.0;
    }

    Real factor = 1.0 / length;

    x *= factor;
    y *= factor;
}
/**
 * get the normalize from another vector.
 *
 * \param vector
 */
inline void Vector2::normalizedby(const Vector2& vector) {
    Real length;
    Real min_normal_length;

    length            = vector.x * vector.x + vector.y * vector.y;
    min_normal_length = 0.000000000001f;

    if (length < min_normal_length) {
        x = 1.0;
        y = 0.0;
    }

    Real factor = 1.0 / length;

    x = vector.x * factor;
    y = vector.y * factor;
}
/**
 * return the norm of this vector.
 *
 * \return
 */
inline Real Vector2::getNorm() const {
    return sqrt(x * x + y * y);
}
/**
 * return the squared norm of this vector.
 *
 * \return
 */
inline Real Vector2::getSquaredNorm() const {
    return (x * x + y * y);
}
/**
 * return the vector dot. return = this * A
 * return = this.x * A.x + this.y * A.y
 *
 * \param vector
 * \return
 */
inline Real Vector2::getdot(const Vector2& vector) {
    return x * vector.x + y * vector.y;
}
/**
 * return the cross of this vector with A. return = this x A
 *
 * \param vector
 * \return
 */
inline Real Vector2::getcross(const Vector2& vector) {
    return x * vector.y - y * vector.x;
}

#endif  // !_OPENDFN_LOCAL_VECTOR2_H
