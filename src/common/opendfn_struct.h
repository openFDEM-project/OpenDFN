/******************************************************************************
 * \file      opendfn_struct.h
 * \brief     Basic classes variables stored in common.
 *
 * \details   Declarations for basic classes variables stored in common.
 *
 * \author    Xiaofeng Li
 * \email     xfli@whrsm.ac.cn
 * \date      Created:       June 24, 2020
 * \date      Last modified: 2026-07-20 01:04:38
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

#ifndef _OPENDFN_LOCAL_STRUCT_H
#define _OPENDFN_LOCAL_STRUCT_H

#include <math.h>
#include <opendfn_defines.h>
#include <stdint.h>
#include <stdlib.h>

// charactor size of one keyword
#define MAX_KEYWORD_LENGTH 2000
// the maximuum count of input keyword count
#define MAX_KEYWORDS 1000
// the maximuum character of one line
#define MAX_CHARACTER 1024

/* no use #define to avoid conflicts with eigen */
// uint32_t
using UInt = unsigned int;
// int64_t, long 
using Int = long;
// double
using Real = double;

//#define _DUMPER_AE_

#ifndef _OPENMPI_
#    define _OPENMPI_
#endif  // !_OPENMPI_

#ifdef _OPENMPI_
#    include <omp.h>
#endif

/**
 * @brief Compact lookup table mapping the two indices of a 2nd-order tensor
 *        component to a flattened storage slot.
 *
 * The 3x3 @c index array stores, for each (i, j) pair, the position of the
 * corresponding component in a packed (e.g. Voigt-style) representation.
 */
struct TensorIndex {
    short index[3][3];  // Set of indexes
};

/**
 * @brief Container holding the tokenised keywords parsed from a single input
 *        line of an OpenDFN data file.
 *
 * @c copy keeps the raw copy of the original line, @c keywords holds pointers
 * to each extracted token (allocated dynamically and released by clean()),
 * @c count is the number of tokens actually parsed and @c findbracket records
 * whether a bracket was encountered while tokenising.
 */
struct Keywords {
    // the origional data
    char copy[MAX_CHARACTER];
    // the maximuum keywords are 40
    char* keywords[MAX_KEYWORDS];
    // maximuum count
    Int MAXNUM = MAX_KEYWORDS;
    // the actual count of keywords
    Int count = 0;
    // the flag of bracket
    bool findbracket = false;
    Keywords() {}
    /**
     * @brief Free every dynamically allocated keyword token.
     *
     * Releases the @c count pointers stored in @c keywords via free(). Must be
     * called once the parsed tokens are no longer needed to avoid leaks; the
     * @c count field itself is left unchanged.
     */
    void clean();
};

/**
 * @brief Out-of-line definition of Keywords::clean().
 *
 * Iterates over the first @c count entries of @c keywords and frees each one.
 */
inline void Keywords::clean() {
    for (Int i = 0; i < count; i++)
        free(keywords[i]);
}
#endif  // !_OPENDFN_LOCAL_STRUCT_
