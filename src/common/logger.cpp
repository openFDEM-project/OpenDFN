/******************************************************************************
 * \file      logger.cpp
 * \brief     Main call files and module loop in the main function
 *
 * \details   Implementation of main call files and module loop in the main
 *            function.
 *
 * \author    Xiaofeng Li
 * \email     xfli@whrsm.ac.cn
 * \date      Created:       June 24, 2020
 * \date      Last modified: 2026-07-20 01:04:50
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

#include "logger.h"
#include <cstdarg>
#include <opendfn_macros.h>
#include "opendfn_error.h"
#include <opendfn_common.h>

namespace ns_common {
/**
 * \brief Global default logger instance used throughout OpenDFN.
 *
 * Initialized at LOG_LEVEL_INFO; writes normal messages to stdout and
 * warnings/errors to stderr until reconfigured via appendLogTo/appendErrorTo.
 */
// Default log output
Logger logger(Logger ::LOG_LEVEL_INFO);

/**
 * \brief Constructs a Logger with the given verbosity level.
 * \param level Initial log level; messages with a level higher (more verbose)
 *              than this are suppressed.
 *
 * Side effects: directs the log stream to stdout and the error stream to
 * stderr, clears the file-close flags, and resets warning/error counters.
 */
Logger ::Logger(logLevelType level)
    : logStream(stdout), errStream(stderr), closeFlag(false), errCloseFlag(false), logLevel(level), numberOfWrn(0), numberOfErr(0)

{}

/**
 * \brief Destructor: closes any log/error files that this Logger opened.
 *
 * Side effects: calls fclose on the log and/or error streams only when the
 * corresponding close flag indicates the Logger owns that file handle.
 */
Logger ::~Logger() {
    if (this->closeFlag) {
        fclose(this->logStream);
    }
    if (this->errCloseFlag) {
        fclose(this->errStream);
    }
}

/**
 * \brief Redirects normal log output to a file, opened in append mode.
 * \param fname Path of the file to append log messages to.
 *
 * Side effects: opens (or reopens, if a log file was already owned) \p fname
 * for appending and sets the log stream to it; emits a warning via
 * OPENDFN_WARNING if the file cannot be opened. Marks the log stream as owned
 * so it will be closed on destruction.
 */
void Logger ::appendLogTo(const std ::string& fname) {
    FILE* stream = NULL;
    if (this->closeFlag) {
        stream = freopen(fname.c_str(), "a", this->logStream);
    } else {
        stream = fopen(fname.c_str(), "a");
    }

    if (stream == NULL) {
        OPENDFN_WARNING("file opening error (%s)", fname.c_str());
    } else {
        this->logStream = stream;
    }

    this->closeFlag = true;
}

/**
 * \brief Redirects error/warning output to a file, opened in append mode.
 * \param fname Path of the file to append error messages to.
 *
 * Side effects: opens (or reopens, if an error file was already owned)
 * \p fname for appending and sets the error stream to it; emits a warning via
 * OPENDFN_WARNING if the file cannot be opened. Marks the error stream as
 * owned so it will be closed on destruction.
 */
void Logger ::appendErrorTo(const std ::string& fname) {
    FILE* stream = NULL;
    if (this->errCloseFlag) {
        stream = freopen(fname.c_str(), "a", this->errStream);
    } else {
        stream = fopen(fname.c_str(), "a");
    }

    if (stream == NULL) {
        OPENDFN_WARNING("file opening error (%s)", fname.c_str());
    } else {
        this->errStream = stream;
    }

    this->errCloseFlag = true;
}

/**
 * \brief Redirects normal log output to an already-open FILE stream.
 * \param stream Destination stream; must not be NULL.
 *
 * Side effects: closes a previously owned log file if any, then sets the log
 * stream to \p stream. Reports a fatal error via OPENDFN_ERROR if \p stream
 * is NULL. Marks the stream as not owned (it will not be closed on
 * destruction).
 */
void Logger ::appendLogTo(FILE* stream) {
    if (this->closeFlag) {
        fclose(this->logStream);
    }

    if (stream == NULL) {
        OPENDFN_ERROR("Logger::appendLogTo : null stream given");
    } else {
        this->logStream = stream;
    }

    this->closeFlag = false;
}

/**
 * \brief Redirects error/warning output to an already-open FILE stream.
 * \param stream Destination stream; must not be NULL.
 *
 * Side effects: closes a previously owned error file if any, then sets the
 * error stream to \p stream. Reports a fatal error via OPENDFN_ERROR if
 * \p stream is NULL. Marks the stream as not owned (it will not be closed on
 * destruction).
 */
void Logger ::appendErrorTo(FILE* stream) {
    if (this->errCloseFlag) {
        fclose(this->errStream);
    }

    if (stream == NULL) {
        OPENDFN_ERROR("Logger::appendLogTo : null stream given");
    } else {
        this->errStream = stream;
    }

    this->errCloseFlag = false;
}

/**
 * \brief Writes a printf-style log message at the given level.
 * \param level  Severity of the message.
 * \param format printf-style format string, followed by matching arguments.
 *
 * Side effects: selects the error stream for FATAL/ERROR/WARNING levels (and
 * increments the error or warning counter accordingly), otherwise uses the
 * log stream. The message is emitted only when \p level is at or below the
 * configured log level.
 */
void Logger ::writeLogMsg(logLevelType level, const char* format, ...) {
    int rank = 0;

    (void)rank;  // prevent a warning about unused variable
    FILE* stream = this->logStream;
    if (level == LOG_LEVEL_FATAL || level == LOG_LEVEL_ERROR) {
        numberOfErr++;
        stream = this->errStream;
    } else if (level == LOG_LEVEL_WARNING) {
        numberOfWrn++;
        stream = this->errStream;
    }

    if (1) {
        va_list args;
        if (level <= this->logLevel) {
            va_start(args, format);
            vfprintf(stream, format, args);
            va_end(args);
        }
    }
}

