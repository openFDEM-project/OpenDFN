/******************************************************************************
 * \file      get_geo_node_group_plane.cpp
 * \brief     Basic classes parser mesh fron selfdeveloped format parameters.
 *
 * \details   Implementation of basic classes parser mesh fron selfdeveloped
 *            format parameters.
 *
 * \author    Xiaofeng Li
 * \email     xfli@whrsm.ac.cn
 * \date      Created:       June 24, 2020
 * \date      Last modified: 2026-07-20 00:04:28
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
#include "common/opendfn_math.h"
#include "geometry/opendfn_geometry.h"
#include "geometry/opendfn_geometry_module.h"
namespace ns_geometry {
/**
 * \brief Register a geometry node group covering points to the left of a line.
 *
 * Appends a new group of type GroupType::PLANELEFT to the geometry's group list.
 * The dividing line is defined by the two endpoints (x1, y1) and (x2, y2), stored
 * in the group's range[0..3] fields; nodes on the left half-plane are later
 * selected against it.
 *
 * Side effects: aborts the program via OPENDFN_EXIT when the geometry has no
 * nodes; validates existing tags through chechGeomtryGroups; reallocates
 * geometry->group via buildmemory; and increments geometry->group_num.
 *
 * \param geometry Geometry object whose group list is extended (mutated in place).
 * \param tag      Name assigned to the new group (copied into the group record).
 * \param x1       X coordinate of the first line endpoint.
 * \param y1       Y coordinate of the first line endpoint.
 * \param x2       X coordinate of the second line endpoint.
 * \param y2       Y coordinate of the second line endpoint.
 */
void GeometryBuilder::getGeoNodeGroupOnPlaneLeft(Geometry geometry, char* tag, double x1, double y1, double x2, double y2) {
    if (geometry->node_num == 0) {
        OpenDFNMessage::RuntimeInfo("Error: no geometry point found in the model.");
        OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
    }
    chechGeomtryGroups(geometry, tag);
    buildmemory(&geometry->group, (geometry->group_num + 1), geometry->group_num);
    if (geometry->group) {
        geometry->group[geometry->group_num].type     = GroupType::PLANELEFT;
        geometry->group[geometry->group_num].range[0] = x1;
        geometry->group[geometry->group_num].range[1] = y1;
        geometry->group[geometry->group_num].range[2] = x2;
        geometry->group[geometry->group_num].range[3] = y2;
        geometry->group[geometry->group_num].mesh_size = geometry->mesh_size;

        copyChars(geometry->group[geometry->group_num].tag, tag);
    }
    geometry->group_num++;
}

/**
 * \brief Register a geometry node group covering points to the right of a line.
 *
 * Appends a new group of type GroupType::PLANERIGHT to the geometry's group list.
 * The dividing line is defined by the two endpoints (x1, y1) and (x2, y2), stored
 * in the group's range[0..3] fields; nodes on the right half-plane are later
 * selected against it.
 *
 * Side effects: aborts the program via OPENDFN_EXIT when the geometry has no
 * nodes; validates existing tags through chechGeomtryGroups; reallocates
 * geometry->group via buildmemory; and increments geometry->group_num.
 *
 * \param geometry Geometry object whose group list is extended (mutated in place).
 * \param tag      Name assigned to the new group (copied into the group record).
 * \param x1       X coordinate of the first line endpoint.
 * \param y1       Y coordinate of the first line endpoint.
 * \param x2       X coordinate of the second line endpoint.
 * \param y2       Y coordinate of the second line endpoint.
 */
void GeometryBuilder::getGeoNodeGroupOnPlaneRight(Geometry geometry, char* tag, double x1, double y1, double x2, double y2) {
    if (geometry->node_num == 0) {
        OpenDFNMessage::RuntimeInfo("Error: no geometry point found in the model.");
        OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
    }
    chechGeomtryGroups(geometry, tag);
    buildmemory(&geometry->group, (geometry->group_num + 1), geometry->group_num);
    if (geometry->group) {
        geometry->group[geometry->group_num].type     = GroupType::PLANERIGHT;
        geometry->group[geometry->group_num].range[0] = x1;
        geometry->group[geometry->group_num].range[1] = y1;
        geometry->group[geometry->group_num].range[2] = x2;
        geometry->group[geometry->group_num].range[3] = y2;
        geometry->group[geometry->group_num].mesh_size = geometry->mesh_size;

        copyChars(geometry->group[geometry->group_num].tag, tag);
    }
    geometry->group_num++;
}

/**
 * \brief Register a geometry node group covering points lying on a line.
 *
 * Appends a new group of type GroupType::PLANEON to the geometry's group list.
 * The line is defined by the two endpoints (x1, y1) and (x2, y2), stored in the
 * group's range[0..3] fields; nodes lying on the line are later selected against it.
 *
 * Side effects: aborts the program via OPENDFN_EXIT when the geometry has no
 * nodes; validates existing tags through chechGeomtryGroups; reallocates
 * geometry->group via buildmemory; and increments geometry->group_num.
 *
 * \param geometry Geometry object whose group list is extended (mutated in place).
 * \param tag      Name assigned to the new group (copied into the group record).
 * \param x1       X coordinate of the first line endpoint.
 * \param y1       Y coordinate of the first line endpoint.
 * \param x2       X coordinate of the second line endpoint.
 * \param y2       Y coordinate of the second line endpoint.
 */
void GeometryBuilder::getGeoNodeGroupOnPlane(Geometry geometry, char* tag, double x1, double y1, double x2, double y2) {
    if (geometry->node_num == 0) {
        OpenDFNMessage::RuntimeInfo("Error: no geometry point found in the model.");
        OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
    }
    chechGeomtryGroups(geometry, tag);
    buildmemory(&geometry->group, (geometry->group_num + 1), geometry->group_num);
    if (geometry->group) {
        int size                                      = 0;
        geometry->group[geometry->group_num].type     = GroupType::PLANEON;
        geometry->group[geometry->group_num].range[0] = x1;
        geometry->group[geometry->group_num].range[1] = y1;
        geometry->group[geometry->group_num].range[2] = x2;
        geometry->group[geometry->group_num].range[3] = y2;
        geometry->group[geometry->group_num].mesh_size = geometry->mesh_size;

        copyChars(geometry->group[geometry->group_num].tag, tag);
    }
    geometry->group_num++;
}
}  // namespace ns_geometry
