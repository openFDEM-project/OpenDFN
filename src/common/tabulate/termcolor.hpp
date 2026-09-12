//!
//! termcolor
//! ~~~~~~~~~
//!
//! termcolor is a header-only c++ library for printing colored messages
//! to the terminal. Written just for fun with a help of the Force.
//!
//! :copyright: (c) 2013 by Ihor Kalnytskyi
//! :license: BSD, see LICENSE for details
//!

/**
 * @file termcolor.hpp
 * @brief Header-only ANSI/Windows terminal color library. Provides a set of
 *        std::ostream manipulators (e.g. red, on_blue, bold, reset) that emit
 *        ANSI escape sequences on POSIX terminals or call the Win32 console
 *        API on Windows, so that text streamed to a colorized stream is
 *        rendered with the requested style.
 *
 * @note Third-party vendored code (termcolor by Ihor Kalnytskyi, BSD License).
 *       Documentation comments only; no code was modified.
 */
#ifndef TERMCOLOR_HPP_
#define TERMCOLOR_HPP_

// the following snippet of code detects the current OS and
// defines the appropriate macro that is used to wrap some
// platform specific things
#if defined(_WIN32) || defined(_WIN64)
#define TERMCOLOR_OS_WINDOWS
#elif defined(__APPLE__)
#define TERMCOLOR_OS_MACOS
#elif defined(__unix__) || defined(__unix)
#define TERMCOLOR_OS_LINUX
#else
#error unsupported platform
#endif

// This headers provides the `isatty()`/`fileno()` functions,
// which are used for testing whether a standart stream refers
// to the terminal. As for Windows, we also need WinApi funcs
// for changing colors attributes of the terminal.
#if defined(TERMCOLOR_OS_MACOS) || defined(TERMCOLOR_OS_LINUX)
#include <unistd.h>
#elif defined(TERMCOLOR_OS_WINDOWS)
#include <io.h>
#include <windows.h>
#endif

#include <cstdio>
#include <iostream>

