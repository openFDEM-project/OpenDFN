
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
#include <tabulate/table_internal.hpp>

#if __cplusplus >= 201703L
#include <string_view>
#include <variant>
using std::get_if;
using std::holds_alternative;
using std::variant;
using std::visit;
using std::string_view;
#else
#include <tabulate/string_view_lite.hpp>
#include <tabulate/variant_lite.hpp>
using nonstd::get_if;
using nonstd::holds_alternative;
using nonstd::variant;
using nonstd::visit;
using nonstd::string_view;
#endif

#include <utility>

namespace tabulate {

/**
 * @file table.hpp
 * @brief Public user-facing facade for the tabulate table renderer.
 *
 * Defines the Table class, the primary entry point applications use to build
 * and render text tables to a stream. Table wraps a shared TableInternal that
 * holds the actual rows, columns and formatting state.
 */

/**
 * @brief User-facing table builder that accumulates rows and renders them as text.
 *
 * Table is a thin, copyable handle over a shared TableInternal instance. It
 * accepts heterogeneous cell values (strings, C-strings, string_view, or even
 * nested Table objects), converts them to strings, and forwards them to the
 * internal representation for layout and printing.
 */
class Table {
public:
  /** @brief Constructs an empty table with a freshly created internal backing store. */
  Table() : table_(TableInternal::create()) {}

  /** @brief Cell value type for a row: each cell may hold a std::string, C-string, string_view, or nested Table. */
  using Row_t = std::vector<variant<std::string, const char *, string_view, Table>>;

  /**
   * @brief Appends a row of heterogeneous cells to the table.
   *
   * The first row added fixes the table's column count. Each cell variant is
   * resolved to a std::string: nested Table values are rendered to their own
   * text block before insertion. The resulting strings are handed to the
   * internal table, mutating this object's row/column state.
   *
   * @param cells Row of cell values; may hold strings, C-strings, string_view, or nested tables.
   * @return Reference to this table, enabling call chaining.
   */
  Table &add_row(const Row_t &cells) {

    if (rows_ == 0) {
      // This is the first row added
      // cells.size() is the number of columns
      cols_ = cells.size();
    }

    std::vector<std::string> cell_strings;
    if (cells.size() < cols_) {
      cell_strings.resize(cols_);
      std::fill(cell_strings.begin(), cell_strings.end(), "");
    } else {
      cell_strings.resize(cells.size());
      std::fill(cell_strings.begin(), cell_strings.end(), "");
    }

    for (size_t i = 0; i < cells.size(); ++i) {
      auto cell = cells[i];
      if (holds_alternative<std::string>(cell)) {
        cell_strings[i] = *get_if<std::string>(&cell);
      } else if (holds_alternative<const char *>(cell)) {
        cell_strings[i] = *get_if<const char *>(&cell);
      }  else if (holds_alternative<string_view>(cell)) {
        cell_strings[i] = std::string{*get_if<string_view>(&cell)};
      } else {
        auto table = *get_if<Table>(&cell);
        std::stringstream stream;
        table.print(stream);
        cell_strings[i] = stream.str();
      }
    }

    table_->add_row(cell_strings);
    rows_ += 1;
    return *this;
  }

  /**
   * @brief Subscript access to a row by index.
   * @param index Zero-based row index.
   * @return Reference to the requested Row.
   */
  Row &operator[](size_t index) { return row(index); }

  /**
   * @brief Returns the row at the given index.
   * @param index Zero-based row index.
   * @return Reference to the requested Row from the internal table.
   */
  Row &row(size_t index) { return (*table_)[index]; }

  /**
   * @brief Returns a column view aggregating the cells at the given index across all rows.
   * @param index Zero-based column index.
   * @return A Column object spanning that column.
   */
  Column column(size_t index) { return table_->column(index); }

  /**
   * @brief Accesses the table-level formatting object.
   * @return Reference to the Format applied to the whole table.
   */
  Format &format() { return table_->format(); }

  /**
   * @brief Renders the table to the given output stream.
   * @param stream Destination stream that receives the formatted table text.
   */
  void print(std::ostream &stream) { table_->print(stream); }

  /**
   * @brief Renders the table to a string.
   * @return The formatted table as a std::string.
   */
  std::string str() {
    std::stringstream stream;
    print(stream);
    return stream.str();
  }

  /**
   * @brief Returns the rendered dimensions of the table.
   * @return Pair of {width in characters, height in lines} as produced by rendering.
   */
  std::pair<size_t, size_t> shape() { return table_->shape(); }

  /**
   * @brief Forward iterator over the table's rows.
   *
   * Wraps the underlying vector-of-shared-pointers iterator and dereferences
   * to a Row reference, enabling range-based for over a Table.
   */
  class RowIterator {
  public:
    /** @brief Constructs an iterator wrapping the given underlying row-pointer iterator. */
    explicit RowIterator(std::vector<std::shared_ptr<Row>>::iterator ptr) : ptr(ptr) {}

    /** @brief Pre-increment; advances to the next row. @return This iterator after advancing. */
    RowIterator operator++() {
      ++ptr;
      return *this;
    }
    /** @brief Inequality comparison against another iterator. @param other Iterator to compare with. @return true if the two point to different positions. */
    bool operator!=(const RowIterator &other) const { return ptr != other.ptr; }
    /** @brief Dereferences to the current row. @return Reference to the Row at the current position. */
    Row &operator*() { return **ptr; }

  private:
    std::vector<std::shared_ptr<Row>>::iterator ptr; ///< Underlying iterator over the internal row list.
  };

  /** @brief Returns an iterator to the first row. @return RowIterator at the beginning. */
  auto begin() -> RowIterator { return RowIterator(table_->rows_.begin()); }
  /** @brief Returns an iterator past the last row. @return RowIterator at the end. */
  auto end() -> RowIterator { return RowIterator(table_->rows_.end()); }

private:
  friend class MarkdownExporter;
  friend class LatexExporter;
  friend class AsciiDocExporter;

  friend std::ostream &operator<<(std::ostream &stream, const Table &table);
  size_t rows_{0};                          ///< Number of rows added so far.
  size_t cols_{0};                          ///< Column count, fixed by the first added row.
  std::shared_ptr<TableInternal> table_;    ///< Shared internal table holding rows and formatting.
};

/**
 * @brief Stream insertion operator that renders a table to an output stream.
 *
 * Casts away constness to invoke the mutating print() path, so the table's
 * rendering side effects are applied while writing to the stream.
 *
 * @param stream Destination output stream.
 * @param table Table to render.
 * @return Reference to the same output stream, for chaining.
 */
inline std::ostream &operator<<(std::ostream &stream, const Table &table) {
  const_cast<Table &>(table).print(stream);
  return stream;
}

} // namespace tabulate