/**
 * \brief Writes a formatted "Exit" message with source location context.
 * \param _func Name of the function where the exit was triggered (may be null).
 * \param _file Source file path; trimmed to start at "OpenDFN_CUDA".
 * \param _line Line number within the source file.
 *
 * Side effects: writes the exit banner, function name, and file:line to the
 * log stream, framed by LOG_ERR_HEADER and LOG_ERR_TAIL.
 */
void Logger ::writeExitMsg(const char* _func, const char* _file, int _line) {
    char  filewithouPath[OpenDFN_ARGC_LENGTH];
    copyChars(filewithouPath, _file);
    removeCharsBeforeSubstring(filewithouPath, "OpenDFN_CUDA");

    FILE* stream = this->logStream;
    fprintf(stream, "%s\n%s     :", LOG_ERR_HEADER, "Exit");

    if (_func) {
        fprintf(stream, "at %s() ", _func);
    }

    if (filewithouPath) {
        fprintf(stream, "(%s:%d)", filewithouPath, _line);
    }

    fprintf(stream, "\n%s", LOG_ERR_TAIL);
}

/**
 * \brief Writes a detailed leveled message with source location context.
 * \param level  Severity of the message.
 * \param _func  Name of the originating function (may be null).
 * \param _file  Source file path; trimmed to start at "OpenDFN_CUDA".
 * \param _line  Line number within the source file.
 * \param format printf-style format string, followed by matching arguments.
 *
 * Side effects: routes FATAL/ERROR/WARNING output to the error stream (and
 * increments the error or warning counter), otherwise uses the log stream.
 * When \p level is at or below the configured log level, emits the level name,
 * the formatted message, and the function/file:line context, framed by
 * LOG_ERR_HEADER and LOG_ERR_TAIL.
 */
void Logger ::writeELogMsg(logLevelType level, const char* _func, const char* _file, int _line, const char* format, ...) {
    va_list args;
    char   filewithouPath[OpenDFN_ARGC_LENGTH];
    
    copyChars(filewithouPath, _file);
    removeCharsBeforeSubstring(filewithouPath, "OpenDFN_CUDA");

    FILE*   stream = this->logStream;
    if (level == LOG_LEVEL_FATAL || level == LOG_LEVEL_ERROR) {
        numberOfErr++;
        stream = this->errStream;
    } else if (level == LOG_LEVEL_WARNING) {
        numberOfWrn++;
        stream = this->errStream;
    }

    if (level <= this->logLevel) {
        if (_file) {
            fprintf(stream, "%s\n%s     :", LOG_ERR_HEADER, giveLevelName(level));
        } else {
            fprintf(stream, "%s\n%s     :", LOG_ERR_HEADER, giveLevelName(level));
        }

        va_start(args, format);
        vfprintf(stream, format, args);
        va_end(args);
        fprintf(stream, "\n");

        if (_func) {
            fprintf(stream, "In %s() ", _func);
        }

        if (filewithouPath) {
            fprintf(stream, "(%s:%d)", filewithouPath, _line);
        }

        fprintf(stream, "\n%s", LOG_ERR_TAIL);
    }
}

/**
 * \brief Writes a warning that a function has not yet been implemented.
 * \param _func Name of the unimplemented function (may be null).
 * \param _file Source file path where the stub resides (may be null).
 * \param _line Line number within the source file.
 *
 * Side effects: writes a "not implemented yet" warning to the log stream,
 * framed by LOG_ERR_HEADER and LOG_ERR_TAIL.
 */
void Logger ::writeImplementLogMsg(const char* _func, const char* _file, int _line) {
    FILE* stream = this->logStream;

    fprintf(stream, "%s\n%s     :", LOG_ERR_HEADER, "Warning");

    if (_func) {
        fprintf(stream, "The \"%s\" function() ", _func);
    }

    if (_file) {
        fprintf(stream, "in (%s:%d) has not been implmented yet!", _file, _line);
    }

    fprintf(stream, "\n%s", LOG_ERR_TAIL);
}

/**
 * \brief Maps a log level to its human-readable name.
 * \param l The log level to name.
 * \return "Error" for ERROR, "Warning" for WARNING, otherwise "Info".
 */
const char* Logger ::giveLevelName(logLevelType l) const {
    switch (l) {
            // case LOG_LEVEL_FATAL:
        case LOG_LEVEL_ERROR: return "Error";

        case LOG_LEVEL_WARNING: return "Warning";

        default: return "Info";
    }
}

/**
 * \brief Sets the current verbosity level of the logger.
 * \param level New log level as an integer.
 *
 * Side effects: updates the log level only if \p level is within the valid
 * range [LOG_LEVEL_FATAL, LOG_LEVEL_DEBUG]; out-of-range values are ignored.
 */
void Logger ::setLogLevel(int level) {
    if ((level >= (int)LOG_LEVEL_FATAL) && (level <= (int)LOG_LEVEL_DEBUG)) {
        this->logLevel = (logLevelType)level;
    }
}

/**
 * \brief Prints a summary of the errors and warnings reported so far.
 *
 * Side effects: writes the total error and warning counts to the log stream.
 */
void Logger ::printStatistics() {
    int rank             = 0;
    int totalNumberOfErr = numberOfErr, totalNumberOfWrn = numberOfWrn;
    if (rank == 0) {
        // force output
        fprintf(logStream, "Total %d error(s) and %d warning(s) reported\n", totalNumberOfErr, totalNumberOfWrn);
    }
}
}  // namespace ns_common