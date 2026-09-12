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
#pragma once
#include <iostream>
#include <memory>
#include <string>
#include <tabulate/format.hpp>
#include <tabulate/utf8.hpp>

#if __cplusplus >= 201703L
#    include <optional>
using std::optional;
#else
#    include <tabulate/optional_lite.hpp>
using nonstd::optional;
#endif

#include <vector>

namespace tabulate {
/**
 * @brief A single table cell holding text and a reference to its owning row.
 *
 * Part of the third-party p-ranav/tabulate library. Stores the cell's string
 * content and exposes text access, display width measurement, locale lookup,
 * and formatting configuration.
 */
class Cell {
public:
    /**
     * @brief Construct a cell that belongs to the given row.
     * @param parent Shared pointer to the owning Row; stored as a weak reference.
     */
    explicit Cell(std::shared_ptr<class Row> parent) : parent_(parent) {}

    /**
     * @brief Set the cell's text content.
     * @param text New text to store in the cell.
     * @note Side effect: overwrites the cell's internal data_ string.
     */
    void set_text(const std::string& text) {
        data_ = text;
    }

    /**
     * @brief Get the cell's current text content.
     * @return Const reference to the stored text.
     */
    const std::string& get_text() {
        return data_;
    }

    /**
     * @brief Compute the displayed length of the cell text.
     * @return Number of display columns, accounting for the cell locale and
     *         whether multi-byte character support is enabled.
     */
    size_t size() {
        return get_sequence_length(data_, locale(), is_multi_byte_character_support_enabled());
    }

    /**
     * @brief Get the locale string used when measuring text width.
     * @return The locale name resolved from this cell's format.
     */
    std::string locale() {
        return *format().locale_;
    }

    /**
     * @brief Access the cell's formatting configuration.
     * @return Reference to the effective Format, resolving inherited defaults.
     * @note Side effect: may lazily initialize the cell's format state.
     */
    Format& format();

    /**
     * @brief Query whether multi-byte character width support is active.
     * @return true if multi-byte (e.g. UTF-8/wide) width handling is enabled.
     */
    bool is_multi_byte_character_support_enabled();

private:
    std::string              data_;
    std::weak_ptr<class Row> parent_;
    optional<Format>         format_;
};
}  // namespace tabulate