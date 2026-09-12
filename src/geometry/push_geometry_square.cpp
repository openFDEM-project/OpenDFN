/******************************************************************************
 * \file      push_geometry_square.cpp
 * \brief     Basic classes parser mesh fron selfdeveloped format parameters.
 *
 * \details   Implementation of basic classes parser mesh fron selfdeveloped
 *            format parameters.
 *
 * \author    Xiaofeng Li
 * \email     xfli@whrsm.ac.cn
 * \date      Created:       June 24, 2020
 * \date      Last modified: 2026-07-20 00:06:23
 * \version   OpenDFN Version 4.40 (managed by CMake macro PRG_VERSION,
 *            generated into common/opendfn_config.h from opendfn_config.h.in)
 *
 * \see OpenDFN Project Website: https://xiaofengli-uoft.github.io/Mainpage/
 * \see Geomechanics Group Website: https://geogroup.utoronto.ca/
 *
 * The OpenDFN Project is maintained by the OpenDFN Foundation.
 *
 * Copyright (C) 2017-2026 Xiaofeng Li. OpenDFN Contributors.
 *
 * OpenDFN is free for educational, research and non-profit purposes.
 * Any commerical or military use should be authorised by the developer.
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
#include "mesh/group/geometry_keyword_reader.h"

namespace ns_geometry {
/**
 * \brief Create a rectangular (axis-aligned) geometry primitive from four
 *        corner bounds and add it to the geometry as a surface or a cut tool.
 *
 * Parses the new tool tag (and, for CUT/REMOVE operations, the target object
 * tag) from the input stream, then reads the square keywords (x_min, x_max,
 * y_min, y_max) via GeometryKeywordReader::pushSquareKeywords. Four corner nodes are
 * created counter-clockwise and joined by four closing lines. When the
 * operation is CUT the lines are marked embedded/reserved and registered into
 * the target surface boolean group; otherwise a closed curve and a surface are
 * built from the four lines, and for REMOVE the surface is added to the removal
 * group so it excavates the matching object.
 *
 * \param general    Runtime context; its \c inputfile stream is advanced while
 *                   reading the tags and square bounds.
 * \param geometry   Geometry container that is mutated: node, line, curve,
 *                   surface and related group arrays are grown and the
 *                   \c *_num counters are updated.
 * \param embedType  Operation mode controlling the output: ENTIRE (plain
 *                   surface), REMOVE (excavation surface), or CUT (embedded
 *                   cutting lines only).
 * \return void
 *
 * \note Side effects: allocates geometry memory via buildmemory and mutates the
 *       shared \p geometry object; performs no direct file or console I/O.
 */
void GeometryBuilder::push_geometry_square(General general, Geometry geometry, OperationType embedType) {
    int n_node    = geometry->node_num;
    int n_line    = geometry->line_num;
    int n_curve   = geometry->curve_num;
    int n_surface = geometry->surface_num;

    char newToolTag[MAXARGC], ObjectTag[MAXARGC];
    parseStringinQuotation(general->inputfile, newToolTag);
    if ((embedType == OperationType::CUT) || (embedType == OperationType::REMOVE)) {
        parseStringinQuotation(general->inputfile, ObjectTag);
    }

    Real value, x_min, x_max, y_min, y_max;
    Real container[4] = {0.0, 0.0, 0.0, 0.0};
    ns_mesh::GeometryKeywordReader::pushSquareKeywords(general->inputfile, container);
    x_min = container[0];
    x_max = container[1];
    y_min = container[2];
    y_max = container[3];

    /* add geometry nodes */
    for (int i = 0; i < 4; i++) {
        // malloc node
        buildmemory(&geometry->node, (n_node + 1), n_node);
        if ((i == 0) || (i == 3))
            value = x_min;
        else
            value = x_max;
        geometry->node[n_node].x = value;

        if ((i == 0) || (i == 1))
            value = y_min;
        else
            value = y_max;

        geometry->node[n_node].y = value;
        getGeoNodeGroupbyCoord(geometry, newToolTag, geometry->node[n_node].x, geometry->node[n_node].y);
        n_node++;
    }

    /* add geometry lines */
    for (int i = 0; i < 4; i++) {
        buildmemory(&geometry->line, (n_line + 1), n_line);
        // copy the tag
        copyChars(geometry->line[n_line].tag, newToolTag);
        // assign the nodes
        geometry->line[n_line].node_1 = geometry->node_num + i;

        if (i == 3)
            geometry->line[n_line].node_2 = geometry->node_num;
        else
            geometry->line[n_line].node_2 = geometry->node_num + i + 1;
        // assign the element flag,  it is not truss
        geometry->line[n_line].iselement = false;
        if (embedType == OperationType::CUT) {
            geometry->line[n_line].isembeded    = true;
            geometry->line[n_line].tobeReserved = true;
            // add to bool groups for cut
            addLinetoSufaceBoolGroups(geometry, ObjectTag, n_line);
        }
        // update the line count
        n_line++;
    }
    if (embedType != OperationType::CUT) {
        // add curves
        buildmemory(&geometry->curve, (n_curve + 1), n_curve);
        // copy the tag
        copyChars(geometry->curve[n_curve].tag, newToolTag);
        // assign the line count
        geometry->curve[n_curve].n_line = 4;
        // assign lines
        buildmemory(&geometry->curve[n_curve].line, 4);
        for (int i = 0; i < 4; i++) {
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
    //addtoLinetoNodeGroups(geometry, newToolTag);
    // build for line groups
    if (embedType == OperationType::CUT) {
        addtoLineGroups(geometry, newToolTag);
    }
}
}  // namespace ns_geometry