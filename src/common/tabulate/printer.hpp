
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
#include <tabulate/color.hpp>
#include <tabulate/font_style.hpp>
#include <utility>
#include <vector>

namespace tabulate {

/**
 * @file printer.hpp
 * @brief Declares tabulate::Printer, a stateless helper that renders a
 *        TableInternal to an output stream, including cell sizing, borders,
 *        text alignment, and ANSI color/font styling via termcolor.
 */

/**
 * @brief Stateless rendering engine for tabulate tables.
 *
 * All members are static. The public interface computes cell dimensions and
 * prints a table (rows, borders, and content) to a std::ostream. Private
 * helpers handle per-alignment content emission and map tabulate Color/FontStyle
 * enums to termcolor escape sequences.
 */
class Printer {
public:
  /**
   * @brief Computes the rendered width and height of every cell in the table.
   * @param table Table whose cells are measured.
   * @return Pair of (column widths, row heights), each as a vector sized to the table.
   */
  static std::pair<std::vector<size_t>, std::vector<size_t>>
  compute_cell_dimensions(TableInternal &table);

  /**
   * @brief Renders the entire table to the given stream.
   * @param stream Output stream to write the formatted table to.
   * @param table Table to render.
   */
  static void print_table(std::ostream &stream, TableInternal &table);

  /**
   * @brief Prints one visual line of a single cell (one row of the cell's content).
   * @param stream Output stream to write to.
   * @param table Table being rendered.
   * @param index (row, column) index of the cell.
   * @param dimension (width, height) of the cell.
   * @param num_columns Total number of columns in the table.
   * @param row_index Zero-based line index within the cell being printed.
   */
  static void print_row_in_cell(std::ostream &stream, TableInternal &table,
                                const std::pair<size_t, size_t> &index,
                                const std::pair<size_t, size_t> &dimension, size_t num_columns,
                                size_t row_index);

  /**
   * @brief Prints the top border segment for a cell, if applicable.
   * @param stream Output stream to write to.
   * @param table Table being rendered.
   * @param index (row, column) index of the cell.
   * @param dimension (width, height) of the cell.
   * @param num_columns Total number of columns in the table.
   * @return True if a border was printed for this cell.
   */
  static bool print_cell_border_top(std::ostream &stream, TableInternal &table,
                                    const std::pair<size_t, size_t> &index,
                                    const std::pair<size_t, size_t> &dimension, size_t num_columns);
  /**
   * @brief Prints the bottom border segment for a cell, if applicable.
   * @param stream Output stream to write to.
   * @param table Table being rendered.
   * @param index (row, column) index of the cell.
   * @param dimension (width, height) of the cell.
   * @param num_columns Total number of columns in the table.
   * @return True if a border was printed for this cell.
   */
  static bool print_cell_border_bottom(std::ostream &stream, TableInternal &table,
                                       const std::pair<size_t, size_t> &index,
                                       const std::pair<size_t, size_t> &dimension,
                                       size_t num_columns);

  /**
   * @brief Emits ANSI escape codes to set foreground color, background color,
   *        and font styles on the stream.
   * @param stream Output stream to write escape codes to.
   * @param foreground_color Text (foreground) color to apply.
   * @param background_color Background color to apply.
   * @param font_style List of font styles (bold, italic, ...) to apply in order.
   */
  static void apply_element_style(std::ostream &stream, Color foreground_color,
                                  Color background_color,
                                  const std::vector<FontStyle> &font_style) {
    apply_foreground_color(stream, foreground_color);
    apply_background_color(stream, background_color);
    for (auto &style : font_style)
      apply_font_style(stream, style);
  }

  /**
   * @brief Emits the ANSI reset escape code, clearing any active styling.
   * @param stream Output stream to write the reset code to.
   */
  static void reset_element_style(std::ostream &stream) { stream << termcolor::reset; }

private:
  /**
   * @brief Prints left-aligned cell content, styling the text and padding the
   *        remaining column width with spaces on the right.
   * @param stream Output stream to write to.
   * @param cell_content The (already word-wrapped) text to print.
   * @param format Cell format supplying color and font-style options.
   * @param text_with_padding_size Rendered width of the content plus its padding.
   * @param column_width Total available column width.
   */
  static void print_content_left_aligned(std::ostream &stream, const std::string &cell_content,
                                         const Format &format, size_t text_with_padding_size,
                                         size_t column_width) {

    // Apply font style
    apply_element_style(stream, *format.font_color_, *format.font_background_color_,
                        *format.font_style_);
    stream << cell_content;
    // Only apply font_style to the font
    // Not the padding. So calling apply_element_style with font_style = {}
    reset_element_style(stream);
    apply_element_style(stream, *format.font_color_, *format.font_background_color_, {});

    if (text_with_padding_size < column_width) {
      for (size_t j = 0; j < (column_width - text_with_padding_size); ++j) {
        stream << " ";
      }
    }
  }

