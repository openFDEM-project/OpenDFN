/******************************************************************************
 * \file      push_mesh_size.cpp
 * \brief     Basic classes parser mesh fron selfdeveloped format parameters.
 *
 * \details   Implementation of basic classes parser mesh fron selfdeveloped
 *            format parameters.
 *
 * \author    Xiaofeng Li
 * \email     xfli@whrsm.ac.cn
 * \date      Created:       June 24, 2020
 * \date      Last modified: 2026-07-20 00:04:44
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

#include "common/opendfn_math.h"
#include "geometry/opendfn_geometry_module.h"

namespace ns_geometry {
/**
 * \brief Parse and assign a target mesh size from the input file to the
 *        geometry, either globally or to specific named node groups.
 *
 * Reads a single real value from \p general->inputfile via getRealValue. When
 * \p keyword is "default" the value becomes the global geometry mesh size and
 * updates the running minimum size (min_size). Otherwise the value is applied
 * to the mesh_size of every geometry group whose tag matches \p keyword (all
 * duplicate matches are updated, no early break).
 *
 * Side effects: mutates geometry->mesh_size / geometry->min_size (default case)
 * or geometry->group[i].mesh_size (named case); advances the input file cursor;
 * on failure emits a runtime error message and calls OPENDFN_EXIT, terminating
 * the program.
 *
 * \param geometry Geometry container whose mesh size / groups are updated.
 * \param general  Global state holding the input file stream to read from.
 * \param keyword  Either "default" for the global size, or a node group tag.
 * \return void
 */
void GeometryBuilder::push_mesh_size(Geometry geometry, General general, char* keyword) {
    Real  value;
    short find_id = 0;
    // Read the mesh-size value that follows the keyword in the input stream.
    getRealValue(general->inputfile, &value);

    if (isSameString("default", keyword)) {
        geometry->mesh_size = value;

        // Track the smallest mesh size seen so far across all assignments.
        if (geometry->min_size == 0.0)
            geometry->min_size = value;
        else
            geometry->min_size = OpenDFN_min(geometry->min_size, value);
    } else {
        // A named target requires at least one defined node group.
        if (geometry->group_num == 0) {
            OpenDFNMessage::RuntimebackStringString("Error: no geometry node groups defined:", keyword);
            OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
        }
        for (UInt i = 0; i < geometry->group_num; i++) {
            if (isSameString(geometry->group[i].tag, keyword)) {
                find_id++;
                // push to element matid
                geometry->group[i].mesh_size = value;  
                // no break, to find all duplicated groups
            }
        }
        if (find_id == 0) {
            OpenDFNMessage::RuntimebackStringString("Error: fail to find node group ", keyword);
            OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
        }
    }
}
}  // namespace ns_geometry
