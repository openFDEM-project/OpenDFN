/******************************************************************************
 * \file      push_geometry_points.cpp
 * \brief     Basic classes parser mesh fron selfdeveloped format parameters.
 *
 * \details   Implementation of basic classes parser mesh fron selfdeveloped
 *            format parameters.
 *
 * \author    Xiaofeng Li
 * \email     xfli@whrsm.ac.cn
 * \date      Created:       June 24, 2020
 * \date      Last modified: 2026-07-20 00:04:25
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
 * @brief Parse a "point" geometry command from the input file and insert the
 *        parsed coordinates as standalone geometry nodes.
 *
 * The command syntax read here is a quoted group tag, a quoted target surface
 * tag, the "size" keyword followed by an integer N, and then 2*N floating point
 * values (x/y pairs). The routine tokenizes the input line by line until 2*N
 * coordinate tokens have been collected, skipping NUL/space-only lines.
 *
 * For every parsed point the function allocates a new geometry node, stores its
 * coordinates, and registers the point as a fragmentation tool on the matching
 * target surface (dimension 0 = point/line fragment tool). Each point is also
 * added to the named node group, whose isInsertedPoints flag is set to true.
 *
 * Side effects:
 *  - Reads and advances the input file stream (general->inputfile).
 *  - Mutates geometry state: grows geometry->node, updates geometry->node_num,
 *    appends fragmenttool entries on matching surfaces, and creates/updates node
 *    groups via getGeoNodeGroupbyCoord.
 *  - Emits runtime messages and may call OPENDFN_EXIT on malformed input
 *    (missing "size" keyword or wrong node count).
 *
 * @param general  Global context providing the input file stream to parse.
 * @param geometry Geometry container that is mutated with the new nodes, surface
 *                 fragmentation tools, and node groups.
 * @return void
 */
void GeometryBuilder::push_geometry_point(General general, Geometry geometry) {
    int n_node = geometry->node_num;

    char newToolTag[MAXARGC], ObjectTag[MAXARGC];
    // group tag
    parseStringinQuotation(general->inputfile, newToolTag);
    // surface tag to be fragment
    parseStringinQuotation(general->inputfile, ObjectTag);

    Real value;
    int  N;
    char size[MAXARGC];
    parseStringinQuotation(general->inputfile, size);
    // add the size keyword
    if (isSameString(size, "size")) {
        getIntValue(general->inputfile, &N);
    } else {
        OpenDFNMessage::RuntimeInfo("Error: not find point size.");
        OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
    }

    Keywords keyword;
    Keywords TEMP;

    while (keyword.count < 2 * N) {
        parseStringinLine(general->inputfile, TEMP);

        if ((int)TEMP.keywords[0] == 0 || (int)TEMP.keywords[0] == 32) {
            fgetc(general->inputfile);
            continue;
        }

        // put the temp container to main keyword
        _for(j, 0, TEMP.count) {
            buildmemory(&keyword.keywords[keyword.count], MAX_KEYWORD_LENGTH);
            copyChars(keyword.keywords[keyword.count], TEMP.keywords[j]);
            keyword.count++;
        }
        if (TEMP.count == 0) {
            OpenDFNMessage::RuntimeString("Warning: the point is not read.");
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

        bool findsurface = false;
        for (int j = 0; j < geometry->surface_num; j++) {
            if (isSameString(geometry->surface[j].tag, ObjectTag)) {
                // add new bool tag
                buildmemory(&geometry->surface[j].fragmenttool, (2 * geometry->surface[j].n_fragmenttool + 2),
                            2 * geometry->surface[j].n_fragmenttool);
                // dimesion is line
                geometry->surface[j].fragmenttool[2 * geometry->surface[j].n_fragmenttool] = 0;
                // add tag
                geometry->surface[j].fragmenttool[2 * geometry->surface[j].n_fragmenttool + 1] = n_node + 1;
                // update count
                geometry->surface[j].n_fragmenttool++;
                // mark the bool
                findsurface = true;
                break;
            }
        }

        // add to node group
        getGeoNodeGroupbyCoord(geometry, newToolTag, geometry->node[n_node].x, geometry->node[n_node].y);
        geometry->group[geometry->group_num - 1].isInsertedPoints = true;
        n_node++;
    }
    keyword.clean();
    /* update to geometry */
    geometry->node_num = n_node;
}
}  // namespace ns_geometry