  /**
   * @brief Prints center-aligned cell content, distributing padding spaces on
   *        both sides (extra space goes to the left when the gap is odd).
   * @param stream Output stream to write to.
   * @param cell_content The (already word-wrapped) text to print.
   * @param format Cell format supplying color and font-style options.
   * @param text_with_padding_size Rendered width of the content plus its padding.
   * @param column_width Total available column width.
   */
  static void print_content_center_aligned(std::ostream &stream, const std::string &cell_content,
                                           const Format &format, size_t text_with_padding_size,
                                           size_t column_width) {
    auto num_spaces = column_width - text_with_padding_size;
    if (num_spaces % 2 == 0) {
      // Even spacing on either side
      for (size_t j = 0; j < num_spaces / 2; ++j)
        stream << " ";

      // Apply font style
      apply_element_style(stream, *format.font_color_, *format.font_background_color_,
                          *format.font_style_);
      stream << cell_content;
      // Only apply font_style to the font
      // Not the padding. So calling apply_element_style with font_style = {}
      reset_element_style(stream);
      apply_element_style(stream, *format.font_color_, *format.font_background_color_, {});

      for (size_t j = 0; j < num_spaces / 2; ++j)
        stream << " ";
    } else {
      auto num_spaces_before = num_spaces / 2 + 1;
      for (size_t j = 0; j < num_spaces_before; ++j)
        stream << " ";

      // Apply font style
      apply_element_style(stream, *format.font_color_, *format.font_background_color_,
                          *format.font_style_);
      stream << cell_content;
      // Only apply font_style to the font
      // Not the padding. So calling apply_element_style with font_style = {}
      reset_element_style(stream);
      apply_element_style(stream, *format.font_color_, *format.font_background_color_, {});

      for (size_t j = 0; j < num_spaces - num_spaces_before; ++j)
        stream << " ";
    }
  }

  /**
   * @brief Prints right-aligned cell content, padding the column with spaces on
   *        the left before emitting the styled text.
   * @param stream Output stream to write to.
   * @param cell_content The (already word-wrapped) text to print.
   * @param format Cell format supplying color and font-style options.
   * @param text_with_padding_size Rendered width of the content plus its padding.
   * @param column_width Total available column width.
   */
  static void print_content_right_aligned(std::ostream &stream, const std::string &cell_content,
                                          const Format &format, size_t text_with_padding_size,
                                          size_t column_width) {
    if (text_with_padding_size < column_width) {
      for (size_t j = 0; j < (column_width - text_with_padding_size); ++j) {
        stream << " ";
      }
    }

    // Apply font style
    apply_element_style(stream, *format.font_color_, *format.font_background_color_,
                        *format.font_style_);
    stream << cell_content;
    // Only apply font_style to the font
    // Not the padding. So calling apply_element_style with font_style = {}
    reset_element_style(stream);
    apply_element_style(stream, *format.font_color_, *format.font_background_color_, {});
  }

  /**
   * @brief Maps a single FontStyle enum value to its termcolor escape sequence.
   * @param stream Output stream to write the escape sequence to.
   * @param style Font style to apply (bold, dark, italic, underline, blink,
   *              reverse, concealed, crossed); unknown values are ignored.
   */
  static void apply_font_style(std::ostream &stream, FontStyle style) {
    switch (style) {
    case FontStyle::bold:
      stream << termcolor::bold;
      break;
    case FontStyle::dark:
      stream << termcolor::dark;
      break;
    case FontStyle::italic:
      stream << termcolor::italic;
      break;
    case FontStyle::underline:
      stream << termcolor::underline;
      break;
    case FontStyle::blink:
      stream << termcolor::blink;
      break;
    case FontStyle::reverse:
      stream << termcolor::reverse;
      break;
    case FontStyle::concealed:
      stream << termcolor::concealed;
      break;
    case FontStyle::crossed:
      stream << termcolor::crossed;
      break;
    default:
      break;
    }
  }

  /**
   * @brief Maps a Color enum value to its termcolor foreground escape sequence.
   * @param stream Output stream to write the escape sequence to.
   * @param foreground_color Foreground color to apply; Color::none is a no-op.
   */
  static void apply_foreground_color(std::ostream &stream, Color foreground_color) {
    switch (foreground_color) {
    case Color::grey:
      stream << termcolor::grey;
      break;
    case Color::red:
      stream << termcolor::red;
      break;
    case Color::green:
      stream << termcolor::green;
      break;
    case Color::yellow:
      stream << termcolor::yellow;
      break;
    case Color::blue:
      stream << termcolor::blue;
      break;
    case Color::magenta:
      stream << termcolor::magenta;
      break;
    case Color::cyan:
      stream << termcolor::cyan;
      break;
    case Color::white:
      stream << termcolor::white;
      break;
    case Color::none:
    default:
      break;
    }
  }

  /**
   * @brief Maps a Color enum value to its termcolor background escape sequence.
   * @param stream Output stream to write the escape sequence to.
   * @param background_color Background color to apply; Color::none is a no-op.
   */
  static void apply_background_color(std::ostream &stream, Color background_color) {
    switch (background_color) {
    case Color::grey:
      stream << termcolor::on_grey;
      break;
    case Color::red:
      stream << termcolor::on_red;
      break;
    case Color::green:
      stream << termcolor::on_green;
      break;
    case Color::yellow:
      stream << termcolor::on_yellow;
      break;
    case Color::blue:
      stream << termcolor::on_blue;
      break;
    case Color::magenta:
      stream << termcolor::on_magenta;
      break;
    case Color::cyan:
      stream << termcolor::on_cyan;
      break;
    case Color::white:
      stream << termcolor::on_white;
      break;
    case Color::none:
    default:
      break;
    }
  }
};

} // namespace tabulate
