/******************************************************************************
 * \file      geometry_keyword_parser.cpp
 * \brief     Basic classes parser group keywords and seprate the data from
 *            keywords.
 *
 * \details   Implementation of basic classes parser group keywords and seprate
 *            the data from keywords.
 *
 * \author    Xiaofeng Li
 * \email     xfli@whrsm.ac.cn
 * \date      Created:       June 24, 2020
 * \date      Last modified: 2026-07-20 00:03:13
 * \version   OpenDFN Version 1.0.0 (managed by CMake macro PRG_VERSION,
 *            generated into common/opendfn_config.h from opendfn_config.h.in)
 *
 * \see OpenDFN Project Website: https://xiaofengli-uoft.github.io/Mainpage/
 * \see Geomechanics Group Website: https://geogroup.utoronto.ca/
 *
 * The OpenDFN Project is maintained by the OpenDFN Foundation.
 *
 * Copyright (C) 2017-2026 Xiaofeng Li. OpenDFN Contributors.
 *
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

// local includes
#include <mesh/group/geometry_keyword_parser.h>
#include "common/opendfn_math.h"

using namespace ns_common;

namespace ns_mesh {
/**
 * \brief Parse the range keywords of one input line into a geometric parameter container.
 *
 * Reads the tokens that follow a group tag on a single line, determines which range
 * method they describe (box / circle / plane), and dispatches to the matching helper
 * that fills \p container with the geometric parameters. On an unsupported method the
 * program prints an error and exits.
 *
 * \param file      Input stream positioned right after the group tag; the current line is consumed.
 * \param container Output array that receives the parsed geometric parameters.
 * \param range     Output range method detected from the keywords.
 *
 * Side effects: advances \p file by one line, mutates \p container and \p range,
 * allocates and frees a temporary Keywords buffer, may terminate the process on error.
 */
void GeometryKeywordParser::parseRangeKeywords(FILE* file, Real container[], rangeMethod& range) {
    Keywords keyword;
    // get the all strings in one line after the group tag
    parseStringinLine(file, keyword);

    // get the range method
    getRangeMethod(keyword, range);
    // find the range to container
    switch (range) {
        case rangeMethod::SQUAREIN:
        case rangeMethod::SQUAREON:
        case rangeMethod::SQUAREOUT: pushSquareKeywords(keyword, container); break;
        case rangeMethod::CIRCLEIN:
        case rangeMethod::CIRCLEON:
        case rangeMethod::CIRCLEOUT: pushCircleKeywords(keyword, container); break;
        case rangeMethod::PLANELEFT:
        case rangeMethod::PLANEON:
        case rangeMethod::PLANERIGHT: pushPlaneKeywords(keyword, container); break;
        default:
            OpenDFNMessage::RuntimeInfo("Error: not supported range method.");
            OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
            break;
    }
    // free the memory
    keyword.clean();
}
/**
 * \brief Parse the range keywords of one input line, with polygon and coordinate support.
 *
 * Extends the base overload by additionally handling polygon ranges (whose vertices
 * are read into \p polygon, possibly spanning several lines) and the by-coordinate method.
 *
 * \param file      Input stream positioned right after the group tag; one or more lines are consumed.
 * \param container Output array that receives box / circle / plane / coordinate parameters.
 * \param range     Output range method detected from the keywords.
 * \param polygon   Output vertex list, filled when the range method is a polygon variant.
 *
 * Side effects: advances \p file, mutates \p container, \p range and \p polygon,
 * allocates and frees a temporary Keywords buffer, may terminate the process on error.
 */
void GeometryKeywordParser::parseRangeKeywords(FILE* file, Real container[], rangeMethod& range, std::vector<Vector2>& polygon) {
    Keywords keyword;
    // get the all strings in one line after the group tag
    parseStringinLine(file, keyword);

    // get the range method
    getRangeMethod(keyword, range);
    // find the range to container
    switch (range) {
        case rangeMethod::SQUAREIN:
        case rangeMethod::SQUAREON:
        case rangeMethod::SQUAREOUT: pushSquareKeywords(keyword, container); break;
        case rangeMethod::CIRCLEIN:
        case rangeMethod::CIRCLEON:
        case rangeMethod::CIRCLEOUT: pushCircleKeywords(keyword, container); break;
        case rangeMethod::PLANELEFT:
        case rangeMethod::PLANEON:
        case rangeMethod::PLANERIGHT: pushPlaneKeywords(keyword, container); break;
        case rangeMethod::POLYGONON:
        case rangeMethod::POLYGONIN:
        case rangeMethod::POLYGONOUT: pushPolygonGeometryKeywords(file, keyword, polygon); break;
        case rangeMethod::BYCOORD: pushCoordKeywords(keyword, container); break;
        default:
            OpenDFNMessage::RuntimeInfo("Error: not supported range method.");
            OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
            break;
    }
    // free the memory
    keyword.clean();
}
/**
 * \brief Parse the range keywords of one input line, also collecting cell/subdivision sizes.
 *
 * Variant used by MPM-style groups that need a cell resolution in addition to the
 * geometric parameters. Supports box, circle, polygon and by-coordinate methods and
 * writes the per-axis cell counts into \p size.
 *
 * \param file      Input stream positioned right after the group tag; one or more lines are consumed.
 * \param container Output array that receives the geometric parameters.
 * \param range     Output range method detected from the keywords.
 * \param polygon   Output vertex list, filled when the range method is a polygon variant.
 * \param size      Output array of per-axis cell counts.
 *
 * Side effects: advances \p file, mutates \p container, \p range, \p polygon and \p size,
 * allocates and frees a temporary Keywords buffer, may terminate the process on error.
 */
