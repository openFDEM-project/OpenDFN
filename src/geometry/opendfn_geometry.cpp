/******************************************************************************
 * \file      opendfn_geometry.cpp
 * \brief     Implementation of the geometry data structures declared in
 *            opendfn_geometry.h. Provides the memory-management helpers
 *            (clear/resize) that allocate and free the dynamic containers
 *            (nodes, lines, curves, surfaces, groups, line groups and
 *            particles) used by the geometry preprocessing module.
 *
 * \details   Implementation of implementation of the geometry data structures
 *            declared in opendfn_geometry.h. Provides the memory-management
 *            helpers (clear/resize) that allocate and free the dynamic
 *            containers (nodes, lines, curves, surfaces, groups, line groups
 *            and particles) used by the geometry preprocessing module.
 *
 * \author    Xiaofeng Li
 * \email     xfli@whrsm.ac.cn
 * \date      Created:       June 24, 2020
 * \date      Last modified: 2026-07-20 00:05:18
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

#include <common/memory_manager.h>
#include <opendfn_geometry.h>

using namespace ns_common;

/**
 * \brief Release the dynamically allocated DimTag arrays owned by a geometry line.
 *
 * Frees the DimTags, DimTags1 and DimTags2 buffers if they were allocated. Side effect:
 * releases heap memory held by this geo_line_type instance. The pointers are not reset to
 * nullptr, so the caller must not free or dereference them again.
 */
void geo_line_type::clear() {
    // clear the list
    if (DimTags != nullptr)
        free(DimTags);
    // clear the list
    if (DimTags1 != nullptr)
        free(DimTags1);
    // clear the list
    if (DimTags2 != nullptr)
        free(DimTags2);
}

/**
 * \brief Release the dynamically allocated DimTags array owned by an embedded line group.
 *
 * Side effect: frees the heap buffer holding the grouped line DimTag pairs when present.
 * The pointer is left dangling and must not be reused.
 */
void geo_EmbededlineGroups_type::clear() {
    // clear the list
    if (DimTags != nullptr)
        free(DimTags);
}

/**
 * \brief Release the line-index array owned by a closed geometry curve.
 *
 * Side effect: frees the heap buffer storing the ordered list of line ids that compose the
 * curve when present. The pointer is left dangling and must not be reused.
 */
void geo_curve_type::clear() {
    // clear the list
    if (line != nullptr)
        free(line);
}

/**
 * \brief Release every dynamically allocated array owned by a 2D surface.
 *
 * Frees, when present, the boolean-operation tool buffers (removetool, fragmenttool,
 * boollines), the resulting DimTags buffer and the curve-index list. Side effect: releases
 * all heap memory held by this geo_surface_type instance; the freed pointers are left
 * dangling and must not be reused.
 */
void geo_surface_type::clear() {
    // clear the list
    if (removetool != nullptr)
        free(removetool);
    // clear the list
    if (fragmenttool != nullptr)
        free(fragmenttool);
    // clear the list
    if (boollines != nullptr)
        free(boollines);
    // clear the list
    if (DimTags != nullptr)
        free(DimTags);
    // clear the list
    if (curve != nullptr)
        free(curve);
}

/**
 * \brief Grow (or allocate) one of the geometry container arrays by name.
 *
 * Dispatches on \p type and delegates to buildmemory() to (re)allocate the matching member
 * array. Recognised types are "node", "line", "curve", "surface", "group" and "linegroups";
 * any other value is a no-op. Side effect: mutates the corresponding pointer member of this
 * Geometry_structure to point at the (re)allocated buffer.
 *
 * \param type        Name of the container to resize.
 * \param size        Target capacity (number of elements) for the container.
 * \param initialsize Number of already-initialised elements to preserve during reallocation.
 */
void Geometry_structure::resize(std::string type, Int size, Int initialsize) {
    if (type == "node") {
        // resize the list
        buildmemory(&node, size, initialsize);
    } else if (type == "line") {
        // resize the list
        buildmemory(&line, size, initialsize);
    } else if (type == "curve") {
        // resize the list
        buildmemory(&curve, size, initialsize);
    } else if (type == "surface") {
        // resize the list
        buildmemory(&surface, size, initialsize);
    } else if (type == "group") {
        // resize the list
        buildmemory(&group, size, initialsize);
    } else if (type == "linegroups") {
        // resize the list
        buildmemory(&linegroups, size, initialsize);
    }
}

void Geometry_structure::clear() {
    // clear the list
    if (node_num)
        free(node);
    node_num = 0;

    // clear the list
    _for(i, 0, line_num) line[i].clear();
    if (line_num)
        free(line);
    line_num = 0;

    // clear the list
    _for(i, 0, curve_num) curve[i].clear();
    if (curve_num)
        free(curve);
    curve_num = 0;

    // clear the list
    _for(i, 0, surface_num) surface[i].clear();
    if (surface_num)
        free(surface);
    surface_num = 0;

    // clear the list
    if (group_num)
        free(group);
    group_num = 0;
    
    // clear the list
    _for(i, 0, linegroups_num) linegroups[i].clear();
    if (linegroups_num)
        free(linegroups);
    linegroups_num = 0;

    // clear the list
    _for(i, 0, wall_num) free(wallTags[i]);
    if (wall_num)
        free(wallTags);
    wall_num = 0;

    // clear the list
    _for(i, 0, LineElement_n) free(LineElememtTags[i]);
    if (LineElement_n)
        free(LineElememtTags);
    LineElement_n = 0;

    // clear the list
   /* _for(i, 0, LineFlip_n) free(LineFlipTags[i]);
    if (LineFlip_n)
        free(LineFlipTags);
    LineFlip_n = 0;*/
}
