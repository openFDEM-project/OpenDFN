/******************************************************************************
 * \file      get_geo_node_group_square.cpp
 * \brief     Basic classes parser mesh fron selfdeveloped format parameters.
 *
 * \details   Implementation of basic classes parser mesh fron selfdeveloped
 *            format parameters.
 *
 * \author    Xiaofeng Li
 * \email     xfli@whrsm.ac.cn
 * \date      Created:       June 24, 2020
 * \date      Last modified: 2026-07-20 00:04:36
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
 * \brief Register a geometry node group that selects nodes lying strictly inside a
 *        rectangular (axis-aligned box) region.
 *
 * Grows the geometry group array by one entry and populates it with a
 * GroupType::RETANGULARIN selector, the box bounds and the default mesh size, then
 * copies the group tag. Aborts the program if the geometry has no points.
 *
 * Side effects: reallocates and appends to geometry->group, increments
 * geometry->group_num, and terminates the process via OPENDFN_EXIT on error.
 *
 * \param geometry Geometry container whose group list is extended (mutated in place).
 * \param tag      Name assigned to the new node group.
 * \param x_lef    Left (minimum x) bound of the box.
 * \param x_rig    Right (maximum x) bound of the box.
 * \param y_bot    Bottom (minimum y) bound of the box.
 * \param y_top    Top (maximum y) bound of the box.
 */
void GeometryBuilder::getGeoNodeGroupInBoxRange(
    Geometry geometry, char* tag, double x_lef, double x_rig, double y_bot, double y_top) {
    if (geometry->node_num == 0) {
        OpenDFNMessage::RuntimeInfo("Error: no geometry point found in the model.");
        OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
    }
    chechGeomtryGroups(geometry, tag);
    buildmemory(&geometry->group, (geometry->group_num + 1), geometry->group_num);
    if (geometry->group) {
        geometry->group[geometry->group_num].type     = GroupType::RETANGULARIN;
        geometry->group[geometry->group_num].range[0] = x_lef;
        geometry->group[geometry->group_num].range[1] = x_rig;
        geometry->group[geometry->group_num].range[2] = y_bot;
        geometry->group[geometry->group_num].range[3] = y_top;
        geometry->group[geometry->group_num].mesh_size = geometry->mesh_size;

        copyChars(geometry->group[geometry->group_num].tag, tag);
    }
    geometry->group_num++;
}

/**
 * \brief Register a geometry node group that selects nodes lying on the boundary of a
 *        rectangular (axis-aligned box) region.
 *
 * Grows the geometry group array by one entry and populates it with a
 * GroupType::RETANGULARON selector, the box bounds and the default mesh size, then
 * copies the group tag. Aborts the program if the geometry has no points.
 *
 * Side effects: reallocates and appends to geometry->group, increments
 * geometry->group_num, and terminates the process via OPENDFN_EXIT on error.
 *
 * \param geometry Geometry container whose group list is extended (mutated in place).
 * \param tag      Name assigned to the new node group.
 * \param x_lef    Left (minimum x) bound of the box.
 * \param x_rig    Right (maximum x) bound of the box.
 * \param y_bot    Bottom (minimum y) bound of the box.
 * \param y_top    Top (maximum y) bound of the box.
 */
void GeometryBuilder::getGeoNodeGroupOnBoxRange(
    Geometry geometry, char* tag, double x_lef, double x_rig, double y_bot, double y_top) {
    if (geometry->node_num == 0) {
        OpenDFNMessage::RuntimeInfo("Error: no geometry point found in the model.");
        OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
    }
    chechGeomtryGroups(geometry, tag);
    buildmemory(&geometry->group, (geometry->group_num + 1), geometry->group_num);
    if (geometry->group) {
        geometry->group[geometry->group_num].type     = GroupType::RETANGULARON;
        geometry->group[geometry->group_num].range[0] = x_lef;
        geometry->group[geometry->group_num].range[1] = x_rig;
        geometry->group[geometry->group_num].range[2] = y_bot;
        geometry->group[geometry->group_num].range[3] = y_top;
        geometry->group[geometry->group_num].mesh_size = geometry->mesh_size;

        copyChars(geometry->group[geometry->group_num].tag, tag);
    }
    geometry->group_num++;
}
/**
 * \brief Register a geometry node group that selects nodes lying outside a
 *        rectangular (axis-aligned box) region.
 *
 * Grows the geometry group array by one entry and populates it with a
 * GroupType::RETANGULAROUT selector, the box bounds and the default mesh size, then
 * copies the group tag. Aborts the program if the geometry has no points.
 *
 * Side effects: reallocates and appends to geometry->group, increments
 * geometry->group_num, and terminates the process via OPENDFN_EXIT on error.
 *
 * \param geometry Geometry container whose group list is extended (mutated in place).
 * \param tag      Name assigned to the new node group.
 * \param x_lef    Left (minimum x) bound of the box.
 * \param x_rig    Right (maximum x) bound of the box.
 * \param y_bot    Bottom (minimum y) bound of the box.
 * \param y_top    Top (maximum y) bound of the box.
 */
void GeometryBuilder::getGeoNodeGroupOutBoxRange(
    Geometry geometry, char* tag, double x_lef, double x_rig, double y_bot, double y_top) {
    if (geometry->node_num == 0) {
        OpenDFNMessage::RuntimeInfo("Error: no geometry point found in the model.");
        OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
    }
    chechGeomtryGroups(geometry, tag);
    buildmemory(&geometry->group, (geometry->group_num + 1), geometry->group_num);
    if (geometry->group) {
        geometry->group[geometry->group_num].type     = GroupType::RETANGULAROUT;
        geometry->group[geometry->group_num].range[0] = x_lef;
        geometry->group[geometry->group_num].range[1] = x_rig;
        geometry->group[geometry->group_num].range[2] = y_bot;
        geometry->group[geometry->group_num].range[3] = y_top;
        geometry->group[geometry->group_num].mesh_size = geometry->mesh_size;

        copyChars(geometry->group[geometry->group_num].tag, tag);
    }
    geometry->group_num++;
}
}  // namespace ns_geometry
