
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
 * @file column.hpp
 * @brief Declares the Column container and defines the ColumnFormat setters
 *        that broadcast styling to every cell in a column.
 */
#pragma once
#include <algorithm>
#include <functional>
#include <iostream>
#include <memory>

#if __cplusplus >= 201703L
#include <optional>
using std::optional;
#else
#include <tabulate/optional_lite.hpp>
using nonstd::optional;
#endif

#include <string>
#include <tabulate/cell.hpp>
#include <tabulate/column_format.hpp>
#include <vector>
#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif

namespace tabulate {

/**
 * @brief Represents a single column of a table as an ordered collection of
 *        cell references.
 *
 * A Column does not own its cells; it holds reference wrappers to Cell objects
 * owned elsewhere (by the table rows). It provides iteration, indexed access,
 * width computation for alignment, and a format() entry point for column-wide
 * styling.
 */
class Column {
public:
  /**
   * @brief Constructs an empty column belonging to a parent table.
   * @param parent Owning TableInternal; stored as a weak reference.
   */
  explicit Column(std::shared_ptr<class TableInternal> parent) : parent_(parent) {}

  /**
   * @brief Appends a cell reference to the end of the column.
   * @param cell Cell to reference. Mutates the internal cell list.
   */
  void add_cell(Cell &cell) { cells_.push_back(cell); }

  /**
   * @brief Returns the cell at the given row index.
   * @param index Zero-based row index (not bounds-checked).
   * @return Reference to the cell at @p index.
   */
  Cell &operator[](size_t index) { return cells_[index]; }

  /**
   * @brief Returns a copy of the column's cell reference list.
   * @return Vector of cell reference wrappers.
   */
  std::vector<std::reference_wrapper<Cell>> cells() const { return cells_; }

  /**
   * @brief Returns the number of cells (rows) in the column.
   * @return Cell count.
   */
  size_t size() const { return cells_.size(); }

  /**
   * @brief Returns a formatter that applies styling to all cells in this column.
   * @return A ColumnFormat bound to this column.
   */
  ColumnFormat format() { return ColumnFormat(*this); }

  /**
   * @brief Forward iterator over the cells of a column, dereferencing to Cell&.
   */
  class CellIterator {
  public:
    /**
     * @brief Wraps an underlying vector iterator.
     * @param ptr Iterator into the column's cell reference vector.
     */
    explicit CellIterator(std::vector<std::reference_wrapper<Cell>>::iterator ptr) : ptr(ptr) {}

    /** @brief Advances to the next cell. @return This iterator after increment. */
    CellIterator operator++() {
      ++ptr;
      return *this;
    }
    /** @brief Inequality comparison against another iterator.
     *  @param other Iterator to compare with. @return True if positions differ. */
    bool operator!=(const CellIterator &other) const { return ptr != other.ptr; }
    /** @brief Dereferences to the current cell. @return Reference to the current Cell. */
    Cell &operator*() { return *ptr; }

  private:
    std::vector<std::reference_wrapper<Cell>>::iterator ptr; ///< Current position.
  };

  /** @brief Returns an iterator to the first cell. @return Begin iterator. */
  auto begin() -> CellIterator { return CellIterator(cells_.begin()); }
  /** @brief Returns an iterator past the last cell. @return End iterator. */
  auto end() -> CellIterator { return CellIterator(cells_.end()); }

private:
  friend class ColumnFormat;
  friend class Printer;

  // Returns the column width as configured
  // For each cell in the column, check the cell.format.width
  // property and return the largest configured column width
  // This is used to ensure that all cells in a column are
  // aligned when printing the column
  size_t get_configured_width() {
    size_t result{0};
    for (size_t i = 0; i < size(); ++i) {
      auto cell = cells_[i];
      auto format = cell.get().format();
      if (format.width_.has_value())
        result = std::max(result, *format.width_);
    }
    return result;
  }

  // Computes the width of the column based on cell contents
  // and configured cell padding
  // For each cell, compute padding_left + cell_contents + padding_right
  // and return the largest value
  //
  // This is useful when no cell.format.width is configured
  // Call get_configured_width()
  // - If this returns 0, then use get_computed_width()
  size_t get_computed_width() {
    size_t result{0};
    for (size_t i = 0; i < size(); ++i) {
      result = std::max(result, get_cell_width(i));
    }
    return result;
  }

