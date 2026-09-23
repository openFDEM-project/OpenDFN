/******************************************************************************
 * \file      geometry_rules.h
 * \brief     Basic classes parser mesh fron selfdeveloped format parameters.
 *
 * \details   Declarations for basic classes parser mesh fron selfdeveloped
 *            format parameters.
 *
 * \author    Xiaofeng Li
 * \email     xfli@whrsm.ac.cn
 * \date      Created:       June 24, 2020
 * \date      Last modified: 2026-07-20 00:52:39
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

#ifndef SOLID_SOLID_MECHANICS_GEOMETRY_UTILS_CLASS_
#define SOLID_SOLID_MECHANICS_GEOMETRY_UTILS_CLASS_
#include <algorithm>
#include "common/opendfn_random.h"
#include <mesh/mesh_utils.h>
#include "core/opendfn_context.h"
using namespace ns_mesh;
using namespace ns_common;

namespace ns_geometry {
/**
 * \brief Parsed parameters describing a single joint set (Jset).
 *
 * Aggregates the statistical descriptors used to stochastically generate the
 * lines of a joint set: dip angle, spacing, trace length and gap, together
 * with the random-sampling method selected for each descriptor. Also records
 * whether the set is non-persistent and whether a user-defined start point is
 * used. Each numeric field is a two-element array holding either a constant
 * value ([0]) or a (mean, deviation) / (lower, upper) pair depending on the
 * associated random_method.
 */
struct Jset_data {
    Real          dip[2];
    random_method dip_method = random_method::RANDOM_NULL;
    Real          space[2];
    random_method space_method = random_method::RANDOM_NULL;
    Real          trace[2];
    random_method trace_method = random_method::RANDOM_NULL;
    Real          gap[2];
    random_method gap_method               = random_method::RANDOM_NULL;
    bool          isNonpersistent          = false;
    bool          useUserdefinedStartPoint = false;
    Vector2       startPoint;
    Jset_data() {}
};

/**
 * \brief Static geometry helper utilities for building and validating
 *        fracture/joint geometry (Jsets, joints, DFN and rDFN tools).
 *
 * Provides quality checks for candidate lines, parsers for joint-set and DFN
 * input keywords, helpers for registering lines/surfaces into geometry groups
 * and boolean/remove tool lists, and low-level geometric predicates
 * (point-on-segment tests, line/rectangle intersection). All members are
 * static; the class carries no instance state and inherits MeshUtils for
 * geometric predicates.
 */
class GeometryRules : public MeshUtils {
private:
    /* data */
public:
    GeometryRules(/* args */) {}
    ~GeometryRules() {}
    /**
     * \brief Do DFN quality check for a candidate line against existing lines,
     *        used by the jset, joint, DFN and rDFN tools.
     *
     * Iterates over all existing geometry lines and rejects the candidate line
     * (endpoints stored in \p container) when it intersects an existing line at
     * an angle smaller than the configured minimum, or when it passes within
     * the minimum feature size of an existing segment without intersecting.
     *
     * \param geometry   Geometry container holding the current lines/nodes and
     *                   the minimum angle/size thresholds.
     * \param container  Candidate line endpoints as [x1, y1, x2, y2].
     * \param isforJsets  When true, only embedded, non-reserved lines are
     *                    considered (jset boundaries are skipped).
     * \return true if the candidate line is well separated/intersected and may
     *         be kept; false if it violates a quality threshold.
     */
    static bool isLinesWellIntersection(Geometry geometry, Real container[], bool isforJsets);

    /**
     * \brief Parse joint-set (Jset) keyword parameters from the current input
     *        line into a Jset_data structure.
     *
     * Reads the next keyword line from \p general->inputfile and fills the dip,
     * spacing, trace, gap and start-point fields of \p Jsetdata, selecting the
     * matching random_method for each. Recognizes keywords such as dip/n_dip/
     * u_dip, space/n_space, trace/n_trace, gap/n_gap, x/y/startPoint. Aborts the
     * program via OPENDFN_EXIT on an unknown keyword.
     *
     * \param general   General context supplying the input file/token stream.
     * \param Jsetdata  Output structure populated with the parsed parameters;
     *                  its persistence and start-point flags are reset first.
     */
    static void parseJsets(General general, Jset_data& Jsetdata);

    /**
     * \brief Parse discrete-fracture-network (DFN) keyword parameters from the
     *        current input line.
     *
     * Reads the next keyword line from \p general->inputfile and fills the dip
     * and length descriptors plus the intensity/count controls. Selects the
     * random_method for dip (dip/n_dip/u_dip) and length (length/n_length/
     * f_length/p_length), and sets the generation method: count (\p method=0),
     * p21 (\p method=1) or p10 (\p method=2). Aborts via OPENDFN_EXIT on an
     * unknown keyword.
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
    static void parseDFNs(General        general,
                          Real           dip[],
                          Real           length[],
                          Int&           count,
                          Int&           method,
                          Real&          p21,
                          Real&          p10,
                          random_method& dip_method,
                          random_method& length_method);

    /**
     * \brief Check that a geometry group tag is not already in use, aborting if
     *        a duplicate is found.
     *
     * Scans the existing geometry groups; if any group already carries \p tag,
     * emits an error message and terminates via OPENDFN_EXIT.
     *
     * \param geometry  Geometry container holding the current group list.
     * \param tag       Candidate group tag to validate for uniqueness.
     */
    static void chechGeomtryGroups(Geometry geometry, char* tag);

