/******************************************************************************
 * \file      opendfn_geometry_module.h
 * \brief     Basic classes cotain the functions to create gmsh geometry and
 *            DFNs, it is implemented gmsh API to create 2D and 3D mesh, it will
 *            be able to export or import more standard mesh types for OpenDFN,
 *            in general, OpenDFN supports to import .msh, .inp, .fdem, .jpg,
 *            .tess, .stl(3D only), .dxf(2D only), .step and .geo for the
 *            prepocessing.
 *
 * \details   Declarations for basic classes cotain the functions to create gmsh
 *            geometry and DFNs, it is implemented gmsh API to create 2D and 3D
 *            mesh, it will be able to export or import more standard mesh types
 *            for OpenDFN, in general, OpenDFN supports to import .msh, .inp,
 *            .fdem, .jpg, .tess, .stl(3D only), .dxf(2D only), .step and .geo
 *            for the prepocessing.
 *
 * \author    Xiaofeng Li
 * \email     xfli@whrsm.ac.cn
 * \date      Created:       June 24, 2020
 * \date      Last modified: 2026-07-20 00:53:23
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

#ifndef SRC_GEOMETRY_OPENDFN_PUSH_GEOMETRY_H
#define SRC_GEOMETRY_OPENDFN_PUSH_GEOMETRY_H
#include <common/opendfn_macros.h>
#include <geometry_rules.h>
#include "common/memory_manager.h"
#include "common/opendfn_message.h"
#include "external/gsl/gsl_randist.h"
#include "external/gsl/gsl_rng.h"
#include "mesh/mesh_utils.h"
#include "core/opendfn_context.h"
using namespace ns_common;
using namespace ns_mesh;

namespace ns_geometry {
/**
 * \brief Preprocessing module that builds OpenDFN geometry and meshes via the Gmsh API.
 *
 * Collects the built-in geometry commands (points, lines, curves, surfaces,
 * joints, DFNs, particles), applies node grouping rules, drives Gmsh meshing,
 * and imports/exports the resulting mesh. All entry points are static helpers
 * operating on a shared \c Geometry description.
 */
class GeometryBuilder : public GeometryRules {
private:
    /* data */
public:
    GeometryBuilder() {}
    ~GeometryBuilder() {}
    /**
     * \brief Push a rectangular surface into the geometry description.
     * \param general    Global state providing the parsed command arguments.
     * \param geometry   Geometry to append the square/rectangle to (mutated).
     * \param embed_flag Boolean operation used when combining with existing entities.
     *
     * example: odfn.geometry.square 'down' -0.2 1.2 -0.05 -0.0
     */
    static void push_geometry_square(General general, Geometry geometry, OperationType embed_flag);

    /**
     * \brief Push a polygon surface (from a list of vertices) into the geometry.
     * \param general    Global state providing the parsed command arguments.
     * \param geometry   Geometry to append the polygon to (mutated).
     * \param embed_flag Boolean operation used when combining with existing entities.
     */
    static void push_geometry_polygon(General general, Geometry geometry, OperationType embed_flag);

    /**
     * \brief Push a surface defined by a coordinate table into the geometry.
     * \param general    Global state providing the parsed command arguments.
     * \param geometry   Geometry to append the entity to (mutated).
     * \param embed_flag Boolean operation used when combining with existing entities.
     */
    static void push_geometry_table(General general, Geometry geometry, OperationType embed_flag);

    /**
     * \brief Add joints read from the OpenDFN self-developed (SRC) source format.
     * \param general  Global state providing the parsed command arguments.
     * \param geometry Geometry to append the joints to (mutated).
     */
    static void pushJointFromSRC(General general, Geometry geometry);

    /**
     * \brief Add lines read from a coordinate table.
     * \param general  Global state providing the parsed command arguments.
     * \param geometry Geometry to append the lines to (mutated).
     */
    static void pushLineFromTable(General general, Geometry geometry);

    /**
     * \brief Push a circular surface/curve into the geometry description.
     * \param general    Global state providing the parsed command arguments.
     * \param geometry   Geometry to append the circle to (mutated).
     * \param embed_flag Boolean operation used when combining with existing entities.
     */
    static void push_geometry_circle(General general, Geometry geometry, OperationType embed_flag);

    /**
     * \brief Push an elliptical surface/curve into the geometry description.
     * \param general    Global state providing the parsed command arguments.
     * \param geometry   Geometry to append the ellipse to (mutated).
     * \param embed_flag Boolean operation used when combining with existing entities.
     */
    static void push_geometry_ellipse(General general, Geometry geometry, OperationType embed_flag);

    /**
     * \brief Push a circular/elliptical arc curve into the geometry description.
     * \param general  Global state providing the parsed command arguments.
     * \param geometry Geometry to append the arc to (mutated).
     */
    static void push_geometry_arc(General general, Geometry geometry);

    /**
     * \brief Push a single joint (fracture line) into the geometry description.
     * \param general  Global state providing the parsed command arguments.
     * \param geometry Geometry to append the joint to (mutated).
     * \return True if the joint was successfully added, false otherwise.
     */
    static bool push_geometry_joint(General general, Geometry geometry);