void GeometryKeywordParser::parseRangeKeywords(FILE* file, Real container[], rangeMethod& range, std::vector<Vector2>& polygon, Int size[]) {
    Keywords keyword;
    // get the all strings in one line after the group tag
    parseStringinLine(file, keyword);

    // get the range method
    getRangeMethod(keyword, range);
    // find the range to container
    switch (range) {
        case rangeMethod::SQUAREIN:
        case rangeMethod::SQUAREON:
        case rangeMethod::SQUAREOUT: pushSquareKeywords(keyword, container, size); break;
        case rangeMethod::CIRCLEIN:
        case rangeMethod::CIRCLEON:
        case rangeMethod::CIRCLEOUT: pushCircleKeywords(keyword, container, size); break;
        case rangeMethod::POLYGONON:
        case rangeMethod::POLYGONIN:
        case rangeMethod::POLYGONOUT: pushPolygonGeometryKeywords(file, keyword, polygon, size); break;
        case rangeMethod::BYCOORD: pushCoordKeywords(keyword, container); break;
        default:
            OpenDFNMessage::RuntimeInfo("Error: not supported method.");
            OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
            break;
    }
    // free the memory
    keyword.clean();
}
/**
 * \brief Map the "range"/"method" keyword and its argument to a rangeMethod enum value.
 *
 * Scans the token list for a "range" or "method" keyword and inspects the following
 * token (e.g. "box.in", "circle.out", "plane.above", "polygon", "coord") to select the
 * matching enum. Unrecognised values print an error and exit.
 *
 * \param keyword Parsed keyword tokens for the current group line.
 * \param range   Output range method resolved from the keywords.
 *
 * Side effects: mutates \p range; may terminate the process on an unknown method.
 */
void GeometryKeywordParser::getRangeMethod(Keywords& keyword, rangeMethod& range) {
    // scan every token looking for the "range" or "method" selector
    _for(j, 0, keyword.count) {
        if (isSamePrefix(keyword.keywords[j], "range", 3)) {
            if (isSameString(keyword.keywords[j + 1], "box.in"))
                range = rangeMethod::SQUAREIN;
            else if (isSameString(keyword.keywords[j + 1], "box"))
                range = rangeMethod::SQUAREIN;
            else if (isSameString(keyword.keywords[j + 1], "box.on"))
                range = rangeMethod::SQUAREON;
            else if (isSameString(keyword.keywords[j + 1], "box.out"))
                range = rangeMethod::SQUAREOUT;
            else if (isSameString(keyword.keywords[j + 1], "circle.on"))
                range = rangeMethod::CIRCLEON;
            else if (isSameString(keyword.keywords[j + 1], "circle.in"))
                range = rangeMethod::CIRCLEIN;
            else if (isSameString(keyword.keywords[j + 1], "circle"))
                range = rangeMethod::CIRCLEIN;
            else if (isSameString(keyword.keywords[j + 1], "circle.out"))
                range = rangeMethod::CIRCLEOUT;
            else if (isSameString(keyword.keywords[j + 1], "circle.on"))
                range = rangeMethod::CIRCLEON;
            else if (isSameString(keyword.keywords[j + 1], "plane.on"))
                range = rangeMethod::PLANEON;
            else if (isSameString(keyword.keywords[j + 1], "plane.above"))
                range = rangeMethod::PLANELEFT;
            else if (isSameString(keyword.keywords[j + 1], "plane.below"))
                range = rangeMethod::PLANERIGHT;
            else if (isSameString(keyword.keywords[j + 1], "polygon.on"))
                range = rangeMethod::POLYGONON;
            else if (isSameString(keyword.keywords[j + 1], "polygon.in"))
                range = rangeMethod::POLYGONIN;
            else if (isSameString(keyword.keywords[j + 1], "polygon"))
                range = rangeMethod::POLYGONIN;
            else if (isSameString(keyword.keywords[j + 1], "polygon.out"))
                range = rangeMethod::POLYGONOUT;
            else if (isSameString(keyword.keywords[j + 1], "coord"))
                range = rangeMethod::BYCOORD;
            else {
                OpenDFNMessage::RuntimebackStringString("Error: unrecognised range method ", keyword.keywords[j]);
                OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
            }
            break;
        } else if (isSamePrefix(keyword.keywords[j], "method", 3)) {
            if (isSameString(keyword.keywords[j + 1], "rectangle"))
                range = rangeMethod::SQUAREIN;
            else if (isSameString(keyword.keywords[j + 1], "circle"))
                range = rangeMethod::CIRCLEIN;
            else if (isSameString(keyword.keywords[j + 1], "polygon"))
                range = rangeMethod::POLYGONIN;
            else {
                OpenDFNMessage::RuntimebackStringString("Error: unrecognised range method ", keyword.keywords[j]);
                OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
            }
            break;
        }
    }
}

