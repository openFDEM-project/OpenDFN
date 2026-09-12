/******************************************************************************
 * \file      logger.h
 * \brief     Main call files and module loop in the main function
 *
 * \details   Declarations for main call files and module loop in the main
 *            function.
 *
 * \author    Xiaofeng Li
 * \email     xfli@whrsm.ac.cn
 * \date      Created:       June 24, 2020
 * \date      Last modified: 2026-07-20 01:03:15
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

#ifndef SRC_COMMON_OPENDFN_LOGGER_H
#define SRC_COMMON_OPENDFN_LOGGER_H

#define OPENDFN_DIVIDING_HEAD                                                                                                   \
    OpenDFNMessage::Runtime_message(                                                                                            \
        "----------------------------------------------------------------------------------------------------------");

#define LOG_ERR_HEADER "_______________________________________________________"
#define LOG_ERR_TAIL "_______________________________________________________\a\n"
#define PRINT_HEADER(stream) fprintf(stream, "%s", LOG_ERR_HEADER)
#define PRINT_TAIL(stream) fprintf(stream, "%s", LOG_ERR_TAIL)

#include <cstdio>
#include <string>

namespace ns_common {
/**
 * Logger class used by OOFEM to print information during analysis.
 * Prints warnings and errors into a separate stream from normal output.
 */
class Logger {
public:
    /// Type defining basic log levels.
    enum logLevelType {
        LOG_LEVEL_FORCED   = -1,
        LOG_LEVEL_FATAL    = 0,
        LOG_LEVEL_ERROR    = 0,
        LOG_LEVEL_WARNING  = 1,
        LOG_LEVEL_RELEVANT = 2,
        LOG_LEVEL_INFO     = 3,
        LOG_LEVEL_ALL      = 4,
        LOG_LEVEL_DEBUG    = 4
    };

protected:
    /// Stream used for logging.
    FILE *logStream, *errStream;
    /// flag indicating whether to close mylogStream.
    bool closeFlag, errCloseFlag;
    /// Current log level, messages with higher level are not reported.
    logLevelType logLevel;
    /// Counter of all warning and error messages.
    int numberOfWrn, numberOfErr;

public:
    /**
     * \brief Constructs a logger and sets its initial verbosity threshold.
     * \param level Initial log level; messages with a higher numeric level are suppressed.
     */
    Logger(logLevelType level);
    /**
     * \brief Destroys the logger, flushing and closing any owned log/error streams.
     *
     * Streams are only closed when their close flag (closeFlag / errCloseFlag) is set.
     */
    ~Logger();
    /// Redirects log output to given file name (with path).
    void appendLogTo(const std ::string& fname);
    /// Redirects error output to given file name (with path).
    void appendErrorTo(const std ::string& fname);
    /// Redirects log output to given stream.
    void appendLogTo(FILE* stream);
    /// Redirects error output to given stream.
    void appendErrorTo(FILE* stream);

    /// Writes the normal log message.
    void writeLogMsg(logLevelType level, const char* format, ...);
    /// Writes extended log message with file and line info.
    void writeELogMsg(logLevelType level, const char* _func, const char* _file, int _line, const char* format, ...);
    /// Writes exit information
    void writeExitMsg(const char* _func, const char* _file, int _line);
    /// write information to be implemented
    void writeImplementLogMsg(const char* _func, const char* _file, int _line);

    /// Flushes the log stream.
    void flush() {
        fflush(logStream);
        fflush(errStream);
    }

    /// Sets log level to given one. Only log messages with level less or equal given threshold will be printed.
    void setLogLevel(logLevelType level) {
        logLevel = level;
    }
    /// Sets log level to given one. Only log messages with level less or equal given threshold will be printed.
    void setLogLevel(int level);
    /// Increment error count by one
    void incrementErrorCounter() {
        numberOfErr++;
    }
    /// Prints number of errors and warning logged.
    void printStatistics();

protected:
    const char* giveLevelName(logLevelType l) const;
};
extern Logger logger;

/**
 * Log reporting macros
 */
//@{
#define OPENDFN_LOG_FATAL(...)                                                                                                  \
    ns_common::logger.writeELogMsg(Logger ::LOG_LEVEL_FATAL, __func__, __FILE__, __LINE__, __VA_ARGS__)
#define OPENDFN_LOG_ERROR(...)                                                                                                  \
    {                                                                                                                            \
        ns_common::logger.writeELogMsg(Logger ::LOG_LEVEL_ERROR, __func__, __FILE__, __LINE__, __VA_ARGS__);                     \
        OPENDFN_EXIT(OPENDFN_EXIT_FAILURE)                                                                                     \
    }
#define OPENDFN_LOG_WARNING(...)                                                                                                \
    ns_common::logger.writeELogMsg(Logger ::LOG_LEVEL_WARNING, __func__, __FILE__, __LINE__, __VA_ARGS__)
#define OPENDFN_TO_IMPLEMENT ns_common::logger.writeImplementLogMsg(__func__, __FILE__, __LINE__)

#define OPENDFN_LOG_FORCED(...) ns_common::logger.writeLogMsg(Logger ::LOG_LEVEL_FORCED, __VA_ARGS__)
#define OPENDFN_LOG_RELEVANT(...) ns_common::logger.writeLogMsg(Logger ::LOG_LEVEL_RELEVANT, __VA_ARGS__)
#define OPENDFN_LOG_INFO(...) ns_common::logger.writeLogMsg(Logger ::LOG_LEVEL_INFO, __VA_ARGS__)
#define OPENDFN_LOG_DEBUG(...) ns_common::logger.writeLogMsg(Logger ::LOG_LEVEL_DEBUG, __VA_ARGS__)

//@}
}  // namespace ns_common
#endif  // SRC_COMMON_OPENDFN_LOGGER_H