    /**
     * \brief Ensure a line group with the given tag exists, creating it if
     *        absent.
     *
     * Searches \p geometry->linegroups for \p newToolTag; if not present,
     * allocates a new line group, copies the tag and increments the line-group
     * count.
     *
     * \param geometry    Geometry container whose linegroups list may be grown.
     * \param newToolTag  Tag of the line group to look up or create.
     */
    static void addtoLineGroups(Geometry geometry, char* newToolTag);

    /**
     * \brief Ensure a line-to-node group with the given tag exists, creating it
     *        if absent.
     *
     * Searches \p geometry->group for \p newToolTag; if not present, allocates a
     * new group, copies the tag, sets its type to GroupType::FROMLINETAGS and
     * increments the group count.
     *
     * \param geometry    Geometry container whose group list may be grown.
     * \param newToolTag  Tag of the group to look up or create.
     */
    static void addtoLinetoNodeGroups(Geometry geometry, char* newToolTag);

    /**
     * \brief Register a line as a boolean tool on a named surface.
     *
     * Finds the surface tagged \p ObjectTag and appends a (dimension=1, tag)
     * entry referencing line \p n_line to that surface's boollines list,
     * growing the list and updating its count. Emits an error message if no
     * matching surface is found.
     *
     * \param geometry   Geometry container holding the surfaces.
     * \param ObjectTag  Tag of the target surface.
     * \param n_line     Zero-based line index; stored as n_line + 1.
     */
    static void addLinetoSufaceBoolGroups(Geometry geometry, char* ObjectTag, Int n_line);

    /**
     * \brief Register a surface as a remove tool on a named surface.
     *
     * Finds the surface tagged \p ObjectTag and appends a (dimension=2, tag)
     * entry referencing surface \p n_surface to that surface's removetool list,
     * growing the list and updating its count. Emits an error message if no
     * matching surface is found.
     *
     * \param geometry   Geometry container holding the surfaces.
     * \param ObjectTag  Tag of the target surface.
     * \param n_surface  Zero-based surface index; stored as n_surface + 1.
     */
    static void addLinetoSufaceRemoveGroups(Geometry geometry, char* ObjectTag, Int n_surface);

    /**
     * \brief Test whether a point lies within the axis-aligned bounding box of
     *        a segment.
     *
     * Returns true when \p point falls inside the bounding box spanned by
     * \p start and \p end. This is a bounding-box containment test intended to
     * validate an already-computed line intersection point, not a strict
     * on-line test.
     *
     * \param point  Point to test.
     * \param start  First endpoint of the segment.
     * \param end    Second endpoint of the segment.
     * \return true if \p point is inside the segment's bounding box.
     */
    static bool isPointOnLineSegment(const Vector2& point, const Vector2& start, const Vector2& end);

    /**
     * \brief Find where a ray from a start point at a given angle intersects a
     *        rectangle, returning the two boundary intersection points.
     *
     * Computes the infinite line through \p startPoint with slope tan(\p angle)
     * and intersects it with the four edges (AB, BC, CD, DA) of the rectangle
     * defined by corners \p A, \p B, \p C, \p D. Points lying within an edge's
     * bounding box are written to \p Intersection as consecutive [x, y] pairs.
     *
     * \param startPoint   Origin of the ray.
     * \param angle        Ray angle in radians; slope is tan(angle).
     * \param A            Rectangle corner A.
     * \param B            Rectangle corner B.
     * \param C            Rectangle corner C.
     * \param D            Rectangle corner D.
     * \param Intersection Output buffer receiving up to two [x, y] pairs.
     * \return true if exactly two boundary intersection points were found,
     *         false otherwise.
     */
    static bool findIntersectionPoints(const Vector2& startPoint,
                                       Real           angle,
                                       const Vector2& A,
                                       const Vector2& B,
                                       const Vector2& C,
                                       const Vector2& D,
                                       Real           Intersection[]);

