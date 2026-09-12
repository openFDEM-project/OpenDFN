
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
 * @file exporter.hpp
 * @brief Abstract base interface for serializing a tabulate Table into a
 *        formatted string representation (third-party tabulate library).
 */
#pragma once
#include <string>
#include <tabulate/table.hpp>

namespace tabulate {

/**
 * @brief Abstract interface for table exporters.
 *
 * An Exporter converts a Table into a textual representation using a specific
 * output format (for example Markdown, LaTeX, or AsciiDoc). Concrete
 * subclasses implement dump() to produce their particular format. This class
 * is a pure interface and holds no state of its own.
 */
class Exporter {
public:
  /**
   * @brief Serialize the given table into a formatted string.
   * @param table The table to export; passed by reference and may be inspected
   *              (and, depending on the concrete implementation, have its
   *              rendering state queried) during export.
   * @return A string containing the table rendered in the exporter's format.
   */
  virtual std::string dump(Table &table) = 0;

  /**
   * @brief Virtual destructor enabling safe polymorphic destruction of
   *        derived exporters through an Exporter pointer.
   */
  virtual ~Exporter() {}
};

} // namespace tabulate
