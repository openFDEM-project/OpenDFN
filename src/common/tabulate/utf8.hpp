
/*
  __        ___.         .__          __
_/  |______ \_ |__  __ __|  | _____ _/  |_  ____
\   __\__  \ | __ \|  |  \  | \__  \\   __\/ __ \
 |  |  / __ \| \_\ \  |  /  |__/ __ \|  | \  ___/
 |__| (____  /___  /____/|____(____  /__|  \___  >
           \/    \/                \/          \/
Table Maker for Modern C++
https://github.com/p-ranav/tabulate

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2019 Pranav Srinivas Kumar <pranav.srinivas.kumar@gmail.com>.

Permission is hereby  granted, free of charge, to any  person obtaining a copy
of this software and associated  documentation files (the "Software"), to deal
in the Software  without restriction, including without  limitation the rights
to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
/**
 * @file utf8.hpp
 * @brief UTF-8 aware display-width helpers for the third-party "tabulate"
 *        library. Provides utilities to compute the on-screen column width of
 *        a possibly multi-byte UTF-8 string, which tabulate uses to align
 *        cell contents correctly for wide characters.
 *
 * @note Third-party vendored code (https://github.com/p-ranav/tabulate,
 *       MIT License). Documentation comments only; no code was modified.
 */
#pragma once
#include <algorithm>
#include <cstdint>
#include <string>

#include <clocale>
#include <locale>

#include <cstdlib>
#include <tabulate/termcolor.hpp>
#include <wchar.h>

namespace tabulate {

/**
 * @brief Compute the terminal display width of a UTF-8 string on POSIX systems.
 *
 * Converts the narrow input string to a wide-character string under the given
 * locale and uses wcswidth() to measure how many terminal columns it occupies
 * (wide/CJK characters count as more than one column).
 *
 * @param string The UTF-8 encoded text to measure.
 * @param locale Locale name used to interpret multi-byte characters; it is set
 *               as the global locale for the duration of the call.
 * @param max_column_width Maximum number of columns to consider (passed to
 *               wcswidth()).
 * @return The display width in columns, or a negative value if the string
 *         contains non-printable characters (per wcswidth() semantics).
 *
 * @note Side effects: temporarily changes the global std::locale (restored
 *       before returning) and allocates/frees a temporary wide-string buffer.
 *       Only compiled on Unix/macOS platforms.
 */
#if defined(__unix__) || defined(__unix) || defined(__APPLE__)
inline int get_wcswidth(const std::string &string, const std::string &locale,
                        size_t max_column_width) {
  if (string.size() == 0)
    return 0;

  // The behavior of wcswidth() depends on the LC_CTYPE category of the current locale.
  // Set the current locale based on cell properties before computing width
  auto old_locale = std::locale::global(std::locale(locale));

  // Convert from narrow std::string to wide string
  wchar_t *wide_string = new wchar_t[string.size()];
  std::mbstowcs(wide_string, string.c_str(), string.size());

  // Compute display width of wide string
  int result = wcswidth(wide_string, max_column_width);
  delete[] wide_string;

  // Restore old locale
  std::locale::global(old_locale);

  return result;
}
#endif

/**
 * @brief Compute the display length (in terminal columns) of a text string.
 *
 * When multi-byte support is disabled, returns the raw byte length. When
 * enabled, it accounts for UTF-8 continuation bytes so that a multi-byte code
 * point is not over-counted. On Windows it approximates the width by
 * subtracting the number of UTF-8 continuation bytes; on Unix/macOS it defers
 * to get_wcswidth() and falls back to the byte-based estimate when wcswidth()
 * reports a negative (non-printable) result.
 *
 * @param text The UTF-8 encoded text to measure.
 * @param locale Locale name used for width computation on Unix/macOS; unused
 *               on Windows.
 * @param is_multi_byte_character_support_enabled If false, the raw byte length
 *               is returned without any UTF-8 handling.
 * @return The estimated number of terminal columns occupied by @p text.
 */
inline size_t get_sequence_length(const std::string &text, const std::string &locale,
                                  bool is_multi_byte_character_support_enabled) {
  if (!is_multi_byte_character_support_enabled)
    return text.length();

#if defined(_WIN32) || defined(_WIN64)
  (void) locale; // unused parameter
  return (text.length() - std::count_if(text.begin(), text.end(),
                                        [](char c) -> bool { return (c & 0xC0) == 0x80; }));
#elif defined(__unix__) || defined(__unix) || defined(__APPLE__)
  auto result = get_wcswidth(text, locale, text.size());
  if (result >= 0)
    return result;
  else
    return (text.length() - std::count_if(text.begin(), text.end(),
                                          [](char c) -> bool { return (c & 0xC0) == 0x80; }));
#endif
}

} // namespace tabulate
