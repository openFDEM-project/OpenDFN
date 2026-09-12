/******************************************************************************
 * \file      opendfn_geometry.h
 * \brief     Basic classes variables stored for geometry module, which is used
 *            to create mesh based on built-in commands in OpenDFN to create
 *            geometries and meshes. The variables will be free up after the
 *            prepocessing operation. The particles, irregular particles, lumped
 *            material points will aslo be created in this module.
 *
 * \details   Declarations for basic classes variables stored for geometry
 *            module, which is used to create mesh based on built-in commands in
 *            OpenDFN to create geometries and meshes. The variables will be
 *            free up after the prepocessing operation. The particles, irregular
 *            particles, lumped material points will aslo be created in this
 *            module.
 *
 * \author    Xiaofeng Li
 * \email     xfli@whrsm.ac.cn
 * \date      Created:       June 24, 2020
 * \date      Last modified: 2026-07-20 00:52:25
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

#ifndef _OPENDFN_STRUCTURE_GEOMETRY_
#define _OPENDFN_STRUCTURE_GEOMETRY_

#include "common/opendfn_common.h"

/**
 * \brief Selection rule used to group mesh nodes/points relative to a geometric region.
 *
 * Determines how nodes are picked when assigning mesh sizes or physical groups:
 * relative to a circle, plane half-space, rectangular box, or by referencing
 * existing line/surface tags or explicit coordinates.
 */
enum class GroupType {
    NOT,
    // node on the circle
    CIRCLEON,
    // node in the circel
    CIRCLEIN,
    // node out the circle
    CIRCLEOUT,
    // node on the plane
    PLANEON,
    // node above the plane
    PLANELEFT,
    // node below the plane
    PLANERIGHT,
    // node on the box
    RETANGULARON,
    // node in the box
    RETANGULARIN,
    // node out the box
    RETANGULAROUT,
    // from line tags
    FROMLINETAGS,
    // from surface tags
    FROMSURFACETAGS,
    // from coord for points
    FROMCOORD
};

/**
 * \brief Boolean operation applied when embedding one geometry entity into another.
 *
 * ENTIRE keeps the entity whole, REMOVE deletes the newly created surface after
 * the operation, and CUT embeds the tool (e.g. a joint) into the target.
 */
enum class OperationType {
    // entire
    ENTIRE,
    // remove the new surface
    REMOVE,
    // embed
    CUT
};

/**
 * \brief Kind of primitive solid/curve used when building an OpenCASCADE geometry entity.
 *
 * Covers 2D surface primitives (rectangle, disk) and 1D curve primitives
 * (circle, ellipse, their arcs, and straight lines).
 */
enum class geoSolidType {
    // retangle occ type surface
    RETANGLE,
    // disk type, penny-shaped or ellipse, surface
    DISK,
    // cricle line
    CIRCLE,
    // ellipse line
    ELLIPSE,
    // circle arc line
    CIRCLEARC,
    // ellipse arc line
    ELLIPSEARC,
    // line
    LINE
};

/**
 * \brief A single geometry point defined by its Cartesian coordinates.
 */
struct geo_node_type {
    Real x = 0.0;  // coord.x
    Real y = 0.0;  // coord.y
    Real z = 0.0;  // coord.z
    // constructor
    geo_node_type() = default;
};
/**
 * structure to define the 1D lines. It contains the information of
 * physcial group, embed type and node list.
 */
struct geo_line_type {
    // line group tag
    char tag[MAXARGC] = "\0";
    // bool for truss element
    bool iselement = true;
    // bool to be reserved
    bool tobeReserved = false;
    // bool for embeded as joints
    bool isembeded = false;
    // new DimTags after bool
    int DimTags_n = 0;
    // tool is DimTag pairs, length is two times of n_tool
    int* DimTags = nullptr;
    // new DimTags after bool
    int layers     = 0;
    int DimTags1_n = 0;
    // tool is DimTag pairs, length is two times of n_tool
    int* DimTags1 = nullptr;
    // new DimTags after bool
    int DimTags2_n = 0;
    // tool is DimTag pairs, length is two times of n_tool
    int* DimTags2 = nullptr;
    // start node id
    int node_1 = -1;
    // end node id
    int node_2 = -1;
    // physcial id
    int physicalID = -1;
    // constructor
    geo_line_type() = default;
    // clear the list
    void clear();
};

/**
 * structure to define the 1D lines. It contains the information of
 * physcial group, embed type and node list.
 */