    /**
     * \brief Sample a dip angle for a joint set according to its configured
     *        random method.
     *
     * Depending on \p Jsetdata.dip_method: constant returns dip[0]; uniform
     * samples in [dip[0], dip[1]]; Gaussian samples about mean dip[0] with
     * deviation dip[1], rejecting draws outside [0, 180]. Aborts via
     * OPENDFN_EXIT if no dip method is set.
     *
     * \param Jsetdata  Joint-set parameters supplying the dip descriptor/method.
     * \param r         OpenDFN random-number generator state (advanced on sampling).
     * \param dipAngle  Output sampled dip angle in degrees.
     */
    static void getRandomDipAngle(Jset_data& Jsetdata, ns_common::OpenDFNRandom& r, Real& dipAngle) {
        Real u;
        if (Jsetdata.dip_method == random_method::RANDOM_NULL) {
            OpenDFNMessage::RuntimeInfo("Error: no dips for joints.");
            OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
        } else if (Jsetdata.dip_method == random_method::RANDOM_CONSTANT) {
            dipAngle = Jsetdata.dip[0];
        } else if (Jsetdata.dip_method == random_method::RANDOM_UNIFORM) {
            u        = r.uniform();
            dipAngle = Jsetdata.dip[0] + (Jsetdata.dip[1] - Jsetdata.dip[0]) * u;
        } else if (Jsetdata.dip_method == random_method::RANDOM_GAUSSE) {
            do {
                u = r.gaussian(Jsetdata.dip[1]);
            } while ((Jsetdata.dip[0] + u) > 180 || (Jsetdata.dip[0] + u) < 0);
            dipAngle = Jsetdata.dip[0] + u;
        }
    }

    /**
     * \brief Sample a spacing distance for a joint set according to its
     *        configured random method.
     *
     * Constant returns space[0]; Gaussian samples about mean space[0] with
     * deviation space[1], rejecting draws below 5% of space[0]. Aborts via
     * OPENDFN_EXIT if no space method is set.
     *
     * \param Jsetdata       Joint-set parameters supplying the space
     *                       descriptor/method.
     * \param r              OpenDFN random-number generator state (advanced).
     * \param spaceDistance  Output sampled spacing distance.
     */
    static void getRandomSpace(Jset_data& Jsetdata, ns_common::OpenDFNRandom& r, Real& spaceDistance) {
        Real u;
        if (Jsetdata.space_method == random_method::RANDOM_NULL) {
            OpenDFNMessage::RuntimeInfo("Error: no space for joints.");
            OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
        } else if (Jsetdata.space_method == random_method::RANDOM_CONSTANT) {
            spaceDistance = Jsetdata.space[0];
        } else if (Jsetdata.space_method == random_method::RANDOM_GAUSSE) {
            do {
                u = r.gaussian(Jsetdata.space[1]);
            } while ((Jsetdata.space[0] + u) < 0.05 * Jsetdata.space[0]);

            spaceDistance = Jsetdata.space[0] + u;
        }
    }

    /**
     * \brief Sample a trace length for a non-persistent joint set according to
     *        its configured random method.
     *
     * Constant returns trace[0]; Gaussian samples about mean trace[0] with
     * deviation trace[1], rejecting draws below 5% of trace[0]. Aborts via
     * OPENDFN_EXIT if no trace method is set.
     *
     * \param Jsetdata     Joint-set parameters supplying the trace
     *                     descriptor/method.
     * \param r            OpenDFN random-number generator state (advanced).
     * \param traceLength  Output sampled trace length.
     */
    static void getRandomTrace(Jset_data& Jsetdata, ns_common::OpenDFNRandom& r, Real& traceLength) {
        Real u;
        if (Jsetdata.trace_method == random_method::RANDOM_NULL) {
            OpenDFNMessage::RuntimeInfo("Error: no space for joints.");
            OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
        } else if (Jsetdata.trace_method == random_method::RANDOM_CONSTANT) {
            traceLength = Jsetdata.trace[0];
        } else if (Jsetdata.trace_method == random_method::RANDOM_GAUSSE) {
            do {
                u = r.gaussian(Jsetdata.trace[1]);
            } while ((Jsetdata.trace[0] + u) < 0.05 * Jsetdata.trace[0]);

            traceLength = Jsetdata.trace[0] + u;
        }
    }

    /**
     * \brief Sample a gap distance for a non-persistent joint set according to
     *        its configured random method.
     *
     * Constant returns gap[0]; Gaussian samples about mean gap[0] with
     * deviation gap[1], rejecting draws below 5% of gap[0]. Aborts via
     * OPENDFN_EXIT if no gap method is set.
     *
     * \param Jsetdata     Joint-set parameters supplying the gap
     *                     descriptor/method.
     * \param r            OpenDFN random-number generator state (advanced).
     * \param gapDistance  Output sampled gap distance.
     */
    static void getRandomGap(Jset_data& Jsetdata, ns_common::OpenDFNRandom& r, Real& gapDistance) {
        Real u;
        if (Jsetdata.gap_method == random_method::RANDOM_NULL) {
            OpenDFNMessage::RuntimeInfo("Error: no space for joints.");
            OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
        } else if (Jsetdata.gap_method == random_method::RANDOM_CONSTANT) {
            gapDistance = Jsetdata.gap[0];
        } else if (Jsetdata.gap_method == random_method::RANDOM_GAUSSE) {
            do {
                u = r.gaussian(Jsetdata.gap[1]);
            } while ((Jsetdata.gap[0] + u) < 0.05 * Jsetdata.gap[0]);

            gapDistance = Jsetdata.gap[0] + u;
        }
    }
};
}  // namespace ns_geometry

#endif  // SOLID_SOLID_MECHANICS_GEOMETRY_UTILS_CLASS_