namespace termcolor {
// Forward declaration of the `_internal` namespace.
// All comments are below.
namespace _internal {
// An index to be used to access a private storage of I/O streams. See
// colorize / nocolorize I/O manipulators for details.
static int colorize_index = std::ios_base::xalloc();

inline FILE *get_standard_stream(const std::ostream &stream);
inline bool is_colorized(std::ostream &stream);
inline bool is_atty(const std::ostream &stream);

#if defined(TERMCOLOR_OS_WINDOWS)
inline void win_change_attributes(std::ostream &stream, int foreground, int background = -1);
#endif
} // namespace _internal

/**
 * @brief Stream manipulator that force-enables colorization for @p stream.
 * @param stream The output stream to mark as colorized.
 * @return Reference to @p stream (side effect: sets the stream's colorize flag).
 */
inline std::ostream &colorize(std::ostream &stream) {
  stream.iword(_internal::colorize_index) = 1L;
  return stream;
}

/**
 * @brief Stream manipulator that force-disables colorization for @p stream.
 * @param stream The output stream to mark as non-colorized.
 * @return Reference to @p stream (side effect: clears the stream's colorize flag).
 */
inline std::ostream &nocolorize(std::ostream &stream) {
  stream.iword(_internal::colorize_index) = 0L;
  return stream;
}

/**
 * @brief Reset all color/style attributes back to the terminal default.
 * @param stream The output stream to reset.
 * @return Reference to @p stream. Emits an ANSI reset sequence on POSIX or
 *         restores default console attributes on Windows, only when the stream
 *         is colorized.
 */
inline std::ostream &reset(std::ostream &stream) {
  if (_internal::is_colorized(stream)) {
#if defined(TERMCOLOR_OS_MACOS) || defined(TERMCOLOR_OS_LINUX)
    stream << "\033[00m";
#elif defined(TERMCOLOR_OS_WINDOWS)
    _internal::win_change_attributes(stream, -1, -1);
#endif
  }
  return stream;
}

/**
 * @brief Text-style manipulators. Each emits the corresponding ANSI SGR escape
 *        sequence when @p stream is colorized (these styles are no-ops on
 *        Windows, which lacks direct ANSI style support). All return the same
 *        stream to allow chaining: bold (SGR 1), dark/faint (2), italic (3),
 *        underline (4), blink (5), reverse (7), concealed (8), crossed-out (9).
 * @param stream The output stream to style.
 * @return Reference to @p stream.
 */
inline std::ostream &bold(std::ostream &stream) {
  if (_internal::is_colorized(stream)) {
#if defined(TERMCOLOR_OS_MACOS) || defined(TERMCOLOR_OS_LINUX)
    stream << "\033[1m";
#elif defined(TERMCOLOR_OS_WINDOWS)
#endif
  }
  return stream;
}

inline std::ostream &dark(std::ostream &stream) {
  if (_internal::is_colorized(stream)) {
#if defined(TERMCOLOR_OS_MACOS) || defined(TERMCOLOR_OS_LINUX)
    stream << "\033[2m";
#elif defined(TERMCOLOR_OS_WINDOWS)
#endif
  }
  return stream;
}

inline std::ostream &italic(std::ostream &stream) {
  if (_internal::is_colorized(stream)) {
#if defined(TERMCOLOR_OS_MACOS) || defined(TERMCOLOR_OS_LINUX)
    stream << "\033[3m";
#elif defined(TERMCOLOR_OS_WINDOWS)
#endif
  }
  return stream;
}

inline std::ostream &underline(std::ostream &stream) {
  if (_internal::is_colorized(stream)) {
#if defined(TERMCOLOR_OS_MACOS) || defined(TERMCOLOR_OS_LINUX)
    stream << "\033[4m";
#elif defined(TERMCOLOR_OS_WINDOWS)
#endif
  }
  return stream;
}

inline std::ostream &blink(std::ostream &stream) {
  if (_internal::is_colorized(stream)) {
#if defined(TERMCOLOR_OS_MACOS) || defined(TERMCOLOR_OS_LINUX)
    stream << "\033[5m";
#elif defined(TERMCOLOR_OS_WINDOWS)
#endif
  }
  return stream;
}

inline std::ostream &reverse(std::ostream &stream) {
  if (_internal::is_colorized(stream)) {
#if defined(TERMCOLOR_OS_MACOS) || defined(TERMCOLOR_OS_LINUX)
    stream << "\033[7m";
#elif defined(TERMCOLOR_OS_WINDOWS)
#endif
  }
  return stream;
}

inline std::ostream &concealed(std::ostream &stream) {
  if (_internal::is_colorized(stream)) {
#if defined(TERMCOLOR_OS_MACOS) || defined(TERMCOLOR_OS_LINUX)
    stream << "\033[8m";
#elif defined(TERMCOLOR_OS_WINDOWS)
#endif
  }
  return stream;
}

inline std::ostream &crossed(std::ostream &stream) {
  if (_internal::is_colorized(stream)) {
#if defined(TERMCOLOR_OS_MACOS) || defined(TERMCOLOR_OS_LINUX)
    stream << "\033[9m";
#elif defined(TERMCOLOR_OS_WINDOWS)
#endif
  }
  return stream;
}

/**
 * @brief Foreground-color manipulators: grey, red, green, yellow, blue,
 *        magenta, cyan, white. When @p stream is colorized, each emits the
 *        matching ANSI foreground SGR code (30-37) on POSIX or sets the
 *        corresponding Win32 console foreground attribute on Windows.
 * @param stream The output stream to color.
 * @return Reference to @p stream.
 */
inline std::ostream &grey(std::ostream &stream) {
  if (_internal::is_colorized(stream)) {
#if defined(TERMCOLOR_OS_MACOS) || defined(TERMCOLOR_OS_LINUX)
    stream << "\033[30m";
#elif defined(TERMCOLOR_OS_WINDOWS)
    _internal::win_change_attributes(stream,
                                     0 // grey (black)
    );
#endif
  }
  return stream;
}

inline std::ostream &red(std::ostream &stream) {
  if (_internal::is_colorized(stream)) {
#if defined(TERMCOLOR_OS_MACOS) || defined(TERMCOLOR_OS_LINUX)
    stream << "\033[31m";
#elif defined(TERMCOLOR_OS_WINDOWS)
    _internal::win_change_attributes(stream, FOREGROUND_RED);
#endif
  }
  return stream;
}

inline std::ostream &green(std::ostream &stream) {
  if (_internal::is_colorized(stream)) {
#if defined(TERMCOLOR_OS_MACOS) || defined(TERMCOLOR_OS_LINUX)
    stream << "\033[32m";
#elif defined(TERMCOLOR_OS_WINDOWS)
    _internal::win_change_attributes(stream, FOREGROUND_GREEN);
#endif
  }
  return stream;
}

inline std::ostream &yellow(std::ostream &stream) {
  if (_internal::is_colorized(stream)) {
#if defined(TERMCOLOR_OS_MACOS) || defined(TERMCOLOR_OS_LINUX)
    stream << "\033[33m";
#elif defined(TERMCOLOR_OS_WINDOWS)
    _internal::win_change_attributes(stream, FOREGROUND_GREEN | FOREGROUND_RED);
#endif
  }
  return stream;
}

inline std::ostream &blue(std::ostream &stream) {
  if (_internal::is_colorized(stream)) {
#if defined(TERMCOLOR_OS_MACOS) || defined(TERMCOLOR_OS_LINUX)
    stream << "\033[34m";
#elif defined(TERMCOLOR_OS_WINDOWS)
    _internal::win_change_attributes(stream, FOREGROUND_BLUE);
#endif
  }
  return stream;
}

inline std::ostream &magenta(std::ostream &stream) {
  if (_internal::is_colorized(stream)) {
#if defined(TERMCOLOR_OS_MACOS) || defined(TERMCOLOR_OS_LINUX)
    stream << "\033[35m";
#elif defined(TERMCOLOR_OS_WINDOWS)
    _internal::win_change_attributes(stream, FOREGROUND_BLUE | FOREGROUND_RED);
#endif
  }
  return stream;
}

inline std::ostream &cyan(std::ostream &stream) {
  if (_internal::is_colorized(stream)) {
#if defined(TERMCOLOR_OS_MACOS) || defined(TERMCOLOR_OS_LINUX)
    stream << "\033[36m";
#elif defined(TERMCOLOR_OS_WINDOWS)
    _internal::win_change_attributes(stream, FOREGROUND_BLUE | FOREGROUND_GREEN);
#endif
  }
  return stream;
}

inline std::ostream &white(std::ostream &stream) {
  if (_internal::is_colorized(stream)) {
#if defined(TERMCOLOR_OS_MACOS) || defined(TERMCOLOR_OS_LINUX)
    stream << "\033[37m";
#elif defined(TERMCOLOR_OS_WINDOWS)
    _internal::win_change_attributes(stream, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
#endif
  }
  return stream;
}

/**
 * @brief Background-color manipulators: on_grey, on_red, on_green, on_yellow,
 *        on_blue, on_magenta, on_cyan, on_white. When @p stream is colorized,
 *        each emits the matching ANSI background SGR code (40-47) on POSIX or
 *        sets the corresponding Win32 console background attribute on Windows.
 * @param stream The output stream to color.
 * @return Reference to @p stream.
 */
inline std::ostream &on_grey(std::ostream &stream) {
  if (_internal::is_colorized(stream)) {
#if defined(TERMCOLOR_OS_MACOS) || defined(TERMCOLOR_OS_LINUX)
    stream << "\033[40m";
#elif defined(TERMCOLOR_OS_WINDOWS)
    _internal::win_change_attributes(stream, -1,
                                     0 // grey (black)
    );
#endif
  }
  return stream;
}

inline std::ostream &on_red(std::ostream &stream) {
  if (_internal::is_colorized(stream)) {
#if defined(TERMCOLOR_OS_MACOS) || defined(TERMCOLOR_OS_LINUX)
    stream << "\033[41m";
#elif defined(TERMCOLOR_OS_WINDOWS)
    _internal::win_change_attributes(stream, -1, BACKGROUND_RED);
#endif
  }
  return stream;
}

inline std::ostream &on_green(std::ostream &stream) {
  if (_internal::is_colorized(stream)) {
#if defined(TERMCOLOR_OS_MACOS) || defined(TERMCOLOR_OS_LINUX)
    stream << "\033[42m";
#elif defined(TERMCOLOR_OS_WINDOWS)
    _internal::win_change_attributes(stream, -1, BACKGROUND_GREEN);
#endif
  }
  return stream;
}

inline std::ostream &on_yellow(std::ostream &stream) {
  if (_internal::is_colorized(stream)) {
#if defined(TERMCOLOR_OS_MACOS) || defined(TERMCOLOR_OS_LINUX)
    stream << "\033[43m";
#elif defined(TERMCOLOR_OS_WINDOWS)
    _internal::win_change_attributes(stream, -1, BACKGROUND_GREEN | BACKGROUND_RED);
#endif
  }
  return stream;
}

inline std::ostream &on_blue(std::ostream &stream) {
  if (_internal::is_colorized(stream)) {
#if defined(TERMCOLOR_OS_MACOS) || defined(TERMCOLOR_OS_LINUX)
    stream << "\033[44m";
#elif defined(TERMCOLOR_OS_WINDOWS)
    _internal::win_change_attributes(stream, -1, BACKGROUND_BLUE);
#endif
  }
  return stream;
}

inline std::ostream &on_magenta(std::ostream &stream) {
  if (_internal::is_colorized(stream)) {
#if defined(TERMCOLOR_OS_MACOS) || defined(TERMCOLOR_OS_LINUX)
    stream << "\033[45m";
#elif defined(TERMCOLOR_OS_WINDOWS)
    _internal::win_change_attributes(stream, -1, BACKGROUND_BLUE | BACKGROUND_RED);
#endif
  }
  return stream;
}

inline std::ostream &on_cyan(std::ostream &stream) {
  if (_internal::is_colorized(stream)) {
#if defined(TERMCOLOR_OS_MACOS) || defined(TERMCOLOR_OS_LINUX)
    stream << "\033[46m";
#elif defined(TERMCOLOR_OS_WINDOWS)
    _internal::win_change_attributes(stream, -1, BACKGROUND_GREEN | BACKGROUND_BLUE);
#endif
  }
  return stream;
}

inline std::ostream &on_white(std::ostream &stream) {
  if (_internal::is_colorized(stream)) {
#if defined(TERMCOLOR_OS_MACOS) || defined(TERMCOLOR_OS_LINUX)
    stream << "\033[47m";
#elif defined(TERMCOLOR_OS_WINDOWS)
    _internal::win_change_attributes(stream, -1,
                                     BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_RED);
#endif
  }

  return stream;
}

//! Since C++ hasn't a way to hide something in the header from
//! the outer access, I have to introduce this namespace which
//! is used for internal purpose and should't be access from
//! the user code.
namespace _internal {
//! Since C++ hasn't a true way to extract stream handler
//! from the a given `std::ostream` object, I have to write
//! this kind of hack.
/**
 * @brief Map a std::ostream to its underlying C FILE* standard stream.
 * @param stream The stream to inspect.
 * @return stdout for std::cout, stderr for std::cerr/std::clog, or nullptr for
 *         any other stream (which cannot be resolved to a standard handle).
 */
inline FILE *get_standard_stream(const std::ostream &stream) {
  if (&stream == &std::cout)
    return stdout;
  else if ((&stream == &std::cerr) || (&stream == &std::clog))
    return stderr;

  return 0;
}

// Say whether a given stream should be colorized or not. It's always
// true for ATTY streams and may be true for streams marked with
// colorize flag.
/**
 * @brief Decide whether color output should be produced for @p stream.
 * @param stream The stream to test.
 * @return true if the stream is a terminal (atty) or has been explicitly
 *         marked with the colorize flag; false otherwise.
 */
inline bool is_colorized(std::ostream &stream) {
  return is_atty(stream) || static_cast<bool>(stream.iword(colorize_index));
}

//! Test whether a given `std::ostream` object refers to
//! a terminal.
/**
 * @brief Test whether the stream is attached to an interactive terminal.
 * @param stream The stream to test.
 * @return true if the resolved standard stream refers to a tty; false if the
 *         stream is not a standard stream or is redirected to a non-terminal.
 */
inline bool is_atty(const std::ostream &stream) {
  FILE *std_stream = get_standard_stream(stream);

  // Unfortunately, fileno() ends with segmentation fault
  // if invalid file descriptor is passed. So we need to
  // handle this case gracefully and assume it's not a tty
  // if standard stream is not detected, and 0 is returned.
  if (!std_stream)
    return false;

#if defined(TERMCOLOR_OS_MACOS) || defined(TERMCOLOR_OS_LINUX)
  return ::isatty(fileno(std_stream));
#elif defined(TERMCOLOR_OS_WINDOWS)
  return ::_isatty(_fileno(std_stream));
#endif
}

#if defined(TERMCOLOR_OS_WINDOWS)
//! Change Windows Terminal colors attribute. If some
//! parameter is `-1` then attribute won't changed.
/**
 * @brief Change the Windows console text attributes for @p stream.
 *
 * Windows lacks ANSI escape sequences, so color changes go through the Win32
 * console API. On first use the current console attributes are cached as the
 * default so they can later be restored.
 *
 * @param stream The output stream (must be std::cout or std::cerr and a tty).
 * @param foreground Win32 foreground attribute flags, or -1 to leave unchanged.
 *                   If both foreground and background are -1, the saved default
 *                   attributes are restored.
 * @param background Win32 background attribute flags, or -1 to leave unchanged.
 * @return void. Side effects: mutates the console's text attributes and a
 *         function-local static cache of the default attributes; no-op if the
 *         stream is not a terminal.
 */
inline void win_change_attributes(std::ostream &stream, int foreground, int background) {
  // yeah, i know.. it's ugly, it's windows.
  static WORD defaultAttributes = 0;

  // Windows doesn't have ANSI escape sequences and so we use special
  // API to change Terminal output color. That means we can't
  // manipulate colors by means of "std::stringstream" and hence
  // should do nothing in this case.
  if (!_internal::is_atty(stream))
    return;

  // get terminal handle
  HANDLE hTerminal = INVALID_HANDLE_VALUE;
  if (&stream == &std::cout)
    hTerminal = GetStdHandle(STD_OUTPUT_HANDLE);
  else if (&stream == &std::cerr)
    hTerminal = GetStdHandle(STD_ERROR_HANDLE);

  // save default terminal attributes if it unsaved
  if (!defaultAttributes) {
    CONSOLE_SCREEN_BUFFER_INFO info;
    if (!GetConsoleScreenBufferInfo(hTerminal, &info))
      return;
    defaultAttributes = info.wAttributes;
  }

  // restore all default settings
  if (foreground == -1 && background == -1) {
    SetConsoleTextAttribute(hTerminal, defaultAttributes);
    return;
  }

  // get current settings
  CONSOLE_SCREEN_BUFFER_INFO info;
  if (!GetConsoleScreenBufferInfo(hTerminal, &info))
    return;

  if (foreground != -1) {
    info.wAttributes &= ~(info.wAttributes & 0x0F);
    info.wAttributes |= static_cast<WORD>(foreground);
  }

  if (background != -1) {
    info.wAttributes &= ~(info.wAttributes & 0xF0);
    info.wAttributes |= static_cast<WORD>(background);
  }

  SetConsoleTextAttribute(hTerminal, info.wAttributes);
}
#endif // TERMCOLOR_OS_WINDOWS

} // namespace _internal

} // namespace termcolor

#undef TERMCOLOR_OS_WINDOWS
#undef TERMCOLOR_OS_MACOS
#undef TERMCOLOR_OS_LINUX

#endif // TERMCOLOR_HPP_
