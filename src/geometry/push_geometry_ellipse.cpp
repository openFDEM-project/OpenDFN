/******************************************************************************
 * \file      push_geometry_ellipse.cpp
 * \brief     Basic classes parser mesh fron selfdeveloped format parameters.
 *
 * \details   Implementation of basic classes parser mesh fron selfdeveloped
 *            format parameters.
 *
 * \author    Xiaofeng Li
 * \email     xfli@whrsm.ac.cn
 * \date      Created:       June 24, 2020
 * \date      Last modified: 2026-07-20 00:07:42
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
 * @brief Parse an ellipse definition and add it to the geometry as a polyline
 *        approximation, optionally forming a closed surface.
 *
 * Reads the tool tag and, for CUT or REMOVE operations, the target object tag from
 * @p general->inputfile, followed by the ellipse parameters
 * {x0, y0, radius_min, radius_max, theta_0, N} where (x0, y0) is the centre,
 * radius_min/radius_max are the semi-axes, theta_0 is the rotation angle, and N is
 * the number of discretisation segments. The routine records the minimum arc size
 * used to guard against oversized element deletion, then generates N nodes around
 * the (rotated) ellipse and N lines connecting them into a closed loop. The
 * behaviour then branches on @p embedType:
 *   - CUT:    lines are flagged embedded and reserved and added to surface
 *             boolean-cut groups; no curve or surface is created.
 *   - ENTIRE / REMOVE (and other non-CUT types): a curve and a surface are built
 *             from the loop; REMOVE additionally registers the surface with the
 *             boolean-remove groups.
 *
 * Side effects: mutates @p geometry (allocates/appends nodes, lines, and possibly a
 * curve and surface; updates counters; assigns node-group membership by coordinate);
 * registers lines/surfaces with boolean-cut or boolean-remove groups; for
 * CUT/REMOVE also registers line groups; advances the input-file parse cursor.
 *
 * @param general   Global context providing the input-file stream to parse.
 * @param geometry  Geometry container that is mutated with the ellipse entities.
 * @param embedType Operation mode (ENTIRE, CUT, or REMOVE) controlling whether the
 *                  ellipse becomes a cutting loop or a full surface.
 * @return void
 */
void GeometryBuilder::push_geometry_ellipse(General general, Geometry geometry, OperationType embedType) {
    int n_node    = geometry->node_num;
    int n_line    = geometry->line_num;
    int n_curve   = geometry->curve_num;
    int n_surface = geometry->surface_num;

    char newToolTag[MAXARGC], ObjectTag[MAXARGC];
    parseStringinQuotation(general->inputfile, newToolTag);
    // get the tag of tool surface
    if ((embedType == OperationType::CUT) || (embedType == OperationType::REMOVE)) {
        parseStringinQuotation(general->inputfile, ObjectTag);
    }

    Real value;
    Int  N;
    Real x0, y0, radius_max, radius_min, theta, theta_0, theta_1;
    Real container[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    ns_mesh::GeometryKeywordReader::pushEllipseGeometryKeywords(general->inputfile, container);
    x0         = container[0];
    y0         = container[1];
    radius_min = container[2];
    radius_max = container[3];
    theta_0    = container[4];
    N          = container[5];

    /* obtain the arc size to avoid being deleted */
    if (geometry->min_size == 0.0)
        geometry->min_size = (2.0 * OpenDFN_PI * radius_min + 4.0 * (radius_max - radius_min)) / N;
    else
        geometry->min_size =
            OpenDFN_min(geometry->min_size, (2.0 * OpenDFN_PI * radius_min + 4.0 * (radius_max - radius_min)) / N);

    /* add geometry node */
    for (int i = 0; i < N; i++) {
        // malloc node
        buildmemory(&geometry->node, (n_node + 1), n_node);
        theta                    = ((double)i / (double)N) * 2 * OpenDFN_PI;
        theta_1                  = atan2(radius_max * sin(theta), radius_min * cos(theta));
        geometry->node[n_node].x = x0 + radius_max * cos(theta_1) * cos(theta_0) - radius_min * sin(theta_1) * sin(theta_0);
        geometry->node[n_node].y = y0 + radius_max * cos(theta_1) * sin(theta_0) + radius_min * sin(theta_1) * cos(theta_0);
        getGeoNodeGroupbyCoord(geometry, newToolTag, geometry->node[n_node].x, geometry->node[n_node].y);
        n_node++;
    }

    /* add geometry line */
    for (int i = 0; i < N; i++) {
        buildmemory(&geometry->line, (n_line + 1), n_line);
        // copy the tag
        copyChars(geometry->line[n_line].tag, newToolTag);

        geometry->line[n_line].node_1 = geometry->node_num + i;
        // assign the element flag,  it is not truss
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
    //addtoLinetoNodeGroups(geometry, newToolTag);
    // build for line groups
    if (embedType == OperationType::CUT | embedType == OperationType::REMOVE) {
        addtoLineGroups(geometry, newToolTag);
    }
}
}  // namespace ns_geometry