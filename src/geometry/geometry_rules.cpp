/******************************************************************************
 * \file      geometry_rules.cpp
 * \brief     Basic classes parser mesh fron selfdeveloped format parameters.
 *
 * \details   Implementation of basic classes parser mesh fron selfdeveloped
 *            format parameters.
 *
 * \author    Xiaofeng Li
 * \email     xfli@whrsm.ac.cn
 * \date      Created:       June 24, 2020
 * \date      Last modified: 2026-07-20 00:54:42
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

#include <geometry/geometry_rules.h>

namespace ns_geometry {
/**
 * \brief Do DFN quality check for a candidate line against all existing lines.
 *
 * Builds the candidate line from \p container endpoints and loops over every
 * existing geometry line. For intersecting pairs the intersection angle is
 * rejected when smaller than geometry->minmuumAngle; for non-intersecting pairs
 * the candidate is rejected when any endpoint lies closer than
 * geometry->minmuumSize to the other segment. When \p isforJsets is set, lines
 * that are not embedded or are marked to be reserved are skipped.
 *
 * \param geometry   Geometry container providing lines, nodes and thresholds.
 * \param container  Candidate line endpoints as [x1, y1, x2, y2].
 * \param isforJsets Whether to skip non-embedded/reserved boundary lines.
 * \return true if the candidate passes all quality checks; false otherwise.
 */
bool GeometryRules::isLinesWellIntersection(Geometry geometry, Real container[], bool isforJsets) {
    Vector2 LineApoint1, LineApoint2, LineBpoint1, LineBpoint2;
    // get the points on the line want to build
    LineApoint1.create(container[0], container[1]);
    LineApoint2.create(container[2], container[3]);
    // loop all lines
    _for(i, 0, geometry->line_num) {
        // for jsets, not broken the boundaries
        if (isforJsets && (!geometry->line[i].isembeded || geometry->line[i].tobeReserved))
            continue;
        // get the points we want to check
        LineBpoint1.create(geometry->node[geometry->line[i].node_1].x, geometry->node[geometry->line[i].node_1].y);
        LineBpoint2.create(geometry->node[geometry->line[i].node_2].x, geometry->node[geometry->line[i].node_2].y);
        // yes for intersection and angle smaller than a value
        if (doIntersect(LineApoint1, LineApoint2, LineBpoint1, LineBpoint2)) {         
            Real slope1 = LineSlope(LineApoint1, LineApoint2);
            Real slope2 = LineSlope(LineBpoint1, LineBpoint2);
            Real angle  = intersectedAngle(slope1, slope2) * 180 / OpenDFN_PI;
            // for small angle intersection
            if (fabs(angle) < geometry->minmuumAngle)
                return false;

            // to check if two lines are intersected but has short segments
            // ignore because this case will be excluded in fragmentation
        } else {
            // for small vertcial distance if not intersected, the small lines intersected will be deleted in api
            if (PointDistancetoSegment(LineApoint1, LineBpoint1, LineBpoint2) < geometry->minmuumSize)
                return false;
            if (PointDistancetoSegment(LineApoint2, LineBpoint1, LineBpoint2) < geometry->minmuumSize)
                return false;
            if (PointDistancetoSegment(LineBpoint1, LineApoint1, LineApoint2) < geometry->minmuumSize)
                return false;
            if (PointDistancetoSegment(LineBpoint2, LineApoint1, LineApoint2) < geometry->minmuumSize)
                return false;
        }
        // check the distancec between four points
   /*     if (LineApoint1.getDistancefrom(LineBpoint1) <  geometry->minmuumSize) {
            container[0] = LineBpoint1.x;
            container[1] = LineBpoint1.y;
        }
        if (LineApoint1.getDistancefrom(LineBpoint2) <  geometry->minmuumSize) {
            container[0] = LineBpoint2.x;
            container[1] = LineBpoint2.y;
        }
        if (LineApoint2.getDistancefrom(LineBpoint1) <  geometry->minmuumSize) {
            container[2] = LineBpoint1.x;
            container[3] = LineBpoint1.y;
        }
        if (LineApoint2.getDistancefrom(LineBpoint2) <  geometry->minmuumSize) {
            container[2] = LineBpoint2.x;
            container[3] = LineBpoint2.y;
        }*/
    }
    return true;
}