struct geo_EmbededlineGroups_type {
    // line group tag
    char tag[MAXARGC] = "\0";
    // new DimTags after bool for line tags, which will be grouped
    int DimTags_n = 0;
    // tool is DimTag pairs, length is two times of n_tool
    int* DimTags = nullptr;
    // constructor
    geo_EmbededlineGroups_type() = default;
    // clear the list
    void clear();
};
/**
 * structure to define 1D closed geometry curve, a assembley of 2D lines.
 *
 * \note The order of the line should be anticlockwise, otherwise will
 * generate inverse mesh
 *
 */
struct geo_curve_type {
    // curve tag
    char tag[MAXARGC] = "\0";
    // line count in this curve
    int n_line = 0;
    // line list in this curve
    int* line        = nullptr;
    geo_curve_type() = default;
    // clear the list
    void clear();
};
/**
 * structures to define 2D surfaces, all surfaces should be defined by closed curves.
 */
struct geo_surface_type {
    // surface tags
    char tag[MAXARGC] = "\0";
    // tool number and tool list, for surface-surface only
    int n_removetool = 0;
    // tool is DimTag pairs, length is two times of n_tool, the tag shoule be identical to gmsh, start from 1
    int* removetool = nullptr;
    // tool number and tool list, for surface-surface only
    int n_fragmenttool = 0;
    // tool is DimTag pairs, length is two times of n_tool, start from 1
    int* fragmenttool = nullptr;
    // curve tool number and curve tool list, for curve-surface only, start from 1
    int n_boollines = 0;
    // tool is DimTag pairs, length is two times of n_tool
    int* boollines = nullptr;
    // new DimTags after bool
    int DimTags_n = 0;
    // tool is DimTag pairs, length is two times of n_tool
    int* DimTags = nullptr;
    // surface to recombine operator
    int recombined = -1;
    // curve count in this surface
    int n_curve = 0;
    // curve list
    int* curve         = nullptr;
    geo_surface_type() = default;
    // clear the list
    void clear();
};
/**
 * structure of geometry groups.
 */
struct geo_group_type {
    // group names, no more than 20 charactors
    char tag[MAXARGC] = "\0";
    // group type
    GroupType type = GroupType::RETANGULARIN;
    // bool for inserted points
    bool isInsertedPoints = false;
    // container to store the range
    Real range[4];
    // group mesh size by usr input and assign
    Real mesh_size = 0.0;
    // constructor
    geo_group_type() = default;
};
typedef struct Geometry_structure* Geometry;
/**
 * Main structure to control the geometry in gmsh.
 */
struct Geometry_structure {
    // AABB domian range
    Real x0 = 0.0;
    Real x1 = 0.0;
    Real y0 = 0.0;
    Real y1 = 0.0;
    // node count input to gmsh
    int node_num = 0;
    // line count
    int line_num = 0;
    // curve count
    int curve_num = 0;
    // surface count
    int surface_num = 0;
    // group count for all entities
    int group_num = 0;
    // count of line groups
    int linegroups_num = 0;
    // element order
    int order = 1;
    // flag to generate triangle or qudralateral elements
    int recombined = 0; /* default is traiangle */
    // global mesh size
    double mesh_size = 0.0;
    // global minmuum mesh size
    double                      min_size   = 0.0;
    geo_node_type*              node       = nullptr;
    geo_line_type*              line       = nullptr;
    geo_curve_type*             curve      = nullptr;
    geo_surface_type*           surface    = nullptr;
    geo_group_type*             group      = nullptr;
    geo_EmbededlineGroups_type* linegroups = nullptr;
    // only for .msh file
    int    wall_num = 0;
    char** wallTags;
    // for inp mesh only
    int LineElement_n = 0;
    // for inp mesh only
    char** LineElememtTags;
    Int    LineFlip_n = 0;
    char** LineFlipTags;
    // minmuum angle to delete bad intersected joints, by default is 20 in degree
    Real minmuumAngle = 0.0;
    // minmuum size to delete the bad joints after fargmentation
    Real minmuumSize = 0.0;
    // the iteration to delete bad joints, 1000 by default
    Int iteration = 1000;

    /**
     * \brief Resize (allocate) one of the geometry containers.
     * \param type        Name of the container to resize (e.g. node, line, curve, surface, group).
     * \param size        New element count to allocate.
     * \param initialsize Existing element count to preserve when growing the container.
     */
    void resize(std::string type, Int size, Int initialsize = 0);
    /**
     * \brief Release all dynamically allocated geometry data and reset counters.
     */
    void clear();
};

#endif  // !_OPENDFN_STRUCTURE_GEOMETRY_
