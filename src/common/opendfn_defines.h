/******************************************************************************
 * \file      opendfn_defines.h
 * \brief     Basic macros predefined for the opendfn project.
 *
 * \details   Declarations for basic macros predefined for the opendfn project.
 *
 * \author    Xiaofeng Li
 * \email     xfli@whrsm.ac.cn
 * \date      Created:       June 24, 2020
 * \date      Last modified: 2026-07-20 01:03:12
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

#ifndef SRC_COMMON_OPENDFN_DEFINE_H_
#define SRC_COMMON_OPENDFN_DEFINE_H_

#include <stdexcept>
#include <sstream>

/**
 * \def OPENDFN_COMPILED_IN_LINUX
 * \brief Defined when the target operating system is detected as Linux.
 *
 * \def OPENDFN_COMPILED_IN_WINDOWS
 * \brief Defined when the target operating system is detected as Windows.
 *
 * The compiler's predefined OS macros are inspected to select exactly one of
 * these platform markers, which downstream code uses for OS-specific branches.
 */
// Defining the OS
#if defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__)
#    define OPENDFN_COMPILED_IN_LINUX
#elif defined(_WIN32) || defined(_WIN64)
#    define OPENDFN_COMPILED_IN_WINDOWS
#endif

/**
 * \def OPENDFN_ENV64
 * \brief Defined when compiling for a 64-bit architecture.
 *
 * \def OPENDFN_ENV32
 * \brief Defined when compiling for a 32-bit architecture.
 *
 * On Windows the choice is driven by _WIN64/_WIN32; on POSIX platforms it is
 * derived from known 64-bit architecture macros (x86_64, ppc64, aarch64),
 * defaulting to 32-bit otherwise.
 */
// Defining the architecture (see https://sourceforge.net/p/predef/wiki/Architectures/)
// Check Windows
#if defined(_WIN32) || defined(_WIN64)
#    if defined(_WIN64)
#        define OPENDFN_ENV64
#    else
#        define OPENDFN_ENV32
#    endif
#else  // It is POSIX (Linux, MacOSX, BSD...)
#    if defined(__x86_64__) || defined(__ppc64__) || defined(__aarch64__)
#        define OPENDFN_ENV64
#    else  // This includes __arm__ and __x86__
#        define OPENDFN_ENV32
#    endif
#endif

/**
 * \def stricmp
 * \brief Maps the Windows case-insensitive string compare to the POSIX
 *        strcasecmp on Linux so a single spelling works across platforms.
 *
 * \def strnicmp
 * \brief Maps the Windows length-bounded case-insensitive compare to the
 *        POSIX strncasecmp on Linux.
 */
// Defining char function for linux
#ifdef __linux__
#    define stricmp strcasecmp
#    define strnicmp strncasecmp
#endif

#endif  // SRC_COMMON_OPENDFN_MACROS_H_