/**
 * \brief Parse joint-set (Jset) keyword parameters into a Jset_data structure.
 *
 * Tokenizes the current input line and walks the keywords, filling the dip,
 * spacing, trace, gap and start-point fields and selecting the matching
 * random_method for each. Sets isNonpersistent when trace/gap keywords appear
 * and useUserdefinedStartPoint when x/y/startPoint keywords appear. Aborts via
 * OPENDFN_EXIT on an unrecognized keyword. Releases the token buffer before
 * returning.
 *
 * \param general   General context supplying the input file/token stream.
 * \param Jsetdata  Output structure populated with parsed parameters; its
 *                  persistence and start-point flags are reset on entry.
 */
void GeometryRules::parseJsets(General general, Jset_data& Jsetdata) {
    Jsetdata.isNonpersistent          = false;
    Jsetdata.useUserdefinedStartPoint = false;
    Keywords Paramter;
    parseStringinLine(general->inputfile, Paramter);
    for (int i = 0; i < Paramter.count; i++) {
        if (isSameString(Paramter.keywords[i], "dip")) {
            // constant dip angle
            Jsetdata.dip[0]     = strtod(Paramter.keywords[i + 1], NULL);
            Jsetdata.dip_method = random_method::RANDOM_CONSTANT;
            i++;
        } else if (isSameString(Paramter.keywords[i], "n_dip")) {
            // normal dip angle, mean and deveration
            Jsetdata.dip[0]     = strtod(Paramter.keywords[i + 1], NULL);
            Jsetdata.dip[1]     = strtod(Paramter.keywords[i + 2], NULL);
            Jsetdata.dip_method = random_method::RANDOM_GAUSSE;
            i                   = i + 2;
        } else if (isSameString(Paramter.keywords[i], "u_dip")) {
            // uniform dip anle, lower and upper values
            Jsetdata.dip[0]     = strtod(Paramter.keywords[i + 1], NULL);
            Jsetdata.dip[1]     = strtod(Paramter.keywords[i + 2], NULL);
            Jsetdata.dip_method = random_method::RANDOM_UNIFORM;
            i                   = i + 2;
        } else if (isSameString(Paramter.keywords[i], "space")) {
            // constant space
            Jsetdata.space[0]     = strtod(Paramter.keywords[i + 1], NULL);
            Jsetdata.space_method = random_method::RANDOM_CONSTANT;
            i++;
        } else if (isSameString(Paramter.keywords[i], "x")) {
            // start points by x and y
            Jsetdata.useUserdefinedStartPoint = true;
            Jsetdata.startPoint.x             = strtod(Paramter.keywords[i + 1], NULL);
            i++;
        } else if (isSamePrefix(Paramter.keywords[i], "startPoint", 3)) {
            // start points
            Jsetdata.useUserdefinedStartPoint = true;
            Jsetdata.startPoint.x             = strtod(Paramter.keywords[i + 1], NULL);
            i++;
            Jsetdata.startPoint.y = strtod(Paramter.keywords[i + 1], NULL);
            i++;
        } else if (isSameString(Paramter.keywords[i], "y")) {
            // start points by x y
            Jsetdata.useUserdefinedStartPoint = true;
            Jsetdata.startPoint.y             = strtod(Paramter.keywords[i + 1], NULL);
            i++;
        } else if (isSameString(Paramter.keywords[i], "n_space")) {
            // normal space
            Jsetdata.space[0]     = strtod(Paramter.keywords[i + 1], NULL);
            Jsetdata.space[1]     = strtod(Paramter.keywords[i + 2], NULL);
            Jsetdata.space_method = random_method::RANDOM_GAUSSE;
            i                     = i + 2;
        } else if (isSameString(Paramter.keywords[i], "n_trace")) {
            // normal space
            Jsetdata.isNonpersistent = true;
            Jsetdata.trace[0]        = strtod(Paramter.keywords[i + 1], NULL);
            Jsetdata.trace[1]        = strtod(Paramter.keywords[i + 2], NULL);
            Jsetdata.trace_method    = random_method::RANDOM_GAUSSE;
            i                        = i + 2;
        } else if (isSameString(Paramter.keywords[i], "trace")) {
            // normal space
            Jsetdata.isNonpersistent = true;
            Jsetdata.trace[0]        = strtod(Paramter.keywords[i + 1], NULL);
            Jsetdata.trace_method    = random_method::RANDOM_CONSTANT;
            i++;
        } else if (isSameString(Paramter.keywords[i], "n_gap")) {
            // normal space
            Jsetdata.isNonpersistent = true;
            Jsetdata.gap[0]          = strtod(Paramter.keywords[i + 1], NULL);
            Jsetdata.gap[1]          = strtod(Paramter.keywords[i + 2], NULL);
            Jsetdata.gap_method      = random_method::RANDOM_GAUSSE;
            i                        = i + 2;
        } else if (isSameString(Paramter.keywords[i], "gap")) {
            // normal space
            Jsetdata.isNonpersistent = true;
            Jsetdata.gap[0]          = strtod(Paramter.keywords[i + 1], NULL);
            Jsetdata.gap_method      = random_method::RANDOM_CONSTANT;
            i++;
        } else {
            OpenDFNMessage::RuntimebackStringString("Error: no found Paramter.keywords: ", Paramter.keywords[i]);
            OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
        }
    }
    Paramter.clean();
}

