/******************************************************************************
 * \file      get_geo_node_group_point.cpp
 * \brief     Basic classes parser mesh fron selfdeveloped format parameters.
 *
 * \details   Implementation of basic classes parser mesh fron selfdeveloped
 *            format parameters.
 *
 * \author    Xiaofeng Li
 * \email     xfli@whrsm.ac.cn
 * \date      Created:       June 24, 2020
 * \date      Last modified: 2026-07-20 00:04:40
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

// local includes
#include "common/opendfn_math.h"
#include "geometry/opendfn_geometry.h"
#include "geometry/opendfn_geometry_module.h"

namespace ns_geometry {
/**
 * \brief Register a geometry node group selecting a point at a given coordinate.
 *
 * Appends a new group of type GroupType::FROMCOORD to the geometry's group list.
 * The target point (x0, y0) is stored in range[0..1], while range[2] holds a small
 * tolerance (1e-7) used when matching a node to the requested coordinate. A new
 * group is always created, even when the tag duplicates an existing one.
 *
 * Side effects: reallocates geometry->group via buildmemory and increments
 * geometry->group_num.
 *
 * \param geometry Geometry object whose group list is extended (mutated in place).
 * \param tag      Name assigned to the new group (copied into the group record).
 * \param x0       X coordinate of the target point.
 * \param y0       Y coordinate of the target point.
 */
void GeometryBuilder::getGeoNodeGroupbyCoord(Geometry geometry, char* tag, double x0, double y0) {
    // crate a new group, even for the duplicated tags
    buildmemory(&geometry->group, (geometry->group_num + 1), geometry->group_num);

    if (geometry->group) {
        geometry->group[geometry->group_num].type = GroupType::FROMCOORD;

        geometry->group[geometry->group_num].range[0]  = x0;
        geometry->group[geometry->group_num].range[1]  = y0;
        geometry->group[geometry->group_num].range[2]  = 1e-7;
        geometry->group[geometry->group_num].mesh_size = geometry->mesh_size;

        copyChars(geometry->group[geometry->group_num].tag, tag);
    }
    geometry->group_num++;
}
}  // namespace ns_geometry