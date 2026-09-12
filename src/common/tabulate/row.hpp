
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
#include <tabulate/cell.hpp>

#if __cplusplus >= 201703L
#include <optional>
using std::optional;
#else
#include <tabulate/optional_lite.hpp>
using nonstd::optional;
#endif

#include <vector>
#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif

namespace tabulate {

/**
 * @file row.hpp
 * @brief Declares tabulate::Row, an ordered collection of Cell objects forming
 *        one horizontal row of a table, plus a forward iterator over its cells.
 */

/**
 * @brief A single row of a table, owning an ordered list of shared Cell pointers.
 *
 * A Row keeps a weak reference to its parent TableInternal and exposes indexed
 * access, iteration, and row-level formatting. Height computation helpers (used
 * by Printer) are private and account for cell padding and word wrapping.
 */
class Row {
public:
  /**
   * @brief Constructs a row bound to a parent table.
   * @param parent Shared pointer to the owning TableInternal; stored as a weak reference.
   */
  explicit Row(std::shared_ptr<class TableInternal> parent) : parent_(parent) {}

  /**
   * @brief Appends a cell to the end of the row.
   * @param cell Shared pointer to the Cell to append.
   */
  void add_cell(std::shared_ptr<Cell> cell) { cells_.push_back(cell); }

  /**
   * @brief Accesses a cell by index (subscript operator).
   * @param index Zero-based cell index.
   * @return Reference to the Cell at the given index.
   */
  Cell &operator[](size_t index) { return cell(index); }

  /**
   * @brief Accesses a cell by index.
   * @param index Zero-based cell index.
   * @return Reference to the Cell at the given index.
   */
  Cell &cell(size_t index) { return *(cells_[index]); }

  /**
   * @brief Returns a copy of the row's cell pointers.
   * @return Vector of shared pointers to the cells in this row.
   */
  std::vector<std::shared_ptr<Cell>> cells() const { return cells_; }

  /**
   * @brief Returns the number of cells in the row.
   * @return Cell count.
   */
  size_t size() const { return cells_.size(); }

  /**
   * @brief Returns the row-level format object.
   * @return Reference to the row's Format.
   */
  Format &format();

  /**
   * @brief Forward iterator that dereferences to Cell references over a row's cells.
   */
  class CellIterator {
  public:
    /**
     * @brief Constructs an iterator wrapping an underlying vector iterator.
     * @param ptr Iterator into the row's cell pointer vector.
     */
    explicit CellIterator(std::vector<std::shared_ptr<Cell>>::iterator ptr) : ptr(ptr) {}

    /**
     * @brief Advances to the next cell (pre-increment semantics).
     * @return Iterator after advancing.
     */
    CellIterator operator++() {
      ++ptr;
      return *this;
    }
    /**
     * @brief Inequality comparison against another iterator.
     * @param other Iterator to compare against.
     * @return True if the two iterators refer to different positions.
     */
    bool operator!=(const CellIterator &other) const { return ptr != other.ptr; }
    /**
     * @brief Dereferences the iterator.
     * @return Reference to the currently pointed-to Cell.
     */
    Cell &operator*() { return **ptr; }

  private:
    std::vector<std::shared_ptr<Cell>>::iterator ptr;
  };

  /**
   * @brief Returns an iterator to the first cell.
   * @return CellIterator positioned at the beginning of the row.
   */
  auto begin() -> CellIterator { return CellIterator(cells_.begin()); }
  /**
   * @brief Returns a past-the-end iterator.
   * @return CellIterator positioned one past the last cell.
   */
  auto end() -> CellIterator { return CellIterator(cells_.end()); }

private:
  friend class Printer;

  /**
   * @brief Returns the largest explicitly configured cell height in the row.
   * @return Maximum of all cells' Format.height_ values, or 0 if none is set.
   */
  // Returns the row height as configured
  // For each cell in the row, check the cell.format.height
  // property and return the largest configured row height
  // This is used to ensure that all cells in a row are
  // aligned when printing the column
  size_t get_configured_height() {
    size_t result{0};
    for (size_t i = 0; i < size(); ++i) {
      auto cell = cells_[i];
      auto format = cell->format();
      if (format.height_.has_value())
        result = std::max(result, *format.height_);
    }
    return result;
  }

  /**
   * @brief Computes the row height from cell contents and padding when no
   *        explicit height is configured.
   * @param column_widths Width of each column, used to estimate wrapped line count.
   * @return Maximum computed height across all cells in the row.
   */
  // Computes the height of the row based on cell contents
  // and configured cell padding
  // For each cell, compute:
  //   padding_top + (cell_contents / column height) + padding_bottom
  // and return the largest value
  //
  // This is useful when no cell.format.height is configured
  // Call get_configured_height()
  // - If this returns 0, then use get_computed_height()
  size_t get_computed_height(const std::vector<size_t> &column_widths) {
    size_t result{0};
    for (size_t i = 0; i < size(); ++i) {
      result = std::max(result, get_cell_height(i, column_widths[i]));
    }
    return result;
  }

  /**
   * @brief Computes the rendered height of a single cell, accounting for
   *        padding and word wrapping (or embedded newlines) at a given width.
   * @param cell_index Index of the cell within the row.
   * @param column_width Available column width for the cell.
   * @return padding_top + wrapped-line count + padding_bottom.
   */
  // Returns padding_top + cell_contents / column_height + padding_bottom
  // for a given cell in the column
  // e.g.,
  // column width = 5
  // cell_contents = "I love tabulate" (size/length = 15)
  // padding top and padding bottom are 1
  // then, cell height = 1 + (15 / 5) + 1 = 1 + 3 + 1 = 5
  // The cell will look like this:
  //
  // .....
  // I lov
  // e tab
  // ulate
  // .....
  size_t get_cell_height(size_t cell_index, size_t column_width) {
    size_t result{0};
    Cell &cell = *(cells_[cell_index]);
    auto format = cell.format();
    auto text = cell.get_text();

    auto padding_left = *format.padding_left_;
    auto padding_right = *format.padding_right_;

    result += *format.padding_top_;

    if (column_width > (padding_left + padding_right)) {
      column_width -= (padding_left + padding_right);
    }

    // Check if input text has embedded newline characters
    auto newlines_in_text = std::count(text.begin(), text.end(), '\n');
    std::string word_wrapped_text;
    if (newlines_in_text == 0) {
      // No new lines in input
      // Apply automatic word wrapping and compute row height
      word_wrapped_text = Format::word_wrap(text, column_width, cell.locale(),
                                            cell.is_multi_byte_character_support_enabled());
    } else {
      // There are embedded '\n' characters
      // Respect these characters
      word_wrapped_text = text;
    }

    auto newlines_in_wrapped_text =
        std::count(word_wrapped_text.begin(), word_wrapped_text.end(), '\n');
    auto estimated_row_height = newlines_in_wrapped_text;

    if (!word_wrapped_text.empty() &&
        word_wrapped_text[word_wrapped_text.size() - 1] != '\n') // text doesn't end with a newline
      estimated_row_height += 1;

    result += estimated_row_height;

    result += *format.padding_bottom_;

    return result;
  }

  std::vector<std::shared_ptr<Cell>> cells_;
  std::weak_ptr<class TableInternal> parent_;
  optional<Format> format_;
};

} // namespace tabulate