  // Returns padding_left + cell_contents.size() + padding_right
  // for a given cell in the column
  size_t get_cell_width(size_t cell_index) {
    size_t result{0};
    Cell &cell = cells_[cell_index].get();
    auto format = cell.format();
    if (format.padding_left_.has_value())
      result += *format.padding_left_;

    // Check if input text has newlines
    auto text = cell.get_text();
    auto split_lines = Format::split_lines(text, "\n", cell.locale(),
                                           cell.is_multi_byte_character_support_enabled());

    // If there are no newlines in input, set column_width = text.size()
    if (split_lines.size() == 1) {
      result += cell.size();
    } else {
      // There are newlines in input
      // Find widest substring in input and use this as column_width
      size_t widest_sub_string_size{0};
      for (auto &line : split_lines)
        if (get_sequence_length(line, cell.locale(),
                                cell.is_multi_byte_character_support_enabled()) >
            widest_sub_string_size)
          widest_sub_string_size = get_sequence_length(
              line, cell.locale(), cell.is_multi_byte_character_support_enabled());
      result += widest_sub_string_size;
    }

    if (format.padding_right_.has_value())
      result += *format.padding_right_;

    return result;
  }

  std::vector<std::reference_wrapper<Cell>> cells_; ///< Non-owning references to the column's cells.
  std::weak_ptr<class TableInternal> parent_;       ///< Weak back-reference to the owning table.
};

// ---------------------------------------------------------------------------
// ColumnFormat setter definitions.
// Each setter iterates over every cell in the bound column and forwards the
// value to the corresponding per-cell Format setter, then returns *this to
// allow fluent chaining. Only @brief is documented here; parameter/return
// semantics are declared in column_format.hpp.
// ---------------------------------------------------------------------------

/** @brief Applies width to every cell in the column. @return Reference to this formatter. */
inline ColumnFormat &ColumnFormat::width(size_t value) {
  for (auto &cell : column_.get().cells_)
    cell.get().format().width(value);
  return *this;
}

inline ColumnFormat &ColumnFormat::height(size_t value) {
  for (auto &cell : column_.get().cells_)
    cell.get().format().height(value);
  return *this;
}

inline ColumnFormat &ColumnFormat::padding(size_t value) {
  for (auto &cell : column_.get().cells_)
    cell.get().format().padding(value);
  return *this;
}

inline ColumnFormat &ColumnFormat::padding_left(size_t value) {
  for (auto &cell : column_.get().cells_)
    cell.get().format().padding_left(value);
  return *this;
}

inline ColumnFormat &ColumnFormat::padding_right(size_t value) {
  for (auto &cell : column_.get().cells_)
    cell.get().format().padding_right(value);
  return *this;
}

inline ColumnFormat &ColumnFormat::padding_top(size_t value) {
  for (auto &cell : column_.get().cells_)
    cell.get().format().padding_top(value);
  return *this;
}

inline ColumnFormat &ColumnFormat::padding_bottom(size_t value) {
  for (auto &cell : column_.get().cells_)
    cell.get().format().padding_bottom(value);
  return *this;
}

inline ColumnFormat &ColumnFormat::border(const std::string &value) {
  for (auto &cell : column_.get().cells_)
    cell.get().format().border(value);
  return *this;
}

inline ColumnFormat &ColumnFormat::border_color(Color value) {
  for (auto &cell : column_.get().cells_)
    cell.get().format().border_color(value);
  return *this;
}

inline ColumnFormat &ColumnFormat::border_background_color(Color value) {
  for (auto &cell : column_.get().cells_)
    cell.get().format().border_background_color(value);
  return *this;
}

inline ColumnFormat &ColumnFormat::border_left(const std::string &value) {
  for (auto &cell : column_.get().cells_)
    cell.get().format().border_left(value);
  return *this;
}

inline ColumnFormat &ColumnFormat::border_left_color(Color value) {
  for (auto &cell : column_.get().cells_)
    cell.get().format().border_left_color(value);
  return *this;
}

inline ColumnFormat &ColumnFormat::border_left_background_color(Color value) {
  for (auto &cell : column_.get().cells_)
    cell.get().format().border_left_background_color(value);
  return *this;
}

inline ColumnFormat &ColumnFormat::border_right(const std::string &value) {
  for (auto &cell : column_.get().cells_)
    cell.get().format().border_right(value);
  return *this;
}

inline ColumnFormat &ColumnFormat::border_right_color(Color value) {
  for (auto &cell : column_.get().cells_)
    cell.get().format().border_right_color(value);
  return *this;
}

inline ColumnFormat &ColumnFormat::border_right_background_color(Color value) {
  for (auto &cell : column_.get().cells_)
    cell.get().format().border_right_background_color(value);
  return *this;
}

inline ColumnFormat &ColumnFormat::border_top(const std::string &value) {
  for (auto &cell : column_.get().cells_)
    cell.get().format().border_top(value);
  return *this;
}

inline ColumnFormat &ColumnFormat::border_top_color(Color value) {
  for (auto &cell : column_.get().cells_)
    cell.get().format().border_top_color(value);
  return *this;
}

inline ColumnFormat &ColumnFormat::border_top_background_color(Color value) {
  for (auto &cell : column_.get().cells_)
    cell.get().format().border_top_background_color(value);
  return *this;
}

inline ColumnFormat &ColumnFormat::border_bottom(const std::string &value) {
  for (auto &cell : column_.get().cells_)
    cell.get().format().border_bottom(value);
  return *this;
}

inline ColumnFormat &ColumnFormat::border_bottom_color(Color value) {
  for (auto &cell : column_.get().cells_)
    cell.get().format().border_bottom_color(value);
  return *this;
}

inline ColumnFormat &ColumnFormat::border_bottom_background_color(Color value) {
  for (auto &cell : column_.get().cells_)
    cell.get().format().border_bottom_background_color(value);
  return *this;
}

inline ColumnFormat &ColumnFormat::corner(const std::string &value) {
  for (auto &cell : column_.get().cells_)
    cell.get().format().corner(value);
  return *this;
}

inline ColumnFormat &ColumnFormat::corner_color(Color value) {
  for (auto &cell : column_.get().cells_)
    cell.get().format().corner_color(value);
  return *this;
}

inline ColumnFormat &ColumnFormat::corner_background_color(Color value) {
  for (auto &cell : column_.get().cells_)
    cell.get().format().corner_background_color(value);
  return *this;
}

inline ColumnFormat &ColumnFormat::column_separator(const std::string &value) {
  for (auto &cell : column_.get().cells_)
    cell.get().format().column_separator(value);
  return *this;
}

inline ColumnFormat &ColumnFormat::column_separator_color(Color value) {
  for (auto &cell : column_.get().cells_)
    cell.get().format().column_separator_color(value);
  return *this;
}

inline ColumnFormat &ColumnFormat::column_separator_background_color(Color value) {
  for (auto &cell : column_.get().cells_)
    cell.get().format().column_separator_background_color(value);
  return *this;
}

inline ColumnFormat &ColumnFormat::font_align(FontAlign value) {
  for (auto &cell : column_.get().cells_)
    cell.get().format().font_align(value);
  return *this;
}

inline ColumnFormat &ColumnFormat::font_style(const std::vector<FontStyle> &style) {
  for (auto &cell : column_.get().cells_)
    cell.get().format().font_style(style);
  return *this;
}

inline ColumnFormat &ColumnFormat::font_color(Color value) {
  for (auto &cell : column_.get().cells_)
    cell.get().format().font_color(value);
  return *this;
}

inline ColumnFormat &ColumnFormat::font_background_color(Color value) {
  for (auto &cell : column_.get().cells_)
    cell.get().format().font_background_color(value);
  return *this;
}

inline ColumnFormat &ColumnFormat::color(Color value) {
  for (auto &cell : column_.get().cells_)
    cell.get().format().color(value);
  return *this;
}

inline ColumnFormat &ColumnFormat::background_color(Color value) {
  for (auto &cell : column_.get().cells_)
    cell.get().format().background_color(value);
  return *this;
}

inline ColumnFormat &ColumnFormat::multi_byte_characters(bool value) {
  for (auto &cell : column_.get().cells_)
    cell.get().format().multi_byte_characters(value);
  return *this;
}

inline ColumnFormat &ColumnFormat::locale(const std::string &value) {
  for (auto &cell : column_.get().cells_)
    cell.get().format().locale(value);
  return *this;
}

} // namespace tabulate
