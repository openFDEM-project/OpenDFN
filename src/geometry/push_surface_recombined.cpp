/******************************************************************************
 * \file      push_surface_recombined.cpp
 * \brief     Basic classes parser mesh fron selfdeveloped format parameters.
 *
 * \details   Implementation of basic classes parser mesh fron selfdeveloped
 *            format parameters.
 *
 * \author    Xiaofeng Li
 * \email     xfli@whrsm.ac.cn
 * \date      Created:       June 24, 2020
 * \date      Last modified: 2026-07-20 00:06:44
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
 * \brief Flag geometry surfaces to use quad recombination during meshing,
 *        either for all surfaces or for a specific named surface group.
 *
 * Sets the global geometry->recombined flag, then when \p keyword is "default"
 * marks every surface as recombined. Otherwise marks only those surfaces whose
 * tag matches \p keyword (all duplicate matches are flagged, no early break).
 *
 * Side effects: mutates geometry->recombined and geometry->surface[i].recombined;
 * on failure to match a named group emits a runtime error message and calls
 * OPENDFN_EXIT, terminating the program. Note: \p general is unused here.
 *
 * \param geometry Geometry container whose surface flags are updated.
 * \param general  Global state (unused in this routine).
 * \param keyword  Either "default" for all surfaces, or a surface group tag.
 * \return void
 */
void GeometryBuilder::push_surface_recombined(Geometry geometry, General general, char* keyword) {
    short find_id        = 0;
    geometry->recombined = 1;
    if (isSameString("default", keyword)) {
        // push to all nodes
        for (UInt i = 0; i < geometry->surface_num; i++) {
            geometry->surface[i].recombined = 1;
        }
    } else {
        for (UInt i = 0; i < geometry->surface_num; i++) {
            if (isSameString(geometry->surface[i].tag, keyword)) {
                find_id++;
                geometry->surface[i].recombined = 1;
            }
        }
        if (find_id == 0) {
            OpenDFNMessage::RuntimebackStringString("Error: fail to find surface group ", keyword);
            OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
        }
    }
}
}  // namespace ns_geometry