/**
 * \brief Extract box/rectangle parameters and cell sizes from an already-parsed keyword list.
 *
 * Recognises the named forms (xmin/xmax/ymin/ymax, xcell/ycell), the bracketed
 * x/y/cell pairs that follow a "range" keyword, and the bare
 * [xmin xmax ymin ymax] bracket form. Fills \p container[0..3] with the bounds and
 * \p size[0..1] with the cell counts. Unrecognised tokens print an error and exit.
 *
 * \param keyword   Parsed keyword tokens for the current group line.
 * \param container Output bounds array [xmin, xmax, ymin, ymax].
 * \param size      Output cell-count array [xcell, ycell].
 *
 * Side effects: mutates \p container and \p size; may terminate the process on error.
 */
void GeometryKeywordParser::pushSquareKeywords(Keywords& keyword, Real container[], Int size[]) {
    // becomes true once the "range"/"method" selector has been seen
    bool afterrange = false;
    _for(j, 0, keyword.count) {
        if (isSamePrefix(keyword.keywords[j], "xmin", 3)) {
            container[0] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSamePrefix(keyword.keywords[j], "xmax", 3)) {
            container[1] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSamePrefix(keyword.keywords[j], "xcell", 3)) {
            size[0] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSamePrefix(keyword.keywords[j], "ymin", 3)) {
            container[2] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSamePrefix(keyword.keywords[j], "ymax", 3)) {
            container[3] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSamePrefix(keyword.keywords[j], "ycell", 3)) {
            size[1] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSameString(keyword.keywords[j], "x") && afterrange && keyword.findbracket) {
            container[0] = strtod(keyword.keywords[j + 1], NULL);
            j++;
            container[1] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSameString(keyword.keywords[j], "y") && afterrange && keyword.findbracket) {
            container[2] = strtod(keyword.keywords[j + 1], NULL);
            j++;
            container[3] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSameString(keyword.keywords[j], "cell") && afterrange && keyword.findbracket) {
            size[0] = strtod(keyword.keywords[j + 1], NULL);
            j++;
            size[1] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSameString(keyword.keywords[j], "range") || isSameString(keyword.keywords[j], "method")) {
            afterrange = true;
            // skip the range keyword
            j++;
        } else if (afterrange && keyword.findbracket) {
            // [xmin xmax ymin ymax]
            j--;
            container[0] = strtod(keyword.keywords[j + 1], NULL);
            j++;
            container[1] = strtod(keyword.keywords[j + 1], NULL);
            j++;
            container[2] = strtod(keyword.keywords[j + 1], NULL);
            j++;
            container[3] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else {
            OpenDFNMessage::RuntimebackStringString("Error: unrecognised keyword ", keyword.keywords[j]);
            OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
        }
    }
}

/**
 * \brief Extract a single (x, y) coordinate from an already-parsed keyword list.
 *
 * Handles the named "x"/"y" forms and the bracketed pair that follows a
 * "range"/"method" keyword. Fills \p container[0..1]. Unrecognised tokens
 * print an error and exit.
 *
 * \param keyword   Parsed keyword tokens for the current group line.
 * \param container Output coordinate array [x, y].
 *
 * Side effects: mutates \p container; may terminate the process on error.
 */
void GeometryKeywordParser::pushCoordKeywords(Keywords& keyword, Real container[]) {
    bool afterrange = false;
    _for(j, 0, keyword.count) {
        if (isSameString(keyword.keywords[j], "x")) {
            container[0] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSameString(keyword.keywords[j], "y")) {
            container[1] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSameString(keyword.keywords[j], "range") || isSameString(keyword.keywords[j], "method")) {
            afterrange = true;
            // skip the range keyword
            j++;
        } else if (afterrange && keyword.findbracket) {
            // [xmin xmax ymin ymax]
            j--;
            container[0] = strtod(keyword.keywords[j + 1], NULL);
            j++;
            container[1] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else {
            OpenDFNMessage::RuntimebackStringString("Error: unrecognised keyword ", keyword.keywords[j]);
            OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
        }
    }
}

/**
 * \brief Extract circle parameters and cell sizes from an already-parsed keyword list.
 *
 * Recognises named forms (x/y/radius, xcell/ycell), the "center [x y]" and "cell"
 * forms after a "range" keyword, and the bare [x y r] bracket form. Fills
 * \p container[0..2] with centre and radius and \p size[0..1] with cell counts.
 * Unrecognised tokens print an error and exit.
 *
 * \param keyword   Parsed keyword tokens for the current group line.
 * \param container Output array [centre.x, centre.y, radius].
 * \param size      Output cell-count array [xcell, ycell].
 *
 * Side effects: mutates \p container and \p size; may terminate the process on error.
 */
void GeometryKeywordParser::pushCircleKeywords(Keywords& keyword, Real container[], Int size[]) {
    bool afterrange = false;
    _for(j, 0, keyword.count) {
        if (isSameString(keyword.keywords[j], "x")) {
            container[0] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSameString(keyword.keywords[j], "y")) {
            container[1] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSamePrefix(keyword.keywords[j], "radius", 3)) {
            container[2] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSamePrefix(keyword.keywords[j], "xcell", 3)) {
            size[0] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSamePrefix(keyword.keywords[j], "ycell", 3)) {
            size[1] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSameString(keyword.keywords[j], "range") || isSameString(keyword.keywords[j], "method")) {
            afterrange = true;
            // skip the range keyword
            j++;
        } else if (isSamePrefix(keyword.keywords[j], "center", 1) && afterrange) {
            // center [x y]
            container[0] = strtod(keyword.keywords[j + 1], NULL);
            j++;
            container[1] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSameString(keyword.keywords[j], "cell") && afterrange && keyword.findbracket) {
            size[0] = strtod(keyword.keywords[j + 1], NULL);
            j++;
            size[1] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (afterrange && keyword.findbracket) {
            j--;
            // [x y r]
            container[0] = strtod(keyword.keywords[j + 1], NULL);
            j++;
            container[1] = strtod(keyword.keywords[j + 1], NULL);
            j++;
            container[2] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else {
            OpenDFNMessage::RuntimebackStringString("Error: unrecognised keyword ", keyword.keywords[j]);
            OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
        }
    }
}

/**
 * \brief Extract plane (two-point line) parameters from an already-parsed keyword list.
 *
 * Recognises the named x0/y0/x1/y1 forms, the "p1"/"p2" point forms after a "range"
 * keyword, and the bare [x0 y0 x1 y1] bracket form. Fills \p container[0..3] with the
 * two endpoints. Unrecognised tokens print an error and exit.
 *
 * \param keyword   Parsed keyword tokens for the current group line.
 * \param container Output array [x0, y0, x1, y1].
 *
 * Side effects: mutates \p container; may terminate the process on error.
 */
void GeometryKeywordParser::pushPlaneKeywords(Keywords& keyword, Real container[]) {
    bool afterrange = false;
    _for(j, 0, keyword.count) {
        if (isSameString(keyword.keywords[j], "x0")) {
            container[0] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSameString(keyword.keywords[j], "y0")) {
            container[1] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSameString(keyword.keywords[j], "x1")) {
            container[2] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSameString(keyword.keywords[j], "y1")) {
            container[3] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSameString(keyword.keywords[j], "range")) {
            afterrange = true;
            // skip the range keyword
            j++;
        } else if (isSameString(keyword.keywords[j], "p1") && afterrange) {
            container[0] = strtod(keyword.keywords[j + 1], NULL);
            j++;
            container[1] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSameString(keyword.keywords[j], "p2") && afterrange) {
            container[2] = strtod(keyword.keywords[j + 1], NULL);
            j++;
            container[3] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (afterrange && keyword.findbracket) {
            // [x0 y0 x1 y1]
            j--;
            container[0] = strtod(keyword.keywords[j + 1], NULL);
            j++;
            container[1] = strtod(keyword.keywords[j + 1], NULL);
            j++;
            container[2] = strtod(keyword.keywords[j + 1], NULL);
            j++;
            container[3] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else {
            OpenDFNMessage::RuntimebackStringString("Error: unrecognised keyword ", keyword.keywords[j]);
            OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
        }
    }
}

/**
 * \brief Read one line from \p file and extract box/rectangle parameters into \p container.
 *
 * Parses a fresh line describing a rectangle, accepting named (xmin/xmax/ymin/ymax),
 * paired (x, y) or bracketed [xmin xmax ymin ymax] forms. Fills \p container[0..3].
 * Unrecognised tokens print an error and exit.
 *
 * \param file      Input stream; the current line is consumed.
 * \param container Output bounds array [xmin, xmax, ymin, ymax].
 *
 * Side effects: advances \p file by one line, mutates \p container, allocates and frees
 * a temporary Keywords buffer, may terminate the process on error.
 */
void GeometryKeywordParser::pushSquareKeywords(FILE* file, Real container[]) {
    Keywords keyword;
    parseStringinLine(file, keyword);
    _for(j, 0, keyword.count) {
        if (isSamePrefix(keyword.keywords[j], "xmin", 3)) {
            container[0] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSamePrefix(keyword.keywords[j], "xmax", 3)) {
            container[1] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSamePrefix(keyword.keywords[j], "ymin", 3)) {
            container[2] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSamePrefix(keyword.keywords[j], "ymax", 3)) {
            container[3] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSameString(keyword.keywords[j], "x")) {
            container[0] = strtod(keyword.keywords[j + 1], NULL);
            j++;
            container[1] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSameString(keyword.keywords[j], "y")) {
            container[2] = strtod(keyword.keywords[j + 1], NULL);
            j++;
            container[3] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (keyword.findbracket) {
            j--;
            // [xmin xmax ymin ymax]
            container[0] = strtod(keyword.keywords[j + 1], NULL);
            j++;
            container[1] = strtod(keyword.keywords[j + 1], NULL);
            j++;
            container[2] = strtod(keyword.keywords[j + 1], NULL);
            j++;
            container[3] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (keyword.keywords[j][0] == '-' || keyword.keywords[j][0] == '+' || keyword.keywords[j][0] == '.' ||
                   (keyword.keywords[j][0] >= '0' && keyword.keywords[j][0] <= '9')) {
            // Bare positional form (no keyword, no bracket): the four numbers on
            // the line are taken in order as xmin xmax ymin ymax. Used by the
            // canonical DFN examples, e.g. "odfn.geometry.domain -4 4 -4 4" and
            // "odfn.geometry.square 'rock_0' 0 2.85 0 3.47".
            container[0] = strtod(keyword.keywords[j], NULL);
            container[1] = strtod(keyword.keywords[j + 1], NULL);
            container[2] = strtod(keyword.keywords[j + 2], NULL);
            container[3] = strtod(keyword.keywords[j + 3], NULL);
            j += 3;
        } else {
            OpenDFNMessage::RuntimebackStringString("Error: unrecognised keyword ", keyword.keywords[j]);
            OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
        }
    }
    keyword.clean();
}

/**
 * \brief Read one line from \p file and extract circle geometry parameters into \p container.
 *
 * Accepts named x/y/radius/segments, the "center" pair, or the bracketed
 * [x y rad seg] form. Fills \p container[0..3] with centre, radius and segment count.
 * Unrecognised tokens print an error and exit.
 *
 * \param file      Input stream; the current line is consumed.
 * \param container Output array [centre.x, centre.y, radius, segments].
 *
 * Side effects: advances \p file by one line, mutates \p container, allocates and frees
 * a temporary Keywords buffer, may terminate the process on error.
 */
void GeometryKeywordParser::pushCircleGeometryKeywords(FILE* file, Real container[]) {
    Keywords keyword;
    parseStringinLine(file, keyword);
    _for(j, 0, keyword.count) {
        if (isSameString(keyword.keywords[j], "x")) {
            container[0] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSameString(keyword.keywords[j], "y")) {
            container[1] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSamePrefix(keyword.keywords[j], "radius", 3)) {
            container[2] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSamePrefix(keyword.keywords[j], "segments", 3)) {
            container[3] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSamePrefix(keyword.keywords[j], "center", 1)) {
            container[0] = strtod(keyword.keywords[j + 1], NULL);
            j++;
            container[1] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (keyword.findbracket) {
            j--;
            // [x y rad seg]
            container[0] = strtod(keyword.keywords[j + 1], NULL);
            j++;
            container[1] = strtod(keyword.keywords[j + 1], NULL);
            j++;
            container[2] = strtod(keyword.keywords[j + 1], NULL);
            j++;
            container[3] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else {
            OpenDFNMessage::RuntimebackStringString("Error: unrecognised keyword ", keyword.keywords[j]);
            OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
        }
    }
    keyword.clean();
}

/**
 * \brief Read one line from \p file and extract ellipse geometry parameters into \p container.
 *
 * Accepts named forms (x/y, maxradius/minradius, theta, segments), the "center" and
 * "radius" pairs, or the bracketed [x y minr maxr theta seg] form. Fills
 * \p container[0..5]. Note that in \p container the minor radius is stored at index 3
 * and the major radius at index 2. Unrecognised tokens print an error and exit.
 *
 * \param file      Input stream; the current line is consumed.
 * \param container Output array [centre.x, centre.y, maxradius, minradius, theta, segments].
 *
 * Side effects: advances \p file by one line, mutates \p container, allocates and frees
 * a temporary Keywords buffer, may terminate the process on error.
 */
void GeometryKeywordParser::pushEllipseGeometryKeywords(FILE* file, Real container[]) {
    Keywords keyword;
    parseStringinLine(file, keyword);

    _for(j, 0, keyword.count) {
        if (isSameString(keyword.keywords[j], "x")) {
            container[0] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSameString(keyword.keywords[j], "y")) {
            container[1] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSamePrefix(keyword.keywords[j], "maxradius", 3)) {
            container[2] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSamePrefix(keyword.keywords[j], "minradius", 3)) {
            container[3] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSamePrefix(keyword.keywords[j], "theta", 3)) {
            container[4] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSamePrefix(keyword.keywords[j], "segments", 3)) {
            container[5] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSamePrefix(keyword.keywords[j], "center", 2)) {
            // center (x,y)
            container[0] = strtod(keyword.keywords[j + 1], NULL);
            j++;
            container[1] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSamePrefix(keyword.keywords[j], "radius", 1)) {
            // radius (min,max)
            container[3] = strtod(keyword.keywords[j + 1], NULL);
            j++;
            container[2] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (keyword.findbracket) {
            j--;
            // [x y minr maxr theta seg]
            container[0] = strtod(keyword.keywords[j + 1], NULL);
            j++;
            container[1] = strtod(keyword.keywords[j + 1], NULL);
            j++;
            container[3] = strtod(keyword.keywords[j + 1], NULL);
            j++;
            container[2] = strtod(keyword.keywords[j + 1], NULL);
            j++;
            container[4] = strtod(keyword.keywords[j + 1], NULL);
            j++;
            container[5] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else {
            OpenDFNMessage::RuntimebackStringString("Error: unrecognised keyword ", keyword.keywords[j]);
            OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
        }
    }
    keyword.clean();
}

/**
 * \brief Read one line from \p file and extract arc geometry parameters into \p container.
 *
 * Accepts named forms (x/y, radius, theta1/theta2, segments), the "center" and "theta"
 * pairs, or the bracketed [x y radius theta1 theta2 seg] form. Fills \p container[0..5].
 * Unrecognised tokens print an error and exit.
 *
 * \param file      Input stream; the current line is consumed.
 * \param container Output array [centre.x, centre.y, radius, theta1, theta2, segments].
 *
 * Side effects: advances \p file by one line, mutates \p container, allocates and frees
 * a temporary Keywords buffer, may terminate the process on error.
 */
void GeometryKeywordParser::pushArcGeometryKeywords(FILE* file, Real container[]) {
    Keywords keyword;
    parseStringinLine(file, keyword);

    _for(j, 0, keyword.count) {
        if (isSameString(keyword.keywords[j], "x")) {
            container[0] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSameString(keyword.keywords[j], "y")) {
            container[1] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSamePrefix(keyword.keywords[j], "radius", 3)) {
            container[2] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSameString(keyword.keywords[j], "theta1")) {
            container[3] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSameString(keyword.keywords[j], "theta2")) {
            container[4] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSamePrefix(keyword.keywords[j], "segments", 3)) {
            container[5] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSamePrefix(keyword.keywords[j], "center", 2)) {
            // center (x,y)
            container[0] = strtod(keyword.keywords[j + 1], NULL);
            j++;
            container[1] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSamePrefix(keyword.keywords[j], "theta", 1)) {
            // radius (min,max)
            container[3] = strtod(keyword.keywords[j + 1], NULL);
            j++;
            container[4] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (keyword.findbracket) {
            j--;
            // [x y minr maxr theta seg]
            container[0] = strtod(keyword.keywords[j + 1], NULL);
            j++;
            container[1] = strtod(keyword.keywords[j + 1], NULL);
            j++;
            container[2] = strtod(keyword.keywords[j + 1], NULL);
            j++;
            container[3] = strtod(keyword.keywords[j + 1], NULL);
            j++;
            container[4] = strtod(keyword.keywords[j + 1], NULL);
            j++;
            container[5] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else {
            OpenDFNMessage::RuntimebackStringString("Error: unrecognised keyword ", keyword.keywords[j]);
            OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
        }
    }
    keyword.clean();
}

/**
 * \brief Read one line from \p file and extract plane (two-point line) parameters into \p container.
 *
 * Accepts named x0/y0/x1/y1, the "p1"/"p2" point pairs, or the bracketed
 * [x0 y0 x1 y1] form. Fills \p container[0..3] with the two endpoints.
 * Unrecognised tokens print an error and exit.
 *
 * \param file      Input stream; the current line is consumed.
 * \param container Output array [x0, y0, x1, y1].
 *
 * Side effects: advances \p file by one line, mutates \p container, allocates and frees
 * a temporary Keywords buffer, may terminate the process on error.
 */
void GeometryKeywordParser::pushPlaneKeywords(FILE* file, Real container[]) {
    Keywords keyword;
    parseStringinLine(file, keyword);

    _for(j, 0, keyword.count) {
        if (isSameString(keyword.keywords[j], "x0")) {
            container[0] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSameString(keyword.keywords[j], "y0")) {
            container[1] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSameString(keyword.keywords[j], "x1")) {
            container[2] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSameString(keyword.keywords[j], "y1")) {
            container[3] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSameString(keyword.keywords[j], "p1")) {
            container[0] = strtod(keyword.keywords[j + 1], NULL);
            j++;
            container[1] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSameString(keyword.keywords[j], "p2")) {
            container[2] = strtod(keyword.keywords[j + 1], NULL);
            j++;
            container[3] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (keyword.findbracket) {
            j--;
            // [x0 y0 x1 y1]
            container[0] = strtod(keyword.keywords[j + 1], NULL);
            j++;
            container[1] = strtod(keyword.keywords[j + 1], NULL);
            j++;
            container[2] = strtod(keyword.keywords[j + 1], NULL);
            j++;
            container[3] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else {
            OpenDFNMessage::RuntimebackStringString("Error: unrecognised keyword ", keyword.keywords[j]);
            OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
        }
    }
    keyword.clean();
}

/**
 * \brief Read one line from \p file and extract plane parameters plus a segment count.
 *
 * Like the plain overload but also parses a "segment" keyword. Accepts named
 * x0/y0/x1/y1, the "start"/"end" point pairs, or the bracketed [x0 y0 x1 y1] form.
 * Fills \p container[0..3] and \p segment. Unrecognised tokens print an error and exit.
 *
 * \param file      Input stream; the current line is consumed.
 * \param container Output array [x0, y0, x1, y1].
 * \param segment   Output number of segments along the line.
 *
 * Side effects: advances \p file by one line, mutates \p container and \p segment,
 * allocates and frees a temporary Keywords buffer, may terminate the process on error.
 */
void GeometryKeywordParser::pushPlaneKeywords(FILE* file, Real container[], Int& segment) {
    Keywords keyword;
    parseStringinLine(file, keyword);

    _for(j, 0, keyword.count) {
        if (isSameString(keyword.keywords[j], "x0")) {
            container[0] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSameString(keyword.keywords[j], "y0")) {
            container[1] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSameString(keyword.keywords[j], "x1")) {
            container[2] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSameString(keyword.keywords[j], "y1")) {
            container[3] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSameString(keyword.keywords[j], "segment")) {
            segment = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSameString(keyword.keywords[j], "start")) {
            container[0] = strtod(keyword.keywords[j + 1], NULL);
            j++;
            container[1] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSameString(keyword.keywords[j], "end")) {
            container[2] = strtod(keyword.keywords[j + 1], NULL);
            j++;
            container[3] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (keyword.findbracket) {
            j--;
            // [x0 y0 x1 y1]
            container[0] = strtod(keyword.keywords[j + 1], NULL);
            j++;
            container[1] = strtod(keyword.keywords[j + 1], NULL);
            j++;
            container[2] = strtod(keyword.keywords[j + 1], NULL);
            j++;
            container[3] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else {
            OpenDFNMessage::RuntimebackStringString("Error: unrecognised keyword ", keyword.keywords[j]);
            OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
        }
    }
    keyword.clean();
}

/**
 * \brief Parse a polygon range definition, reading its vertices into \p polygon.
 *
 * The polygon syntax is "tag range polygon [ ] size N x0 y0 x1 y1 ...". The vertex
 * count follows the "size" keyword; if it is missing the program exits. Because the
 * vertices may span several input lines, this reads additional lines from \p file and
 * appends their tokens to \p keyword until enough tokens are available, then pushes the
 * N (x, y) pairs into \p polygon. Unrecognised tokens print an error and exit.
 *
 * \param file    Input stream; extra lines are consumed until all vertices are read.
 * \param keyword Keyword tokens for the current group line; grown in place with new tokens.
 * \param polygon Output list that receives the polygon vertices.
 *
 * Side effects: advances \p file, mutates \p keyword and \p polygon, allocates keyword
 * token memory, may terminate the process on error.
 */
void GeometryKeywordParser::pushPolygonGeometryKeywords(FILE* file, Keywords& keyword, std::vector<Vector2>& polygon) {
    Keywords TEMP;
    Int      size;
    bool     findSize = false;
    // first locate the "size" token to learn the vertex count
    _for(j, 0, keyword.count) {
        if (isSameString(keyword.keywords[j], "size")) {
            size     = strtod(keyword.keywords[j + 1], NULL);
            findSize = true;
            break;
        }
    }
    if (!findSize) {
        OpenDFNMessage::RuntimeInfo("Error: not find polygon size.");
        OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
    }
    // polygon should be: tag range polygon cell [ ] size N [] [] []
    while (keyword.count < 2 * (size + 2)) {
        // parse if not on the same line
        parseStringinLine(file, TEMP);
        // put the temp container to main keyword
        _for(j, 0, TEMP.count) {
            buildmemory(&keyword.keywords[keyword.count], MAX_KEYWORD_LENGTH);
            copyChars(keyword.keywords[keyword.count], TEMP.keywords[j]);
            if (TEMP.findbracket)
                keyword.findbracket = true;
            keyword.count++;
        }
        TEMP.clean();
    }
    // parsing the polygon
    _for(j, 0, keyword.count) {
        if (isSameString(keyword.keywords[j], "range") || isSameString(keyword.keywords[j], "method")) {
            j++;
        } else if (isSameString(keyword.keywords[j], "size")) {
            j++;
        } else if (keyword.findbracket) {
            j--;
            _for(in, 0, size) {
                polygon.push_back(Vector2(strtod(keyword.keywords[j + 1], NULL), strtod(keyword.keywords[j + 2], NULL)));
                j = j + 2;
            }
        } else {
            OpenDFNMessage::RuntimebackStringString("Error: unrecognised keyword ", keyword.keywords[j]);
            OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
        }
    }
}

/**
 * \brief Parse a polygon range definition with a cell resolution, reading vertices into \p polygon.
 *
 * Like the base polygon parser but also reads a "cell" pair into \p cell. The syntax is
 * "tag range polygon cell [ ] size N x0 y0 ...". Reads extra lines from \p file until all
 * vertices are available, then pushes the N (x, y) pairs into \p polygon. Unrecognised
 * tokens print an error and exit.
 *
 * \param file    Input stream; extra lines are consumed until all vertices are read.
 * \param keyword Keyword tokens for the current group line; grown in place with new tokens.
 * \param polygon Output list that receives the polygon vertices.
 * \param cell    Output per-axis cell counts [xcell, ycell].
 *
 * Side effects: advances \p file, mutates \p keyword, \p polygon and \p cell, allocates
 * keyword token memory, may terminate the process on error.
 */
void GeometryKeywordParser::pushPolygonGeometryKeywords(FILE* file, Keywords& keyword, std::vector<Vector2>& polygon, Int cell[]) {
    Keywords TEMP;
    Int      size;
    bool     findSize = false;
    _for(j, 0, keyword.count) {
        if (isSameString(keyword.keywords[j], "size")) {
            size     = strtod(keyword.keywords[j + 1], NULL);
            findSize = true;
            break;
        }
    }
    if (!findSize) {
        OpenDFNMessage::RuntimeInfo("Error: not find polygon size.");
        OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
    }
    // polygon should be: tag range polygon cell [ ] size N [] [] []
    while (keyword.count < 2 * (size + 2) + 3) {
        // parse if not on the same line
        parseStringinLine(file, TEMP);
        // put the temp container to main keyword
        _for(j, 0, TEMP.count) {
            buildmemory(&keyword.keywords[keyword.count], MAX_KEYWORD_LENGTH);
            copyChars(keyword.keywords[keyword.count], TEMP.keywords[j]);
            keyword.count++;
        }
        TEMP.clean();
    }
    // parsing the polygon
    _for(j, 0, keyword.count) {
        if (isSameString(keyword.keywords[j], "range") || isSameString(keyword.keywords[j], "method")) {
            j++;
        } else if (isSameString(keyword.keywords[j], "size")) {
            j++;
        } else if (isSameString(keyword.keywords[j], "cell")) {
            cell[0] = strtod(keyword.keywords[j + 1], NULL);
            j++;
            cell[1] = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (keyword.findbracket) {
            j--;
            _for(in, 0, size) {
                polygon.push_back(Vector2(strtod(keyword.keywords[j + 1], NULL), strtod(keyword.keywords[j + 2], NULL)));
                j = j + 2;
            }
        } else {
            OpenDFNMessage::RuntimebackStringString("Error: unrecognised keyword ", keyword.keywords[j]);
            OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
        }
    }
}
}  // namespace ns_mesh
