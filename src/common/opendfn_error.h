/******************************************************************************
 * \file      opendfn_error.h
 * \brief     Main call files and module loop in the main function
 *
 * \details   Declarations for main call files and module loop in the main
 *            function.
 *
 * \author    Xiaofeng Li
 * \email     xfli@whrsm.ac.cn
 * \date      Created:       June 24, 2020
 * \date      Last modified: 2026-07-20 01:03:07
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

#ifndef SRC_COMMON_OPENDFN_ERROR_H
#define SRC_COMMON_OPENDFN_ERROR_H

#define OPENDFN_DEBUG

#include "logger.h"

#include <cstdio>
#include <cstdlib>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <utility>

namespace ns_common {
/** Cause opendfn program termination by calling exit. */
#define OPENDFN_EXIT(code)                                                                                                      \
    {                                                                                                                            \
        ns_common::logger.writeExitMsg(__func__, __FILE__, __LINE__);                                                            \
        fprintf(stderr, "OpenDFN programe exit code %d\n", code);                                                               \
        printf("Press any key to exit...\n");                                                                                    \
        system("pause");                                                                                                         \
        exit(code);                                                                                                              \
    }

/**
 * \brief Exception type thrown by the OPENDFN_FATAL / OPENDFN_ERROR macros.
 *
 * Carries a formatted diagnostic message that records where the error was
 * raised (function, file, line) together with a user-supplied printf-style
 * description. Derives from std::exception so it can be caught generically.
 */
class RuntimeException : public std::exception {
public:
    std::string msg;  ///< Fully formatted error message (location + description).

    /**
     * \brief Construct a runtime exception with a formatted message.
     * \param _func   Name of the function where the error was raised (e.g. __func__).
     * \param _file   Source file where the error was raised (e.g. __FILE__).
     * \param _line   Source line where the error was raised (e.g. __LINE__).
     * \param format  printf-style format string for the error description.
     * \param ...     Variadic arguments matching \p format.
     */
    RuntimeException(const char* _func, const char* _file, int _line, const char* format, ...);

    /**
     * \brief Return the stored diagnostic message.
     * \return C-string pointer to the formatted message (owned by this object).
     */
    const char* what() const noexcept override;
};

/**
 * Macros for printing errors.
 * This macro can be used only within classes that implement errorInfo function.
 */
#define OPENDFN_FATAL(...)                                                                                                      \
    { throw RuntimeException::RuntimeException(__func__, __FILE__, __LINE__, __VA_ARGS__); }
#define OPENDFN_ERROR(...)                                                                                                      \
    { throw RuntimeException::RuntimeException(__func__, __FILE__, __LINE__, __VA_ARGS__); }
#define OPENDFN_WARNING(...)                                                                                                    \
    logger.writeELogMsg(Logger ::LOG_LEVEL_WARNING, errorInfo(__func__).c_str(), __FILE__, __LINE__, __VA_ARGS__)
#define OPENDFN_SERROR(...)                                                                                                     \
    {                                                                                                                            \
        logger.writeELogMsg(Logger ::LOG_LEVEL_ERROR, __func__, __FILE__, __LINE__, __VA_ARGS__);                                \
        OPENDFN_EXIT(1);                                                                                                        \
    }
#define OPENDFN_SWARNING(...) logger.writeELogMsg(Logger ::LOG_LEVEL_WARNING, __func__, __FILE__, __LINE__, __VA_ARGS__)

/**
 * Macros for debugging in function.
 */
#define OPENDFN_LOCATION                                                                                                        \
    "{ " << std::string(__func__) << "() : " << std::string(__FILE__) << ", at line: " << std::to_string(__LINE__) << "} "

#define OPENDFN_STRINGSTREAM_IN(_str, _sstr)                                                                                    \
    ;                                                                                                                            \
    do {                                                                                                                         \
        std::stringstream _dbg_s_info;                                                                                           \
        _dbg_s_info << _sstr;                                                                                                    \
        (_str) = _dbg_s_info.str();                                                                                              \
    } while (false)

#define OPENDFN_DEBUG_(pref, info)                                                                                              \
    do {                                                                                                                         \
        std::string _dbg_str;                                                                                                    \
        OPENDFN_STRINGSTREAM_IN(_dbg_str, info << " " << OPENDFN_LOCATION);                                                    \
        char* b = (char*)_dbg_str.c_str();                                                                                       \
        std::cout << pref << " " << b << std::endl;                                                                              \
    } while (false)

#ifdef OPENDFN_DEBUG
#    define OPENDFN_DEBUG_IN() OPENDFN_DEBUG_("==>", __func__ << "() step in")
#    define OPENDFN_DEBUG_OUT() OPENDFN_DEBUG_("<==", __func__ << "() step out")
#elif
#    define OPENDFN_DEBUG_IN()
#    define OPENDFN_DEBUG_OUT()
#endif

/**
 * \brief Build a standardized error-info prefix string for a given function.
 * \param func Name of the function requesting the prefix (e.g. __func__).
 * \return A formatted string used to prefix warning/error log messages.
 */
std::string errorInfo(const char* func);
}  // end namespace ns_common
#endif  // SRC_COMMON_OPENDFN_ERROR_H
