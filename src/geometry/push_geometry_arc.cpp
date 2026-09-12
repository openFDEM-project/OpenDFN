/******************************************************************************
 * \file      push_geometry_arc.cpp
 * \brief     Basic classes parser mesh fron selfdeveloped format parameters.
 *
 * \details   Implementation of basic classes parser mesh fron selfdeveloped
 *            format parameters.
 *
 * \author    Xiaofeng Li
 * \email     xfli@whrsm.ac.cn
 * \date      Created:       June 24, 2020
 * \date      Last modified: 2026-07-20 00:04:13
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
 * \brief Parse an arc definition from the input file and append its discretized
 *        nodes and line segments to the geometry.
 *
 * Reads two quoted tags (the new tool tag and the target object tag) followed by
 * the arc keyword block (center x0/y0, radius, start/end angles theta_1/theta_2
 * in degrees, and segment count N). The arc is sampled into N+1 nodes and N line
 * segments spanning theta_1 to theta_2.
 *
 * Side effects: mutates \p geometry by growing its node and line arrays
 * (via buildmemory), incrementing node_num/line_num, updating min_size to the
 * smallest segment arc length, marking generated lines as embedded/reserved and
 * non-element, and registering the lines into surface bool, node, and line
 * groups. Also advances the input file cursor of \p general.
 *
 * \param general  Runtime context providing the input file stream being parsed.
 * \param geometry Geometry container that receives the generated nodes/lines.
 * \return void.
 */
void GeometryBuilder::push_geometry_arc(General general, Geometry geometry) {
    int n_node = geometry->node_num;
    // get the tags
    char newToolTag[MAXARGC], ObjectTag[MAXARGC];
    parseStringinQuotation(general->inputfile, newToolTag);
    parseStringinQuotation(general->inputfile, ObjectTag);

    Real value;
    Int  N;
    Real x0, y0, radius, theta, theta_1, theta_2;
    Real container[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    ns_mesh::GeometryKeywordReader::pushArcGeometryKeywords(general->inputfile, container);
    x0      = container[0];
    y0      = container[1];
    radius  = container[2];
    theta_1 = container[3];
    theta_2 = container[4];
    N       = container[5];

    /* obtain the arc size to avoid being deleted */
    if (geometry->min_size == 0.0)
        geometry->min_size = 2.0 * OpenDFN_PI * radius * fabs(theta_2 - theta_1) / 360.0 / N;
    else
        geometry->min_size = OpenDFN_min(geometry->min_size, 2.0 * OpenDFN_PI * radius * fabs(theta_2 - theta_1) / 360.0 / N);

    /* add geometry node */
    for (int i = 0; i < N + 1; i++) {
        // malloc node
        buildmemory(&geometry->node, (geometry->node_num + 1), geometry->node_num);
        theta                                = theta_1 + ((double)i / (double)N) * (theta_2 - theta_1) / 360.0 * 2 * OpenDFN_PI;
        geometry->node[geometry->node_num].x = x0 + radius * cos(theta);
        geometry->node[geometry->node_num].y = y0 + radius * sin(theta);

        geometry->node_num++;
    }

    /* add geometry line */
    for (int i = 0; i < N ; i++) {
        // malloc line
        buildmemory(&geometry->line, (geometry->line_num + 1), geometry->line_num);
        // copy the tag
        copyChars(geometry->line[geometry->line_num].tag, newToolTag);
        // fill the nodes
        geometry->line[geometry->line_num].node_1 = n_node + i;
        geometry->line[geometry->line_num].node_2 = n_node + i + 1;
        // update the flags
        geometry->line[geometry->line_num].isembeded = true;
        geometry->line[geometry->line_num].tobeReserved = true;
        geometry->line[geometry->line_num].iselement = false;

        // add to bool groups for cut
        addLinetoSufaceBoolGroups(geometry, ObjectTag, geometry->line_num);
        geometry->line_num++;
    }

        // add to node groups
    addtoLinetoNodeGroups(geometry, newToolTag);
    // build for line groups
    addtoLineGroups(geometry, newToolTag);
}
}  // namespace ns_geometry