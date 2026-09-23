/******************************************************************************
 * \file      opendfn_message.h
 * \brief     Static helper class for formatted runtime and log messages in
 *            OpenDFN.
 *
 * \details   Declarations for static helper class for formatted runtime and log
 *            messages in OpenDFN.
 *
 * \author    Xiaofeng Li
 * \email     xfli@whrsm.ac.cn
 * \date      Created:       June 24, 2020
 * \date      Last modified: 2026-07-20 01:06:30
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

#ifndef SRC_COMMON_OPENDFN_MESSAGE_HPP
#define SRC_COMMON_OPENDFN_MESSAGE_HPP

#include <common/opendfn_macros.h>

/* to clear small define in windows.h -> winscard.h -> wtypes.h -> rpcndr.h */
#ifdef small
#    undef small
#endif

namespace ns_common {
/**
 * \brief Collection of static helpers for emitting formatted runtime messages.
 *
 * Every method writes an informational line to stdout (or a caller-supplied
 * file) using the shared writeCharValue / writeIntValue primitives. The helpers
 * standardise the "Info     :" prefix and the bracketed value formatting used
 * throughout OpenDFN's console and log output.
 */
class OpenDFNMessage {
public:
    /**
     * print a string to screen.
     * \code
     * Runtime_message("exit with error.");
     * => exit with error.\n
     * \endcode
     *
     * \param c1
     */
    static void Runtime_message(const char* c1)  // print message out to screen
    {
        writeCharValue(stdout, c1);
        writeCharValue(stdout, "\n");
    }
    /**
     * print a string to screen.
     * \code
     * Runtime_message("exit with error.");
     * =>Info     :exit with error.\n
     * \endcode
     *
     * \param c1
     */
    static void RuntimeInfo(const char* c1)  // print message out to screen
    {
        printf("Info     :");
        writeCharValue(stdout, c1);
        writeCharValue(stdout, "\n");
    }
    /**
     * print a formated string to screen.
     * \code
     * Runtime_strformat("exit with error.");
     * =>Info     :exit with error.\n
     * \endcode
     *
     * \param c1
     */
    static void Runtime_strformat(const char* c1)  // print message out to screen
    {
        printf("Info     :");
        writeCharValue(stdout, c1);
        writeCharValue(stdout, "\n");
    }
    /**
     * print error message for keword parsing.
     * \code
     * RuntimeKeywordError("geometry",12)
     * =>Info     :Error: unknow keyword [geometry] at line [12]. \n
     * \endcode
     * \param s1
     * \param i1
     */
    static void RuntimeKeywordError(const char* s1, int i1)  // print message out to screen
    {
        printf("Info     :");
        writeCharValue(stdout, "Error: unknown keyword ");
        writeCharValue(stdout, "[");
        writeCharValue(stdout, s1);
        writeCharValue(stdout, "] ");
        writeCharValue(stdout, "at line ");
        printf("[%d", i1);
        writeCharValue(stdout, "].");
        writeCharValue(stdout, "\n");
    }
    /**
     * \brief Print a line of the form "prefix s1 [i1] s2 [i2]." to stdout.
     * \param s1 First label text.
     * \param i1 First integer value, printed in brackets after s1.
     * \param s2 Second label text.
     * \param i2 Second integer value, printed in brackets after s2.
     */
    static void RuntimeStringIntStringInt(const char* s1, int i1, const char* s2, int i2) {
        printf("Info     :");
        writeCharValue(stdout, s1);
        printf(" [%d", i1);
        writeCharValue(stdout, "] ");
        writeCharValue(stdout, s2);
        printf(" [%d", i2);
        writeCharValue(stdout, "].");
        writeCharValue(stdout, "\n");
    }
    /**
     * print a string to file.
     * \code
     * Runtime_message("exit with error.");
     * =< exit with error.\n
     * \endcode
     *
     * \param c1
     */
    static void RuntimeStringtoFile(FILE* fileptr, const char* c1)  // print message out to screen
    {
        writeCharValue(fileptr, c1);
        writeCharValue(fileptr, "\n");
    }
    /**
     * print a sentence composed by two strings. \n
     * \code
     * RuntimebackStringString("geometry group is","rock.")
     * =>Info     : geometry group is [rock]. \n
     * \endcode
     * \param c1
     * \param c2
     */
    static void RuntimebackStringString(const char* c1, const char* c2)  // print message out to screen
    {
        printf("Info     :");
        writeCharValue(stdout, c1);
        writeCharValue(stdout, "[");
        writeCharValue(stdout, c2);
        writeCharValue(stdout, "].");
        writeCharValue(stdout, "\n");
    }
    /**
     * print a sentence composed by two strings. \n
     * \code
     * RuntimebackStringString("mesh.msh"," file is imported.")
     * =>Info     : [mesh.msh] file is imported. \n
     *
     * \param c1
     * \param c2
     */
    static void RuntimefrontStringString(const char* c1, const char* c2)  // print message out to screen
    {
        printf("Info     :");
        writeCharValue(stdout, "[");
        writeCharValue(stdout, c1);
        writeCharValue(stdout, "] ");
        writeCharValue(stdout, c2);
        writeCharValue(stdout, "\n");
    }
    /**
     * print a sentence composed by a integer and two strings.
     *
     * \code
     * RuntimeIntStringString(20,"nodes are added in group","rock.")
     * =>Info     : [20] nodes are added in group [rock]. \n
     * \endcode
     *
     * \param n1
     * \param c1
     * \param c2
     */
    static void RuntimeIntStringString(int n1, const char* c1, const char* c2)  // print message out to screen
    {
        printf("Info     :[%d", n1);
        writeCharValue(stdout, "] ");
        writeCharValue(stdout, c1);
        writeCharValue(stdout, "[");
        writeCharValue(stdout, c2);
        writeCharValue(stdout, "].");
        writeCharValue(stdout, "\n");
    }
    /**
     * print a sentence composed by a string and integer.
     * \code
     * RuntimeStringInt("The count of node in the model is ","2300")
     * =>Info     : The count of node in the model is [2300]. \n
     * \endcode
     *
     * \param c1
     * \param c2
     */
    static void RuntimeStringInt(const char* c1, int c2)  // print message out to screen
    {
        printf("Info     :");
        writeCharValue(stdout, c1);
        writeCharValue(stdout, "[");
        writeIntValue(stdout, c2, 1);
        writeCharValue(stdout, "].");
        writeCharValue(stdout, "\n");
    }
    /**
     * \brief Print an informational message from a std::string to stdout.
     * \param c1 Message text to print after the "Info     :" prefix.
     */
    static void RuntimeString(std::string c1)  // print message out to screen
    {
        printf("Info     :");
        writeCharValue(stdout, c1.c_str());
        writeCharValue(stdout, "\n");
    }
    /**
     * print a sentence composed by a string and double value.
     * \code
     * RuntimeStringReal("The stiffness of the rock is ","1.2e9")
     * =>Info     : The stiffness of the rock is [1.2e9]. \n
     * \endcode
     *
     * \param c1
     * \param c2
     */
    static void RuntimeStringReal(const char* c1, double c2)  // print message out to screen
    {
        printf("Info     :");
        writeCharValue(stdout, c1);
        writeCharValue(stdout, "[");
        printf("%.2e", c2);
        writeCharValue(stdout, "].");
        writeCharValue(stdout, "\n");
    }
    /**
     * print a sentence composed by a integer and a string.
     * \code
     * RuntimeStringReal("20","nodes are added in the model.")
     * =>Info     : [20] nodes are added in the model. \n
     * \endcode
     *
     * \param c1
     * \param c2
     */
    static void RuntimeIntString(int c2, const char* c1)  // print message out to screen
    {
        printf("Info     :[%d", c2);
        writeCharValue(stdout, "] ");
        writeCharValue(stdout, c1);
        writeCharValue(stdout, "\n");
    }
    /**
     * print a sentence composed by a string, integer and a string.
     * \code
     * RuntimeStringReal("Congratulation,","20","nodes are added in the model.")
     * =>Info     :Congratulation, [20] nodes are added in the model. \n
     * \endcode
     *
     * \param s1
     * \param c2
     * \param s2
     */
    static void RuntimeStringIntString(const char* s1, int c2, const char* s2)  // print message out to screen
    {
        printf("Info     :");
        writeCharValue(stdout, s1);
        printf(" [%d", c2);
        writeCharValue(stdout, "] ");
        writeCharValue(stdout, s2);
        writeCharValue(stdout, "\n");
    }
    /**
     * print a sentence composed by a string, integer and a string.
     * \code
     * RuntimeStringReal("Congratulation,","20","nodes are added in the model.")
     * =>Info     :Congratulation, [20] nodes are added in the model. \n
     * \endcode
     *
     * \param s1
     * \param c2
     * \param s2
     */
    static void RuntimeStringStringString(const char* s1, const char* c2, const char* s2)  // print message out to screen
    {
        printf("Info     :");
        writeCharValue(stdout, s1);
        writeCharValue(stdout, "[");
        writeCharValue(stdout, c2);
        writeCharValue(stdout, "] ");
        writeCharValue(stdout, s2);
        writeCharValue(stdout, "\n");
    }
    /**
     * write parsing log to log file.
     *
     * \param checkfile
     * \param keyword
     */
    static void Runtime_writelogs(FILE* checkfile, const char* keyword)  // chech the parse postion for debuging
    {
        writeCharValue(checkfile, "Info     :parsing ");
        writeCharValue(checkfile, keyword);
        writeCharValue(checkfile, "\n");
    }
};
}  // namespace ns_common

#endif  // !_OPENDFN_MESSAGE_
