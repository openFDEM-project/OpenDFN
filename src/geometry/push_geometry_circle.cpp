/******************************************************************************
 * \file      push_geometry_circle.cpp
 * \brief     Basic classes parser mesh fron selfdeveloped format parameters.
 *
 * \details   Implementation of basic classes parser mesh fron selfdeveloped
 *            format parameters.
 *
 * \author    Xiaofeng Li
 * \email     xfli@whrsm.ac.cn
 * \date      Created:       June 24, 2020
 * \date      Last modified: 2026-07-20 00:04:27
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

#include <math.h>
#include "common/opendfn_math.h"
#include "geometry/opendfn_geometry_module.h"
#include "mesh/group/geometry_keyword_reader.h"

namespace ns_geometry {
/**
 * \brief Parse a circle definition from the input file and append the resulting
 *        nodes, lines, and (optionally) a closed curve/surface to the geometry.
 *
 * Reads the new object tag (always) and, for CUT/REMOVE operations, an object
 * tag identifying the target surface, followed by the circle keyword block
 * (center x0/y0, radius, and segment count N). The circle is discretized into N
 * nodes and N closed line segments. When \p embedType is not CUT, a curve
 * referencing the N lines and a surface referencing that curve are also created;
 * REMOVE additionally registers the surface into the remove groups.
 *
 * Side effects: mutates \p geometry by growing its node/line/curve/surface arrays
 * (via buildmemory) and updating node_num/line_num/curve_num/surface_num, sets
 * per-line flags (iselement/isembeded/tobeReserved) according to \p embedType,
 * updates min_size to the smallest segment arc length, assigns node group
 * membership by coordinate, and registers lines into bool/remove/line groups.
 * Also advances the input file cursor of \p general.
 *
 * \param general   Runtime context providing the input file stream being parsed.
 * \param geometry  Geometry container that receives the generated entities.
 * \param embedType Boolean operation mode (ENTIRE, REMOVE, CUT) controlling
 *                  whether a surface is built and how the lines are tagged.
 * \return void.
 */
void GeometryBuilder::push_geometry_circle(General general, Geometry geometry, OperationType embedType) {
    int n_node    = geometry->node_num;
    int n_line    = geometry->line_num;
    int n_curve   = geometry->curve_num;
    int n_surface = geometry->surface_num;

    char newToolTag[MAXARGC], ObjectTag[MAXARGC];
    // get the tag of new object
    parseStringinQuotation(general->inputfile, newToolTag);
    // get the tag of tool surface
    if ((embedType == OperationType::CUT) || (embedType == OperationType::REMOVE)) {
        parseStringinQuotation(general->inputfile, ObjectTag);
    }

    Int  N = 20;
    Real x0, y0, radius, theta;
    Real container[4] = {0.0, 0.0, 0.0, 0.0};
    ns_mesh::GeometryKeywordReader::pushCircleGeometryKeywords(general->inputfile, container);
    x0     = container[0];
    y0     = container[1];
    radius = container[2];
    N      = container[3];

    /* obtain the arc size to avoid being deleted */
    if (geometry->min_size == 0.0)
        geometry->min_size = 2.0 * OpenDFN_PI * radius / N;
    else
        geometry->min_size = OpenDFN_min(geometry->min_size, 2.0 * OpenDFN_PI * radius / N);

    /* add geometry node */
    for (int i = 0; i < N; i++) {
        // malloc node
        buildmemory(&geometry->node, (n_node + 1), n_node);
        // compute theta
        theta = ((double)i / (double)N) * 2 * OpenDFN_PI;
        // get the new coords
        geometry->node[n_node].x = x0 + radius * cos(theta);
        geometry->node[n_node].y = y0 + radius * sin(theta);
        getGeoNodeGroupbyCoord(geometry, newToolTag, geometry->node[n_node].x, geometry->node[n_node].y);
        n_node++;
    }

    /* add geometry line */
    for (int i = 0; i < N; i++) {
        buildmemory(&geometry->line, (n_line + 1), n_line);
        // copy the tag
        copyChars(geometry->line[n_line].tag, newToolTag);
        // assign the element flag,  it is not truss
        geometry->line[n_line].iselement = false;
        if (embedType == OperationType::CUT) {
            geometry->line[n_line].isembeded    = true;
            geometry->line[n_line].tobeReserved = true;
            // add to bool groups for cut
            addLinetoSufaceBoolGroups(geometry, ObjectTag, n_line);
        }
        geometry->line[n_line].node_1 = geometry->node_num + i;

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
    //addtoLinetoNodeGroups(geometry, newToolTag);

    // build for line groups
    if (embedType == OperationType::CUT) {
        addtoLineGroups(geometry, newToolTag);
    }
}
}  // namespace ns_geometry