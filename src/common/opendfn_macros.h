/******************************************************************************
 * \file      opendfn_macros.h
 * \brief     Basic macros predefined for the opendfn project.
 *
 * \details   Declarations for basic macros predefined for the opendfn project.
 *
 * \author    Xiaofeng Li
 * \email     xfli@whrsm.ac.cn
 * \date      Created:       June 24, 2020
 * \date      Last modified: 2026-07-20 01:05:39
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

#ifndef SRC_COMMON_OPENDFN_MACROS_H_
#define SRC_COMMON_OPENDFN_MACROS_H_

#include <cmath>
#include <string>
#include "opendfn_struct.h"

// Two-step token stringification: STRINGIZE expands its argument first, then
// STRINGIZE_ turns the expanded text into a string literal.
#define STRINGIZE_(x) #x
#define STRINGIZE(x) STRINGIZE_(x)

// Null aliases used for various pointer/handle types throughout the code base.
#define FileNull (nullptr)
#define Char2DNull (nullptr)
#define Double1DNull (nullptr)
#define Double2DNull (nullptr)
#define Double3DNull (nullptr)
#define Int1DNull (nullptr)
#define Int2DNull (nullptr)
#define NULL (nullptr)

// consider the compliation between different versions
#define _VERSION_COMPLIATION_

#define _USE_POLYGON_
// maximum element node count possible in the model
#ifdef _USE_POLYGON_
#    define POLYGON_NODE_SIZE 20
#else
#    define POLYGON_NODE_SIZE 8
#endif  // _USE_POLYGON_

// maximum element face count possible in the model
#define POLYFACE 6
// macro exit
#define OPENDFN_EXIT_SUCCESS 0
#define OPENDFN_EXIT_FAILURE 1

// macro for
#define _for(i, a, b) for (int i = a; i < b; i++)
// macro for matrix indices
#define Ind(i, j, __rows, __cols) ((i) * (__cols) + (j))

// macro for copy pointer, a and b should be the same structure, a_starter is the begin address you want to put
// and size is the count, b is the destination
#define copyRawArray(from, destination, size) std::copy(from, from + size, destination);
// macro copy stl array
#define copyArray(from, destination, size) std::copy(&from[0], &from[0] + size, destination.begin());
// macro for free a raw array, avoid hangling pointer
#define freeArray(list)                                                                                                          \
    {                                                                                                                            \
        free(list);                                                                                                              \
        list = nullptr;                                                                                                          \
    }

// macro copy vector from array, if you want to copy vector to vector
// directly use operator =
#define copyVector(from, destination, size) destination.assign(from, from + size);
// free a vector and free the memory
#define freeVector(list, type)                                                                                                   \
    if (list.size())                                                                                                             \
        std::vector<type>().swap(list);

// macro to compare two strings, no case
#define StringCompare(sting1, string2)                                                                                           \
    ([](const std::string& str1, const std::string& str2) {                                                                      \
        std::string lowerStr1 = str1;                                                                                            \
        std::string lowerStr2 = str2;                                                                                            \
        std::transform(lowerStr1.begin(), lowerStr1.end(), lowerStr1.begin(), ::tolower);                                        \
        std::transform(lowerStr2.begin(), lowerStr2.end(), lowerStr2.begin(), ::tolower);                                        \
        return lowerStr1.compare(lowerStr2);                                                                                     \
    })(sting1, string2)

// macro to compare two strings, no case
#define StringCompareN(sting1, string2, n)                                                                                       \
    ([](const std::string& str1, const std::string& str2, std::size_t count) {                                                   \
        std::string subStr1 = str1.substr(0, count);                                                                             \
        std::string subStr2 = str2.substr(0, count);                                                                             \
        std::transform(subStr1.begin(), subStr1.end(), subStr1.begin(), ::tolower);                                              \
        std::transform(subStr2.begin(), subStr2.end(), subStr2.begin(), ::tolower);                                              \
        return subStr1.compare(subStr2);                                                                                         \
    })(sting1, string2, n)

// macro copy chars to chars
#define copyChars(destination, from) strcpy(destination, from);

// macro to compare same string (chars), captial insentitive
#define isSameString(TestChar, TargetChar) !stricmp(TestChar, TargetChar)
// macro to same prefix (chars)
#define isSamePrefix(TestChar, TargetChar, size) !strnicmp(TestChar, TargetChar, size)

// ensure the same bits for all models
// plastic indicator
// 1
static const UInt shear_now = 0x001;
// 2
static const UInt tension_now = 0x002;
// 4
static const UInt shear_past = 0x004;
// 8
static const UInt tension_past = 0x008;

const TensorIndex _internalIndexes = {0, 1, -1, 2, 3, -1, -1, -1, 4};

// Triangle face map, 3 faces
static const Int TRITOEDGE[][2] = {{0, 1}, {1, 2}, {2, 0}};

// Quadratic triangle node map
static const Int TRITOQUADEDGE[3] = {3, 4, 5};

// Quadralateral face map, 4 faces
static const Int QUADTOEDGE[][2] = {{0, 1}, {1, 2}, {2, 3}, {3, 0}};

// Quadralateral  node map
static const Int QUADTOQUADEDGE[4] = {4, 5, 6, 7};

// Console Control
int static TrapCtrlCBreak = 1000;

// Max input arguments for geometry volume parameters
#define MAXARGC 30

// Max file tag
#define OpenDFN_ARGC_LENGTH 256

// Max file imported
#define OpenDFN_FILE_LIMIT 50

// OpenDFN dimension
#define OPENDFN_DIMENSION 2

// maximuum node count in CZM
#define CZM_NODE_SIZE 4
#define CZM_EDGE_SIZE 2
#define CZM_ELEMENT_SIZE 2

#define RANGE_SIZE 4
#define ID_RANGE_SIZE 2

#define EDGE_NODE_SIZE 2
// maximuum node count in CZM one surface
#define NHALFCZMNODE 2
// maximuum gauss count in CZM
#define NCZMGAUSS 3
// maximuum node count in the element,should be increased if you use higher-order element
#define NMATRIXNODE 20
// maximuum gauss count for the element, should be increased if you use higher-order element
#define NMATRIXGAUSS 10

// maximuum allowable groups
#define MAX_GROUP_SIZE 1000
// maximuum allowable materials
#define MAX_MATERIAL_SIZE 1000
#define MAT_MATRIX_TYPE_ID 0
#define MAT_CZM_TYPE_ID 1
#define MAT_CONTACT_TYPE_ID 2

#define MAT_HYDRO_TYPE_ID 3
#define MAT_HYDRO_MATRIX_TYPE_ID 30
#define MAT_HYDRO_FRACTURE_TYPE_ID 31
#define MAT_HYDRO_FLUID_TYPE_ID 32
#define MAT_HYDRO_GAS_TYPE_ID 33

#define MAT_THERMAL_TYPE_ID 4

// maximuum allowable boundaries
#define MAX_BOUNDARY_SIZE 1000
#define BOU_ON_NODE_TYPE_ID 0
#define BOU_ON_EDGE_TYPE_ID 1
#define BOU_ON_ELEMENT_TYPE_ID 2
#define BOU_ON_HYDRO_TYPE_ID 3
#define BOU_ON_THERMAL_TYPE_ID 4

/*
 * Standard mathematical defines functions
 */
