/******************************************************************************
 * \file      geometry_keyword_parser.h
 * \brief     Basic classes parser mesh fron selfdeveloped format parameters.
 *
 * \details   Declarations for basic classes parser mesh fron selfdeveloped
 *            format parameters.
 *
 * \author    Xiaofeng Li
 * \email     xfli@whrsm.ac.cn
 * \date      Created:       June 24, 2020
 * \date      Last modified: 2026-07-20 00:03:59
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

#ifndef SRC_MESH_GROUP_UTILS_MODULE_H
#define SRC_MESH_GROUP_UTILS_MODULE_H

#include "common/memory_manager.h"
#include "common/opendfn_message.h"
#include "mesh/mesh_utils.h"
#include "core/opendfn_context.h"
using namespace ns_common;

namespace ns_mesh {
/**
 * \brief Stateless helper collection for parsing group "range" definitions from input files.
 *
 * GeometryKeywordParser provides static routines that read the keyword tokens following a group
 * tag and translate them into geometric parameters (box, circle, plane, polygon,
 * ellipse, arc or coordinate). It also resolves the range method enum and checks that
 * group tags are unique across the mesh. All members are static; the class holds no state.
 */
class GeometryKeywordParser {
private:
    /* data */
public:
    GeometryKeywordParser(/* args */) {}
    ~GeometryKeywordParser() {}
    /**
     * common function to parse the keywords of range.
     *
     * \param file
     * \param container
     * \param range
     */
    static void parseRangeKeywords(FILE* file, Real container[], rangeMethod& range);
    static void parseRangeKeywords(FILE* file, Real container[], rangeMethod& range, std::vector<Vector2>& polygon );
    /**
     * parse range of mpm.
     *
     * \param file
     * \param container
     * \param range
     * \param polygon
     * \param size
     */
    static void parseRangeKeywords(FILE* file, Real container[], rangeMethod& range, std::vector<Vector2>& polygon, Int size[]);

    /** \brief Extract box bounds (and optional cell sizes) from parsed keyword tokens. \param keyword parsed tokens \param container output [xmin,xmax,ymin,ymax] \param size output [xcell,ycell]. */
    static void pushSquareKeywords(Keywords& keyword, Real container[], Int size[] = {});
    /** \brief Extract circle centre/radius (and optional cell sizes) from parsed keyword tokens. \param keyword parsed tokens \param container output [x,y,radius] \param size output [xcell,ycell]. */
    static void pushCircleKeywords(Keywords& keyword, Real container[], Int size[] = {});
    /** \brief Extract plane two-point parameters from parsed keyword tokens. \param keyword parsed tokens \param container output [x0,y0,x1,y1]. */
    static void pushPlaneKeywords(Keywords& keyword, Real container[]);

    /** \brief Read one line and extract circle geometry. \param file input stream \param container output [x,y,radius,segments]. */
    static void pushCircleGeometryKeywords(FILE* file, Real container[]);
    /** \brief Read one line and extract ellipse geometry. \param file input stream \param container output [x,y,maxradius,minradius,theta,segments]. */
    static void pushEllipseGeometryKeywords(FILE* file, Real container[]);
    /** \brief Read one line and extract arc geometry. \param file input stream \param container output [x,y,radius,theta1,theta2,segments]. */
    static void pushArcGeometryKeywords(FILE* file, Real container[]);
    /** \brief Read one line and extract plane two-point parameters. \param file input stream \param container output [x0,y0,x1,y1]. */
    static void pushPlaneKeywords(FILE* file, Real container[]);
    /** \brief Read one line and extract plane parameters plus segment count. \param file input stream \param container output [x0,y0,x1,y1] \param segment output segment count. */
    static void pushPlaneKeywords(FILE* file, Real container[], Int& segment);
    /** \brief Read one line and extract box bounds. \param file input stream \param container output [xmin,xmax,ymin,ymax]. */
    static void pushSquareKeywords(FILE* file, Real container[]);
    /** \brief Extract a single (x,y) coordinate from parsed keyword tokens. \param keyword parsed tokens \param container output [x,y]. */
    static void pushCoordKeywords(Keywords& keyword, Real container[]);
    /** \brief Parse a polygon range, reading its vertices (over multiple lines if needed) into \p polygon. \param file input stream \param keyword parsed tokens, grown in place \param polygon output vertex list. */
    static void pushPolygonGeometryKeywords(FILE* file, Keywords& keyword, std::vector<Vector2>& polygon);
    /** \brief Parse a polygon range with a cell resolution, reading vertices into \p polygon. \param file input stream \param keyword parsed tokens, grown in place \param polygon output vertex list \param size output [xcell,ycell]. */
    static void pushPolygonGeometryKeywords(FILE* file, Keywords& keyword, std::vector<Vector2>& polygon, Int size[]);

    /**
     * get the range method.
     *
     * \param grouptags
     * \param groupcount
     * \param range
     */
    static void getRangeMethod(Keywords& keyword, rangeMethod& range);
};
}  // namespace ns_mesh

#endif  // SRC_MESH_GROUP_UTILS_MODULE_H