/**
 * \brief Verify a geometry group tag is unique, aborting on a duplicate.
 *
 * Scans all existing geometry groups; if any already carries \p tag, emits an
 * error and terminates via OPENDFN_EXIT.
 *
 * \param geometry  Geometry container holding the current group list.
 * \param tag       Candidate group tag to validate for uniqueness.
 */
void GeometryRules::chechGeomtryGroups(Geometry geometry, char* tag) {
    for (int i = 0; i < geometry->group_num; i++) {
        if (isSameString(geometry->group[i].tag, tag)) {
            OpenDFNMessage::RuntimeStringStringString("Error: geometry group ", tag, "is already existed.");
            OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
        }
    }
}

/**
 * \brief Parse discrete-fracture-network (DFN) keyword parameters.
 *
 * Tokenizes the current input line and fills the dip and length descriptors and
 * the intensity/count controls, choosing the random_method for dip (dip/n_dip/
 * u_dip) and length (length/n_length/f_length/p_length) and setting the
 * generation method: count (\p method=0), p21 (\p method=1) or p10
 * (\p method=2). Aborts via OPENDFN_EXIT on an unrecognized keyword. Releases
 * the token buffer before returning.
 *
 * \param general        General context supplying the input token stream.
 * \param dip            Output dip descriptor [constant]/[mean,dev]/[low,up].
 * \param length         Output length descriptor, same layout as \p dip.
 * \param count          Output number of fractures (used when \p method=0).
 * \param method         Output generation method selector (0=count, 1=p21,
 *                       2=p10).
 * \param p21            Output 2D fracture intensity (used when \p method=1).
 * \param p10            Output 1D fracture intensity (used when \p method=2).
 * \param dip_method     Output random-sampling method chosen for the dip.
 * \param length_method  Output random-sampling method chosen for the length.
 */