    /**
     * \brief Push a single geometry point into the geometry description.
     * \param general  Global state providing the parsed command arguments.
     * \param geometry Geometry to append the point to (mutated).
     */
    static void push_geometry_point(General general, Geometry geometry);

    /**
     * \brief Apply the global mesh size to every point entity of the current Gmsh model.
     * \param geometry Geometry providing the global mesh size to apply.
     * \param ierr     Gmsh error code, updated by the underlying Gmsh calls.
     */
    static void setGlobalMeshSize(Geometry geometry, int& ierr);

    /**
     * \brief Assign per-group mesh sizes to Gmsh point entities selected by geometry groups.
     * \param geometry Geometry holding the group definitions and their mesh sizes.
     * \param ierr     Gmsh error code, updated by the underlying Gmsh calls.
     */
    static void setMeshSizefromGroups(Geometry geometry, int& ierr);

    /**
     * \brief Create Gmsh point physical groups for geometry groups defined from coordinates.
     * \param geometry Geometry holding the group definitions (isInsertedPoints flags cleared).
     * \param ierr     Gmsh error code, updated by the underlying Gmsh calls.
     */
    static void addPhysicalSetsforPoints(Geometry geometry, int& ierr);

    /**
     * \brief Register named line groups so broken/embedded lines can be grouped by tag.
     * \param general    Global state providing the parsed command arguments.
     * \param geometry   Geometry to append the line groups to (mutated).
     * \param groupcount Number of group tags provided.
     * \param grouptags  Array of group tag strings.
     */
    static void pushGeometryLineGroups(General general, Geometry geometry, const Int groupcount, char** grouptags);

    /**
     * \brief Push a line entity into the geometry description.
     * \param general  Global state providing the parsed command arguments.
     * \param geometry Geometry to append the line to (mutated).
     */
    static void push_geometry_line(General general, Geometry geometry);

    /**
     * \brief Push a joint set (stochastic fracture family) into the geometry.
     * \param general  Global state providing the parsed command arguments.
     * \param geometry Geometry to append the joint set to (mutated).
     */
    static void push_geometry_jset(General general, Geometry geometry);

    /**
     * \brief Push a discrete fracture network (DFN) into the geometry description.
     * \param general  Global state providing the parsed command arguments.
     * \param geometry Geometry to append the DFN to (mutated).
     */
    static void push_geometry_DFN(General general, Geometry geometry);

    /**
     * \brief Group geometry nodes that fall inside a rectangular box range.
     * \param geometry Geometry to tag (mutated).
     * \param tag      Group tag assigned to matching nodes.
     * \param x_lef    Left  (minimum x) box bound.
     * \param x_rig    Right (maximum x) box bound.
     * \param y_bot    Bottom (minimum y) box bound.
     * \param y_top    Top    (maximum y) box bound.
     */
    static void getGeoNodeGroupInBoxRange(Geometry geometry, char* tag, double x_lef, double x_rig, double y_bot, double y_top);

    /**
     * \brief Group geometry nodes that lie on the border of a rectangular box range.
     * \param geometry Geometry to tag (mutated).
     * \param tag      Group tag assigned to matching nodes.
     * \param x_lef    Left  (minimum x) box bound.
     * \param x_rig    Right (maximum x) box bound.
     * \param y_bot    Bottom (minimum y) box bound.
     * \param y_top    Top    (maximum y) box bound.
     */
    static void
    getGeoNodeGroupOnBoxRange(Geometry geometry, char* tag, double x_lef, double x_rig, double y_bot, double y_top);
    /**
     * \brief Group geometry nodes that fall outside a rectangular box range.
     * \param geometry Geometry to tag (mutated).
     * \param tag      Group tag assigned to matching nodes.
     * \param x_lef    Left  (minimum x) box bound.
     * \param x_rig    Right (maximum x) box bound.
     * \param y_bot    Bottom (minimum y) box bound.
     * \param y_top    Top    (maximum y) box bound.
     */
    static void
    getGeoNodeGroupOutBoxRange(Geometry geometry, char* tag, double x_lef, double x_rig, double y_bot, double y_top);

    /**
     * \brief Group geometry nodes lying on the left half-space of a line through (x1,y1)-(x2,y2).
     * \param geometry Geometry to tag (mutated).
     * \param tag      Group tag assigned to matching nodes.
     * \param x1       X coordinate of the first point defining the plane/line.
     * \param y1       Y coordinate of the first point defining the plane/line.
     * \param x2       X coordinate of the second point defining the plane/line.
     * \param y2       Y coordinate of the second point defining the plane/line.
     */
    static void getGeoNodeGroupOnPlaneLeft(Geometry geometry, char* tag, double x1, double y1, double x2, double y2);

