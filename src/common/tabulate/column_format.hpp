
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
 * @file column_format.hpp
 * @brief Declares ColumnFormat, a fluent formatter that applies styling to
 *        every cell in a single table column.
 */
#pragma once

namespace tabulate {

/**
 * @brief Formatting proxy for an entire column.
 *
 * ColumnFormat derives from Format and exposes the same styling setters, but
 * each setter is implemented (in column.hpp) to fan the value out to every
 * Cell in the referenced Column. Setters return @c *this so calls can be
 * chained fluently. The definitions live in column.hpp because they depend on
 * the complete Column type.
 */
class ColumnFormat : public Format {
public:
  /**
   * @brief Constructs a formatter bound to a given column.
   * @param column The column whose cells will be styled by this formatter.
   *        A reference is stored; the column must outlive this object.
   */
  explicit ColumnFormat(class Column &column) : column_(column) {}

  /** @brief Sets a fixed display width (in characters) for every cell.
   *  @param value Column width. @return Reference to this formatter. */
  ColumnFormat &width(size_t value);
  /** @brief Sets a fixed display height (in lines) for every cell.
   *  @param value Column height. @return Reference to this formatter. */
  ColumnFormat &height(size_t value);

  // Padding
  /** @brief Sets padding on all four sides of every cell.
   *  @param value Padding amount. @return Reference to this formatter. */
  ColumnFormat &padding(size_t value);
  /** @brief Sets left padding of every cell.
   *  @param value Left padding. @return Reference to this formatter. */
  ColumnFormat &padding_left(size_t value);
  /** @brief Sets right padding of every cell.
   *  @param value Right padding. @return Reference to this formatter. */
  ColumnFormat &padding_right(size_t value);
  /** @brief Sets top padding of every cell.
   *  @param value Top padding. @return Reference to this formatter. */
  ColumnFormat &padding_top(size_t value);
  /** @brief Sets bottom padding of every cell.
   *  @param value Bottom padding. @return Reference to this formatter. */
  ColumnFormat &padding_bottom(size_t value);

  // Border
  /** @brief Sets the border glyph on all sides of every cell.
   *  @param value Border string. @return Reference to this formatter. */
  ColumnFormat &border(const std::string &value);
  /** @brief Sets the border color on all sides of every cell.
   *  @param value Border color. @return Reference to this formatter. */
  ColumnFormat &border_color(Color value);
  /** @brief Sets the border background color on all sides of every cell.
   *  @param value Background color. @return Reference to this formatter. */
  ColumnFormat &border_background_color(Color value);
  /** @brief Sets the left border glyph of every cell.
   *  @param value Border string. @return Reference to this formatter. */
  ColumnFormat &border_left(const std::string &value);
  /** @brief Sets the left border color of every cell.
   *  @param value Border color. @return Reference to this formatter. */
  ColumnFormat &border_left_color(Color value);
  /** @brief Sets the left border background color of every cell.
   *  @param value Background color. @return Reference to this formatter. */
  ColumnFormat &border_left_background_color(Color value);
  /** @brief Sets the right border glyph of every cell.
   *  @param value Border string. @return Reference to this formatter. */
  ColumnFormat &border_right(const std::string &value);
  /** @brief Sets the right border color of every cell.
   *  @param value Border color. @return Reference to this formatter. */
  ColumnFormat &border_right_color(Color value);
  /** @brief Sets the right border background color of every cell.
   *  @param value Background color. @return Reference to this formatter. */
  ColumnFormat &border_right_background_color(Color value);
  /** @brief Sets the top border glyph of every cell.
   *  @param value Border string. @return Reference to this formatter. */
  ColumnFormat &border_top(const std::string &value);
  /** @brief Sets the top border color of every cell.
   *  @param value Border color. @return Reference to this formatter. */
  ColumnFormat &border_top_color(Color value);
  /** @brief Sets the top border background color of every cell.
   *  @param value Background color. @return Reference to this formatter. */
  ColumnFormat &border_top_background_color(Color value);
  /** @brief Sets the bottom border glyph of every cell.
   *  @param value Border string. @return Reference to this formatter. */
  ColumnFormat &border_bottom(const std::string &value);
  /** @brief Sets the bottom border color of every cell.
   *  @param value Border color. @return Reference to this formatter. */
  ColumnFormat &border_bottom_color(Color value);
  /** @brief Sets the bottom border background color of every cell.
   *  @param value Background color. @return Reference to this formatter. */
  ColumnFormat &border_bottom_background_color(Color value);

  // Corner
  /** @brief Sets the corner glyph of every cell.
   *  @param value Corner string. @return Reference to this formatter. */
  ColumnFormat &corner(const std::string &value);
  /** @brief Sets the corner color of every cell.
   *  @param value Corner color. @return Reference to this formatter. */
  ColumnFormat &corner_color(Color value);
  /** @brief Sets the corner background color of every cell.
   *  @param value Background color. @return Reference to this formatter. */
  ColumnFormat &corner_background_color(Color value);

  // Column separator
  /** @brief Sets the column separator glyph of every cell.
   *  @param value Separator string. @return Reference to this formatter. */
  ColumnFormat &column_separator(const std::string &value);
  /** @brief Sets the column separator color of every cell.
   *  @param value Separator color. @return Reference to this formatter. */
  ColumnFormat &column_separator_color(Color value);
  /** @brief Sets the column separator background color of every cell.
   *  @param value Background color. @return Reference to this formatter. */
  ColumnFormat &column_separator_background_color(Color value);

  // Font styling
  /** @brief Sets the horizontal text alignment of every cell.
   *  @param value Font alignment. @return Reference to this formatter. */
  ColumnFormat &font_align(FontAlign value);
  /** @brief Sets the font styles (bold, italic, etc.) of every cell.
   *  @param style List of font styles. @return Reference to this formatter. */
  ColumnFormat &font_style(const std::vector<FontStyle> &style);
  /** @brief Sets the font (foreground) color of every cell.
   *  @param value Font color. @return Reference to this formatter. */
  ColumnFormat &font_color(Color value);
  /** @brief Sets the font background color of every cell.
   *  @param value Background color. @return Reference to this formatter. */
  ColumnFormat &font_background_color(Color value);
  /** @brief Alias for font_color; sets the foreground color of every cell.
   *  @param value Foreground color. @return Reference to this formatter. */
  ColumnFormat &color(Color value);
  /** @brief Alias for font_background_color of every cell.
   *  @param value Background color. @return Reference to this formatter. */
  ColumnFormat &background_color(Color value);

  // Locale
  /** @brief Enables/disables multi-byte (wide) character handling per cell.
   *  @param value True to enable multi-byte support. @return Reference to this formatter. */
  ColumnFormat &multi_byte_characters(bool value);
  /** @brief Sets the locale used for text measurement of every cell.
   *  @param value Locale name. @return Reference to this formatter. */
  ColumnFormat &locale(const std::string &value);

private:
  std::reference_wrapper<class Column> column_; ///< Reference to the styled column.
};

} // namespace tabulate