void GeometryRules::parseDFNs(General        general,
                              Real           dip[],
                              Real           length[],
                              Int&           count,
                              Int&           method,
                              Real&          p21,
                              Real&          p10,
                              random_method& dip_method,
                              random_method& length_method) {
    Keywords Paramter;
    parseStringinLine(general->inputfile, Paramter);
    for (int i = 0; i < Paramter.count; i++) {
        if (isSameString(Paramter.keywords[i], "dip")) /* constant dip */
        {
            // constant value
            dip[0]     = strtod(Paramter.keywords[i + 1], NULL);
            dip_method = random_method::RANDOM_CONSTANT;
            i++;
        } else if (isSameString(Paramter.keywords[i], "n_dip")) /* normal dip */
        {
            // mean and deviation
            dip[0]     = strtod(Paramter.keywords[i + 1], NULL);
            dip[1]     = strtod(Paramter.keywords[i + 2], NULL);
            dip_method = random_method::RANDOM_GAUSSE;
            i          = i + 2;
        } else if (isSameString(Paramter.keywords[i], "u_dip")) /* uniform dip */
        {
            // lower and upper dip
            dip[0]     = strtod(Paramter.keywords[i + 1], NULL);
            dip[1]     = strtod(Paramter.keywords[i + 2], NULL);
            dip_method = random_method::RANDOM_UNIFORM;
            i          = i + 2;
        } else if (isSameString(Paramter.keywords[i], "length")) /* constant length */
        {
            length[0]     = strtod(Paramter.keywords[i + 1], NULL);
            length_method = random_method::RANDOM_CONSTANT;
            i++;
        } else if (isSameString(Paramter.keywords[i], "n_length")) /* normal length */
        {
            length[0]     = strtod(Paramter.keywords[i + 1], NULL);
            length[1]     = strtod(Paramter.keywords[i + 2], NULL);
            length_method = random_method::RANDOM_GAUSSE;
            i             = i + 2;
        } else if (isSameString(Paramter.keywords[i], "f_length")) /* fisher length */
        {
            length[0]     = strtod(Paramter.keywords[i + 1], NULL);
            length[1]     = strtod(Paramter.keywords[i + 2], NULL);
            length_method = random_method::RANDOM_FISHER;
            i             = i + 2;
        } else if (isSameString(Paramter.keywords[i], "p_length")) /* power length */
        {
            length[0]     = strtod(Paramter.keywords[i + 1], NULL);
            length[1]     = strtod(Paramter.keywords[i + 2], NULL);
            length_method = random_method::RANDOM_POWER;
            i             = i + 2;
        } else if (isSameString(Paramter.keywords[i], "count")) /* count method */
        {
            count  = strtod(Paramter.keywords[i + 1], NULL);
            method = 0;
            i++;
        } else if (isSameString(Paramter.keywords[i], "p21")) /* p21 method */
        {
            p21    = strtod(Paramter.keywords[i + 1], NULL);
            method = 1;
            i      = i + 1;
        } else if (isSameString(Paramter.keywords[i], "p10")) /* p10 method */
        {
            p10    = strtod(Paramter.keywords[i + 1], NULL);
            method = 2;
            i      = i + 1;
        } else {
            OpenDFNMessage::RuntimebackStringString("Error: no found Paramter.keywords: ", Paramter.keywords[i]);
            OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
        }
    }
    Paramter.clean();
}

/**
 * \brief Ensure a line-to-node group with the given tag exists, creating it if
 *        absent.
 *
 * Searches geometry->group for \p newToolTag; if not found, grows the group
 * array, copies the tag, sets the group type to GroupType::FROMLINETAGS and
 * increments geometry->group_num.
 *
 * \param geometry    Geometry container whose group list may be grown.
 * \param newToolTag  Tag of the group to look up or create.
 */
void GeometryRules::addtoLinetoNodeGroups(Geometry geometry, char* newToolTag) {
    bool findtag = false;
    _for(i, 0, geometry->group_num) {
        if (isSameString(geometry->group[i].tag, newToolTag))
            findtag = true;
    }
    // build a new line groups if not find
    if (!findtag) {
        buildmemory(&geometry->group, (geometry->group_num + 1), geometry->group_num);
        // copy the tag
        copyChars(geometry->group[geometry->group_num].tag, newToolTag);
        geometry->group[geometry->group_num].type = GroupType::FROMLINETAGS;
        // update
        geometry->group_num++;
    }
}