#define precisionPolarDecompose 10.0E-10
#define GEOMETRYFPZTHICKNESS 1E-3

// define constant values for Evans-Marathe FPZ model
#define Evans_Marathe_a 0.63
#define Evans_Marathe_b 1.8
#define Evans_Marathe_a 6.0

// message

// Pre-built printf/scanf format-string tables indexed by desired field width /
// precision, used by the write*Value macros below (DoubleString for reals,
// IntString for integers, CharString for strings).
static const char* DoubleString[20] = {"%le",      "%+1.0le",  "%+2.0le",  "%+3.0le",   "%+4.0le",   "%+5.0le",  "%+6.0le",
                                       "%+7.0le",  "%+8.1le",  "%+9.2le",  "%+10.3le",  "%+11.4le",  "%+12.5le", "%+13.6le",
                                       "%+14.7le", "%+15.8le", "%+16.9le", "%+17.10le", "%+18.11le", "%+19.12le"};
static const char* IntString[20]    = {"%ld",   "%1ld",  "%2ld",  "%3ld",  "%4ld",  "%5ld",  "%6ld",  "%7ld",  "%8ld",  "%9ld",
                                    "%10ld", "%11ld", "%12ld", "%13ld", "%14ld", "%15ld", "%16ld", "%17ld", "%18ld", "%19ld"};
static const char* CharString[30]   = {"%s",   "%1s",  "%2s",  "%3s",  "%4s",  "%5s",  "%6s",  "%7s",  "%8s",  "%9s",
                                     "%10s", "%11s", "%12s", "%13s", "%14s", "%15s", "%16s", "%17s", "%18s", "%19s",
                                     "%20s", "%21s", "%22s", "%23s", "%24s", "%25s", "%26s", "%27s", "%28s", "%29s"};

// Formatted read/write helpers over a FILE*: get*Value scan a single value into
// pointer x; write*Value print value x, with the *Int/*Real/*String variants
// selecting a width via the ndigit index into the format tables above.
#define getIntValue(file, x)                                                                                                     \
    { fscanf((file), "%ld", ((x))); }

#define getRealValue(file, x)                                                                                                    \
    { fscanf((file), "%le", ((x))); }

#define getCharValue(file, x)                                                                                                    \
    { fscanf((file), "%s", ((x))); }

#define writeCharValue(file, x)                                                                                                  \
    { fprintf((file), "%s", ((x))); }

#define writeIntValue(file, x, ndigit) fprintf((file), (IntString[(ndigit)]), ((x)));

#define writeRealValue(file, x, ndigit) fprintf((file), (DoubleString[((ndigit))]), (x));

#define writeStringValue(file, x, ndigit)                                                                                        \
    { fprintf((file), (CharString[(ndigit)]), (x)); };

#endif  // SRC_COMMON_OPENDFN_MACROS_H_
