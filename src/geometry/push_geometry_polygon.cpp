/******************************************************************************
 * \file      push_geometry_polygon.cpp
 * \brief     Basic classes parser mesh fron selfdeveloped format parameters.
 *
 * \details   Implementation of basic classes parser mesh fron selfdeveloped
 *            format parameters.
 *
 * \author    Xiaofeng Li
 * \email     xfli@whrsm.ac.cn
 * \date      Created:       June 24, 2020
 * \date      Last modified: 2026-07-20 00:07:25
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

#include "geometry/opendfn_geometry_module.h"
namespace ns_geometry {
/**
 * @brief Parse a "polygon" geometry command and build the corresponding nodes,
 *        lines, curve loop, and surface according to the requested operation.
 *
 * The command syntax is a quoted new tag, an optional quoted target object tag
 * (only for CUT and REMOVE operations), the "size" keyword followed by an
 * integer N, and then 2*N floating point values forming N (x/y) vertices. Input
 * is tokenized line by line until 2*N coordinate tokens are collected, skipping
 * NUL/space-only lines.
 *
 * Nodes are appended for each vertex and registered to the named node group. N
 * lines are then created connecting consecutive vertices (the last line closing
 * back to the first). For a CUT operation the lines are flagged as embedded and
 * reserved and added to the target surface's boolean-cut groups. For
 * non-CUT operations a closed curve is built from the N lines and a new surface
 * is created from that curve; a REMOVE operation additionally registers the
 * surface with the target object's remove groups.
 *
 * Side effects:
 *  - Reads and advances the input file stream (general->inputfile).
 *  - Mutates geometry state: grows geometry->node, geometry->line, and (for
 *    non-CUT) geometry->curve and geometry->surface; updates node_num, line_num,
 *    curve_num, and surface_num; and registers node/line/boolean groups.
 *  - Emits runtime messages and may call OPENDFN_EXIT on malformed input
 *    (missing "size" keyword or wrong node count).
 *
 * @param general   Global context providing the input file stream to parse.
 * @param geometry  Geometry container that is mutated with the new nodes, lines,
 *                  curves, surfaces, and groups.
 * @param embedType Operation type controlling how the polygon is applied:
 *                  ENTIRE (standalone surface), REMOVE (subtract from target),
 *                  or CUT (embed lines into a target surface for boolean cut).
 * @return void
 */
void GeometryBuilder::push_geometry_polygon(General general, Geometry geometry, OperationType embedType) {
    int n_node    = geometry->node_num;
    int n_line    = geometry->line_num;
    int n_curve   = geometry->curve_num;
    int n_surface = geometry->surface_num;

    char newToolTag[MAXARGC], ObjectTag[MAXARGC];
    parseStringinQuotation(general->inputfile, newToolTag);
    if ((embedType == OperationType::CUT) || (embedType == OperationType::REMOVE)) {
        parseStringinQuotation(general->inputfile, ObjectTag);
    }

    Real value;
    int  N;
    char size[MAXARGC];
    parseStringinQuotation(general->inputfile, size);
    // add the size keyword
    if (isSameString(size, "size")) {
        getIntValue(general->inputfile, &N);
    } else {
        OpenDFNMessage::RuntimeInfo("Error: not find polygon size.");
        OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
    }

    Keywords keyword;
    Keywords TEMP;

    while (keyword.count < 2 * N) {
        parseStringinLine(general->inputfile, TEMP);
        // put the temp container to main keyword
        // NUT or white space
        if ((int)TEMP.keywords[0] == 0 || (int)TEMP.keywords[0] == 32) {
            fgetc(general->inputfile);
            continue;
        }
        _for(j, 0, TEMP.count) {
            buildmemory(&keyword.keywords[keyword.count], MAX_KEYWORD_LENGTH);
            copyChars(keyword.keywords[keyword.count], TEMP.keywords[j]);
            keyword.count++;
        }
        if (TEMP.count == 0) {
            OpenDFNMessage::RuntimeString("Warning: the line of polygon point is not read.");
            // OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
        }
        TEMP.clean();
    }
    if (keyword.count != 2 * N) {
        OpenDFNMessage::RuntimeStringInt("Error: the count of input nodes is not ", N);
        OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
    }
    /* add geometry node */
    for (int i = 0; i < N; i++) {
        // malloc node
        buildmemory(&geometry->node, (n_node + 1), n_node);
        geometry->node[n_node].x = strtod(keyword.keywords[2 * i], NULL);
        geometry->node[n_node].y = strtod(keyword.keywords[2 * i + 1], NULL);
        getGeoNodeGroupbyCoord(geometry, newToolTag, geometry->node[n_node].x, geometry->node[n_node].y);
        n_node++;
    }
    keyword.clean();

    /* add geometry line */
    for (int i = 0; i < N; i++) {
        buildmemory(&geometry->line, (n_line + 1), n_line);
        // copy the tag
        copyChars(geometry->line[n_line].tag, newToolTag);

        geometry->line[n_line].node_1 = geometry->node_num + i;

        geometry->line[n_line].iselement = false;
        if (embedType == OperationType::CUT) {
            geometry->line[n_line].isembeded    = true;
            geometry->line[n_line].tobeReserved = true;
            // add to bool groups for cut
            addLinetoSufaceBoolGroups(geometry, ObjectTag, n_line);
        }
        if (i == (N - 1))
            geometry->line[n_line].node_2 = geometry->node_num;
        else
            geometry->line[n_line].node_2 = geometry->node_num + i + 1;
        n_line++;
    }
    if (embedType != OperationType::CUT) {
        // add curves
        buildmemory(&geometry->curve, (n_curve + 1), n_curve);
        // copy the tag
        copyChars(geometry->curve[n_curve].tag, newToolTag);
        // assign the line count
        geometry->curve[n_curve].n_line = N;
        // assign lines
        buildmemory(&geometry->curve[n_curve].line, N);
        for (int i = 0; i < N; i++) {
            geometry->curve[n_curve].line[i] = geometry->line_num + i;
        }
        n_curve++;
        // update curve
        geometry->curve_num = n_curve;

        // create the new surface
        buildmemory(&geometry->surface, (n_surface + 1), n_surface);
        copyChars(geometry->surface[n_surface].tag, newToolTag);
        geometry->surface[n_surface].n_curve = 1;
        buildmemory(&geometry->surface[n_surface].curve, 1);
        // assing the curve id
        geometry->surface[n_surface].curve[0] = geometry->curve_num - 1;
        switch (embedType) {
            case OperationType::ENTIRE: break;
            case OperationType::REMOVE: {
                // to find the object
                addLinetoSufaceRemoveGroups(geometry, ObjectTag, geometry->surface_num);
            } break;
            case OperationType::CUT: break;
            default: break;
        }
        // update surface
        n_surface++;
        geometry->surface_num = n_surface;
    }
    /* update to geometry */
    geometry->node_num = n_node;
    geometry->line_num = n_line;
    // add to node groups
    // addtoLinetoNodeGroups(geometry, newToolTag);
    // build for line groups
    if (embedType == OperationType::CUT) {
        addtoLineGroups(geometry, newToolTag);
    }
}

}  // namespace ns_geometry