/**
 * \brief Ensure a line group with the given tag exists, creating it if absent.
 *
 * Searches geometry->linegroups for \p newToolTag; if not found, grows the
 * linegroups array, copies the tag and increments geometry->linegroups_num.
 *
 * \param geometry    Geometry container whose linegroups list may be grown.
 * \param newToolTag  Tag of the line group to look up or create.
 */
void GeometryRules::addtoLineGroups(Geometry geometry, char* newToolTag) {
    bool findtag = false;
    _for(i, 0, geometry->linegroups_num) {
        if (isSameString(geometry->linegroups[i].tag, newToolTag))
            findtag = true;
    }
    // build a new line groups if not find
    if (!findtag) {
        buildmemory(&geometry->linegroups, (geometry->linegroups_num + 1), geometry->linegroups_num);
        // copy the tag
        copyChars(geometry->linegroups[geometry->linegroups_num].tag, newToolTag);
        // update
        geometry->linegroups_num++;
    }
}

/**
 * \brief Register a line as a boolean tool on a named surface.
 *
 * Locates the surface tagged \p ObjectTag, grows its boollines array and
 * appends a (dimension=1, tag=n_line+1) pair, then increments n_boollines.
 * Emits an error message if no matching surface is found.
 *
 * \param geometry   Geometry container holding the surfaces.
 * \param ObjectTag  Tag of the target surface.
 * \param n_line     Zero-based line index; stored as n_line + 1.
 */
void GeometryRules::addLinetoSufaceBoolGroups(Geometry geometry, char* ObjectTag, Int n_line) {
    // to find the object
    bool findsurface = false;
    for (int j = 0; j < geometry->surface_num; j++) {
        if (isSameString(geometry->surface[j].tag, ObjectTag)) {
            // add new bool tag
            buildmemory(&geometry->surface[j].boollines, (2 * geometry->surface[j].n_boollines + 2),
                        2 * geometry->surface[j].n_boollines);
            // dimesion is line
            geometry->surface[j].boollines[2 * geometry->surface[j].n_boollines] = 1;
            // add tag
            geometry->surface[j].boollines[2 * geometry->surface[j].n_boollines + 1] = n_line + 1;
            // update count
            geometry->surface[j].n_boollines++;
            // mark the bool
            findsurface = true;
            break;
        }
    }
    if (!findsurface) {
        OpenDFNMessage::RuntimebackStringString("Error: no found object surface: ", ObjectTag);
    }
}

/**
 * \brief Register a surface as a remove tool on a named surface.
 *
 * Locates the surface tagged \p ObjectTag, grows its removetool array and
 * appends a (dimension=2, tag=n_surface+1) pair, then increments n_removetool.
 * Emits an error message if no matching surface is found.
 *
 * \param geometry   Geometry container holding the surfaces.
 * \param ObjectTag  Tag of the target surface.
 * \param n_surface  Zero-based surface index; stored as n_surface + 1.
 */
void GeometryRules::addLinetoSufaceRemoveGroups(Geometry geometry, char* ObjectTag, Int n_surface) {
    // to find the object
    bool findsurface = false;
    for (int j = 0; j < geometry->surface_num; j++) {
        if (isSameString(geometry->surface[j].tag, ObjectTag)) {
            // add new remove tag
            buildmemory(&geometry->surface[j].removetool, (2 * geometry->surface[j].n_removetool + 2),
                        2 * geometry->surface[j].n_removetool);
            // dimesion
            geometry->surface[j].removetool[2 * geometry->surface[j].n_removetool] = 2;
            // add tag
            geometry->surface[j].removetool[2 * geometry->surface[j].n_removetool + 1] = n_surface + 1;
            // update count
            geometry->surface[j].n_removetool++;
            // mark the bool
            findsurface = true;
            break;
        }
    }
    if (!findsurface) {
        OpenDFNMessage::RuntimebackStringString("Error: no found object surface: ", ObjectTag);
    }
}

