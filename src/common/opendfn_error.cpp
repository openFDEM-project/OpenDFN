/******************************************************************************
 * \file      opendfn_error.cpp
 * \brief     Main call files and module loop in the main function
 *
 * \details   Implementation of main call files and module loop in the main
 *            function.
 *
 * \author    Xiaofeng Li
 * \email     xfli@whrsm.ac.cn
 * \date      Created:       June 24, 2020
 * \date      Last modified: 2026-07-20 01:03:44
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

#include "opendfn_error.h"
#include <cstdarg>
#include <string>

namespace ns_common {
/**
 * \brief Builds a diagnostic string from a function identifier.
 *
 * Wraps the given C-string function name in a std::string, providing a
 * convenience for embedding the originating function in error messages.
 *
 * \param func Null-terminated function name (typically __func__).
 * \return A std::string copy of \p func.
 */
std::string errorInfo(const char* func) {
    return std ::string(func);
}

/**
 * \brief Constructs a runtime exception with a formatted message annotated
 *        by the originating source location.
 *
 * The variadic \p format arguments are rendered with printf-style formatting
 * into an internal buffer and stored in the member \c msg; a second line
 * recording the function, file, and line is then appended. Mutates the
 * object's \c msg state.
 *
 * \param _func   Name of the function raising the exception.
 * \param _file   Source file where the exception is raised.
 * \param _line   Line number where the exception is raised.
 * \param format  printf-style format string for the message body.
 * \param ...     Variadic arguments consumed by \p format.
 */
RuntimeException::RuntimeException(const char* _func, const char* _file, int _line, const char* format, ...) {
    char    stream[500];
    va_list args;
    va_start(args, format);
    vsprintf(stream, format, args);
    va_end(args);
    msg = stream;

    sprintf(stream, "\nIn %s (%s:%d)", _func, _file, _line);
    msg += stream;
}

/**
 * \brief Returns the stored diagnostic message.
 *
 * Overrides std::exception::what to expose the formatted message and source
 * location captured at construction time. Does not modify object state.
 *
 * \return Null-terminated pointer to the internal message buffer.
 */
const char* RuntimeException::what() const noexcept {
    return msg.c_str();
}
}  // namespace ns_common