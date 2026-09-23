/******************************************************************************
 * \file      get_geo_node_group_circle.cpp
 * \brief     Basic classes parser mesh fron selfdeveloped format parameters.
 *
 * \details   Implementation of basic classes parser mesh fron selfdeveloped
 *            format parameters.
 *
 * \author    Xiaofeng Li
 * \email     xfli@whrsm.ac.cn
 * \date      Created:       June 24, 2020
 * \date      Last modified: 2026-07-20 00:04:04
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
 * \brief Register a geometry node group covering points inside a circular region.
 *
 * Appends a new group of type GroupType::CIRCLEIN to the geometry's group list.
 * The circle is defined by its centre (x0, y0) and radius, stored in the group's
 * range[0..2] fields; nodes lying inside the circle are later selected against it.
 *
 * Side effects: aborts the program via OPENDFN_EXIT when the geometry has no
 * nodes; validates existing tags through chechGeomtryGroups; reallocates
 * geometry->group via buildmemory; and increments geometry->group_num.
 *
 * \param geometry Geometry object whose group list is extended (mutated in place).
 * \param tag      Name assigned to the new group (copied into the group record).
 * \param x0       X coordinate of the circle centre.
 * \param y0       Y coordinate of the circle centre.
 * \param radius   Radius of the circular selection region.
 */
void GeometryBuilder::getGeoNodeGroupInCircleRange(Geometry geometry, char* tag, double x0, double y0, double radius) {
    if (geometry->node_num == 0) {
        OpenDFNMessage::RuntimeInfo("Error: no geometry point found in the model.");
        OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
    }
    chechGeomtryGroups(geometry,  tag);
    buildmemory(&geometry->group, (geometry->group_num + 1), geometry->group_num);
    
    if (geometry->group) {
        geometry->group[geometry->group_num].type     = GroupType::CIRCLEIN;
        geometry->group[geometry->group_num].range[0] = x0;
        geometry->group[geometry->group_num].range[1] = y0;
        geometry->group[geometry->group_num].range[2] = radius;
        geometry->group[geometry->group_num].mesh_size = geometry->mesh_size;

        copyChars(geometry->group[geometry->group_num].tag, tag);

        geometry->group_num++;
    }
}

/**
 * \brief Register a geometry node group covering points outside a circular region.
 *
 * Appends a new group of type GroupType::CIRCLEOUT to the geometry's group list.
 * The circle is defined by its centre (x0, y0) and radius, stored in range[0..2];
 * nodes lying outside the circle are later selected against it.
 *
 * Side effects: aborts the program via OPENDFN_EXIT when the geometry has no
 * nodes; validates existing tags through chechGeomtryGroups; reallocates
 * geometry->group via buildmemory; and increments geometry->group_num.
 *
 * \param geometry Geometry object whose group list is extended (mutated in place).
 * \param tag      Name assigned to the new group (copied into the group record).
 * \param x0       X coordinate of the circle centre.
 * \param y0       Y coordinate of the circle centre.
 * \param radius   Radius of the circular selection region.
 */
void GeometryBuilder::getGeoNodeGroupOutCircleRange(Geometry geometry, char* tag, double x0, double y0, double radius) {
    if (geometry->node_num == 0) {
        OpenDFNMessage::RuntimeInfo("Error: no geometry point found in the model.");
        OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
    }
    chechGeomtryGroups(geometry, tag);
    buildmemory(&geometry->group, (geometry->group_num + 1), geometry->group_num);

    if (geometry->group) {
        geometry->group[geometry->group_num].type     = GroupType::CIRCLEOUT;
        geometry->group[geometry->group_num].range[0] = x0;
        geometry->group[geometry->group_num].range[1] = y0;
        geometry->group[geometry->group_num].range[2] = radius;
        geometry->group[geometry->group_num].mesh_size = geometry->mesh_size;

        copyChars(geometry->group[geometry->group_num].tag, tag);
    }

    geometry->group_num++;
}

/**
 * \brief Register a geometry node group covering points lying on a circle.
 *
 * Appends a new group of type GroupType::CIRCLEON to the geometry's group list.
 * The circle is defined by its centre (x0, y0) and radius, stored in range[0..2];
 * nodes lying on the circle boundary are later selected against it.
 *
 * Side effects: aborts the program via OPENDFN_EXIT when the geometry has no
 * nodes; validates existing tags through chechGeomtryGroups; reallocates
 * geometry->group via buildmemory; and increments geometry->group_num.
 *
 * \param geometry Geometry object whose group list is extended (mutated in place).
 * \param tag      Name assigned to the new group (copied into the group record).
 * \param x0       X coordinate of the circle centre.
 * \param y0       Y coordinate of the circle centre.
 * \param radius   Radius of the circular boundary.
 */
void GeometryBuilder::getGeoNodeGroupOnCircle(Geometry geometry, char* tag, double x0, double y0, double radius) {
    if (geometry->node_num == 0) {
        OpenDFNMessage::RuntimeInfo("Error: no geometry point found in the model.");
        OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
    }
    chechGeomtryGroups(geometry, tag);
    buildmemory(&geometry->group, (geometry->group_num + 1), geometry->group_num);

    if (geometry->group) {
        geometry->group[geometry->group_num].type     = GroupType::CIRCLEON;
        geometry->group[geometry->group_num].range[0] = x0;
        geometry->group[geometry->group_num].range[1] = y0;
        geometry->group[geometry->group_num].range[2] = radius;
        geometry->group[geometry->group_num].mesh_size = geometry->mesh_size;

        copyChars(geometry->group[geometry->group_num].tag, tag);
    }
    geometry->group_num++;
}

}  // namespace ns_geometry