/**
 * \brief Test whether a point lies within the axis-aligned bounding box of a
 *        segment.
 *
 * Returns true when \p point falls inside the min/max x and y range spanned by
 * \p start and \p end. Used to accept a computed intersection point that must
 * fall within a rectangle edge.
 *
 * \param point  Point to test.
 * \param start  First endpoint of the segment.
 * \param end    Second endpoint of the segment.
 * \return true if \p point is inside the segment's bounding box.
 */
bool GeometryRules::isPointOnLineSegment(const Vector2& point, const Vector2& start, const Vector2& end) {
    Real minX = std::min(start.x, end.x);
    Real maxX = std::max(start.x, end.x);
    Real minY = std::min(start.y, end.y);
    Real maxY = std::max(start.y, end.y);

    return (point.x >= minX && point.x <= maxX && point.y >= minY && point.y <= maxY);
}

/**
 * \brief Find the two rectangle-boundary intersection points of a ray.
 *
 * Forms the infinite line through \p startPoint with slope tan(\p angle) and
 * computes its intersection with each of the four rectangle edges AB, BC, CD
 * and DA. Every candidate that falls within the corresponding edge's bounding
 * box is appended to \p Intersection as an [x, y] pair.
 *
 * \param startPoint   Origin of the ray.
 * \param angle        Ray angle in radians; slope is tan(angle).
 * \param A            Rectangle corner A.
 * \param B            Rectangle corner B.
 * \param C            Rectangle corner C.
 * \param D            Rectangle corner D.
 * \param Intersection Output buffer receiving up to two [x, y] pairs.
 * \return true if exactly two boundary intersection points were found, false
 *         otherwise.
 */
bool GeometryRules::findIntersectionPoints(const Vector2& startPoint,
                                           Real           angle,
                                           const Vector2& A,
                                           const Vector2& B,
                                           const Vector2& C,
                                           const Vector2& D,
                                           Real           Intersection[]) {
    // Calculate the slope of the line
    Real slope = tan(angle);

    // Calculate the intersection points with the rectangle edges
    Vector2 intersectionAB, intersectionBC, intersectionCD, intersectionDA;

    // Calculate intersection with edge AB
    intersectionAB.x = startPoint.x + (A.y - startPoint.y) / slope;
    intersectionAB.y = A.y;

    // Calculate intersection with edge BC
    intersectionBC.x = B.x;
    intersectionBC.y = slope * B.x + startPoint.y - slope * startPoint.x;

    // Calculate intersection with edge CD
    intersectionCD.x = startPoint.x + (C.y - startPoint.y) / slope;
    intersectionCD.y = C.y;

    // Calculate intersection with edge DA
    intersectionDA.x = D.x;
    intersectionDA.y = slope * D.x + startPoint.y - slope * startPoint.x;

    Int nodeI = 0;
    // Output the intersection points within the rectangle edges
    if (isPointOnLineSegment(intersectionAB, A, B)) {
        Intersection[2 * nodeI]     = intersectionAB.x;
        Intersection[2 * nodeI + 1] = intersectionAB.y;
        nodeI++;
    }

    if (isPointOnLineSegment(intersectionBC, B, C)) {
        Intersection[2 * nodeI]     = intersectionBC.x;
        Intersection[2 * nodeI + 1] = intersectionBC.y;
        nodeI++;
    }

    if (isPointOnLineSegment(intersectionCD, C, D)) {
        Intersection[2 * nodeI]     = intersectionCD.x;
        Intersection[2 * nodeI + 1] = intersectionCD.y;
        nodeI++;
    }

    if (isPointOnLineSegment(intersectionDA, D, A)) {
        Intersection[2 * nodeI]     = intersectionDA.x;
        Intersection[2 * nodeI + 1] = intersectionDA.y;
        nodeI++;
    }

    if (nodeI == 2)
        return true;
    else
        return false;
}
}  // namespace ns_geometry