    /**
     * \brief Group geometry nodes lying on the right half-space of a line through (x1,y1)-(x2,y2).
     * \param geometry Geometry to tag (mutated).
     * \param tag      Group tag assigned to matching nodes.
     * \param x1       X coordinate of the first point defining the plane/line.
     * \param y1       Y coordinate of the first point defining the plane/line.
     * \param x2       X coordinate of the second point defining the plane/line.
     * \param y2       Y coordinate of the second point defining the plane/line.
     */
    static void getGeoNodeGroupOnPlaneRight(Geometry geometry, char* tag, double x1, double y1, double x2, double y2);

    /**
     * \brief Group geometry nodes lying on a line/plane through (x1,y1)-(x2,y2).
     * \param geometry Geometry to tag (mutated).
     * \param tag      Group tag assigned to matching nodes.
     * \param x1       X coordinate of the first point defining the plane/line.
     * \param y1       Y coordinate of the first point defining the plane/line.
     * \param x2       X coordinate of the second point defining the plane/line.
     * \param y2       Y coordinate of the second point defining the plane/line.
     */
    static void getGeoNodeGroupOnPlane(Geometry geometry, char* tag, double x1, double y1, double x2, double y2);

    /**
     * \brief Group geometry nodes that fall inside a circle.
     * \param geometry Geometry to tag (mutated).
     * \param tag      Group tag assigned to matching nodes.
     * \param x0       Circle centre X coordinate.
     * \param y0       Circle centre Y coordinate.
     * \param radius   Circle radius.
     */
    static void getGeoNodeGroupInCircleRange(Geometry geometry, char* tag, double x0, double y0, double radius);

    /**
     * \brief Group geometry nodes that fall outside a circle.
     * \param geometry Geometry to tag (mutated).
     * \param tag      Group tag assigned to matching nodes.
     * \param x0       Circle centre X coordinate.
     * \param y0       Circle centre Y coordinate.
     * \param radius   Circle radius.
     */
    static void getGeoNodeGroupOutCircleRange(Geometry geometry, char* tag, double x0, double y0, double radius);

    /**
     * \brief Group geometry nodes lying on the circumference of a circle.
     * \param geometry Geometry to tag (mutated).
     * \param tag      Group tag assigned to matching nodes.
     * \param x0       Circle centre X coordinate.
     * \param y0       Circle centre Y coordinate.
     * \param radius   Circle radius.
     */
    static void getGeoNodeGroupOnCircle(Geometry geometry, char* tag, double x0, double y0, double radius);

    /**
     * \brief Group the geometry node located at an explicit coordinate.
     * \param geometry Geometry to tag (mutated).
     * \param tag      Group tag assigned to the matching node.
     * \param x0       Target node X coordinate.
     * \param y0       Target node Y coordinate.
     */
    static void getGeoNodeGroupbyCoord(Geometry geometry, char* tag, double x0, double y0);

    /**
     * \brief Parse and apply a mesh-size command to the geometry.
     * \param geometry Geometry whose mesh size fields are set (mutated).
     * \param general  Global state providing the parsed command arguments.
     * \param keyword  Command keyword identifying the mesh-size operation.
     */
    static void push_mesh_size(Geometry geometry, General general, char* keyword);

    /**
     * \brief Parse and apply a surface-recombination command (triangles to quads).
     * \param geometry Geometry whose surface recombine flags are set (mutated).
     * \param general  Global state providing the parsed command arguments.
     * \param keyword  Command keyword identifying the recombination target.
     */
    static void push_surface_recombined(Geometry geometry, General general, char* keyword);
    /**
     * gmsh interface to call api and do mesh.
     *
     * \param opendfn
     * \param geometry
     * \param argv
     * \param keyword
     * \param write_flag
     */
    static void generateMesh(OpenDFNHandle context, Geometry geometry, char** argv, char* keyword, int writeFlag);
    /**
     * free up all variables in geometry.
     *
     * \param geometry
     */
    /**
     * create realistic DFNs from image.
     *
     * \param general
     * \param geometry
     */
    static void createRealisticDFNs(General general, Geometry geometry);
    /**
     * add the list of element sets to determine the 2d elements are shells, walls, and not DFNs.
     * DFNs are also 2D elements, and are imported by default. for .inp mesh only
     * \attention This function should be called
     * before the mesh is imported.
     *
     * \param general
     * \param geometry
     * \param groupcount
     * \param grouptags
     */
    static void addGeometryDFNTags(General general, Geometry geometry, const Int groupcount, char** grouptags);
    /**
     * to flip the direction of 2D wall. The wall direction to act with entities or
     * particles should be determined manually.
     * The default is postive to counter clockwise of the plane,
     * the node list will be reversed when this function is called.
     *
     * \attenion This function should be called before the mesh
     * info is imported.
     *
     *
     * \param general
     * \param geometry
     * \param groupcount
     * \param grouptags
     */
    static void addGeometryFlipTags(General general, Geometry geometry, const Int groupcount, char** grouptags);
};
}  // namespace ns_geometry
#endif  // !SRC_GEOMETRY_OPENDFN_PUSH_GEOMETRY_H
