/******************************************************************************
 * \file      push_geometry_jset.cpp
 * \brief     Basic classes parser mesh fron selfdeveloped format parameters.
 *
 * \details   Implementation of basic classes parser mesh fron selfdeveloped
 *            format parameters.
 *
 * \author    Xiaofeng Li
 * \email     xfli@whrsm.ac.cn
 * \date      Created:       June 24, 2020
 * \date      Last modified: 2026-07-20 00:04:35
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

#include <common/opendfn_math.h>
#include "geometry/opendfn_geometry_module.h"

#define _DFN_ITERATION_ 30

namespace ns_geometry {
/**
 * @brief Parse a joint-set (Jset) command and stochastically generate joint
 *        lines (DFN) across the geometry domain.
 *
 * Reads the new joint tag and target object tag from the input, then parses the
 * joint-set statistical parameters via parseJsets (dip, spacing, trace length,
 * gap, persistence, optional user-defined start point). A C++ standard-library
 * random-number generator is initialised to sample those distributions.
 *
 * Starting from a seed point (user-defined or offset from a domain corner based
 * on dip), the routine marches across the rectangular domain in steps of the
 * sampled spacing. For each step it samples a dip angle, computes the line's
 * intersection with the domain box, and validates the intersection. For
 * persistent joints a single line spanning the intersection is created; for
 * non-persistent joints the intersection segment is subdivided into multiple
 * trace segments separated by sampled gaps. Each generated line is appended with
 * its nodes, tagged, flagged (iselement=false, tobeReserved=false,
 * isembeded=true), and registered to the target surface's boolean-cut groups.
 *
 * Side effects:
 *  - Reads and advances the input file stream (general->inputfile).
 *  - Consumes randomness from a local standard-library RNG.
 *  - Mutates geometry state: grows geometry->node and geometry->line, updates
 *    node_num/line_num, adds lines to surface boolean groups, and finally
 *    registers node groups (addtoLinetoNodeGroups) and line groups
 *    (addtoLineGroups) for the new joint tag.
 *  - Emits runtime warnings for rejected joints and may call OPENDFN_EXIT for an
 *    undefined domain or an out-of-bounds user start point.
 *
 * @param general  Global context providing the input file stream and iteration
 *                 limits used during parsing.
 * @param geometry Geometry container whose domain bounds are read and whose
 *                 nodes, lines, and groups are mutated with the generated joints.
 * @return void
 */
void GeometryBuilder::push_geometry_jset(General general, Geometry geometry) {
    Jset_data Jsetdata;

    Real    dipAngle = 0, spaceDistance = 0.0, traceLength = 0.0, oldtraceLength = 0.0, gapDistance = 0.0;
    Vector2 Point0, Point1, Point2, Point3, movingPoint, submovingPoint;
    double  u;
    Int     tryTimes = 0;

    /* get the name of the jset */
    char newToolTag[MAXARGC], ObjectTag[MAXARGC];
    parseStringinQuotation(general->inputfile, newToolTag);
    parseStringinQuotation(general->inputfile, ObjectTag);

    ns_common::OpenDFNRandom r;

    // get the parameters of jset command
    parseJsets(general, Jsetdata);

    Real Region[4] = {geometry->x0, geometry->x1, geometry->y0, geometry->y1};

    if ((geometry->x0 == geometry->x1) || (geometry->y0 == geometry->y1)) {
        OpenDFNMessage::RuntimeInfo("Error: undefined geometry domain.");
        OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
    } else {
        // ABCD of the domain should be clockwise
        /*
        D___________C
        |           |
        |           |
        |           |
        A___________B
        */
        Point0.create(geometry->x0, geometry->y0);
        Point1.create(geometry->x1, geometry->y0);
        Point2.create(geometry->x1, geometry->y1);
        Point3.create(geometry->x0, geometry->y1);

        // ��ʼ��
        if (Jsetdata.useUserdefinedStartPoint) {
            if (!isinStrictBox(Jsetdata.startPoint, Region)) {
                OpenDFNMessage::RuntimeInfo("Error: the start point should not on or outside of the domain.");
                OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
            }
            movingPoint = Jsetdata.startPoint;
        } else {
            // ���Ͻ�
            if (Jsetdata.dip[0] < 90) {
                movingPoint.x = Point3.x + 0.05 * Jsetdata.space[0];
                movingPoint.y = Point3.y - 0.05 * Jsetdata.space[0];
            }
            // ���½�, to avoid the start point on the edge of domain
            else {
                movingPoint.x = Point0.x + 0.05 * Jsetdata.space[0];
                movingPoint.y = Point0.y + 0.05 * Jsetdata.space[0];
            }
        }

        do {
            // for nonpersistant joints
            if (Jsetdata.isNonpersistent) {
                getRandomDipAngle(Jsetdata, r, dipAngle);

                /* get the space */
                getRandomSpace(Jsetdata, r, spaceDistance);

                Real container[4];
                if (!findIntersectionPoints(movingPoint, dipAngle / 180 * OpenDFN_PI, Point0, Point1, Point2, Point3, container))
                    continue;

                if (!GeometryRules::isLinesWellIntersection(geometry, container, true) && tryTimes < geometry->iteration) {
                    OpenDFNMessage::RuntimeStringStringString("Warning: Joint ", newToolTag,
                                                               "is not added due to bad angle or line size.");
                    tryTimes++;
                    continue;
                }
                tryTimes = 0;

                // step 2, ����ཻ�㿪ʼ�� ��ÿ����ΪԲ�ģ�traceΪ�뾶�������߶Σ�ƫ����trace ��gap��ͬ����
                // ��������Ϊ���ĵ㳬��box range

                // get the start point from left hand
                if (container[0] < container[2]) {
                    submovingPoint.create(container[0], container[1]);
                } else {
                    submovingPoint.create(container[2], container[3]);
                }
                Real length = sqrt((container[0] - container[2]) * (container[0] - container[2]) +
                                   (container[1] - container[3]) * (container[1] - container[3]));

                // get the initial trace
                getRandomTrace(Jsetdata, r, traceLength);

                do {
                    // too small segments
                    if (traceLength < 2.0 * geometry->minmuumSize)
                        break;
                    // too small lines
                    if (traceLength > length)
                        break;

                    oldtraceLength = traceLength;
                    //############################to check what?

                    /* add geometry geometry->node */
                    for (int i = 0; i < 2; i++) {
                        // malloc node
                        buildmemory(&geometry->node, (geometry->node_num + 1), geometry->node_num);
                        geometry->node[geometry->node_num].x =
                            submovingPoint.x - pow(-1, i) * 0.5 * traceLength * cos(dipAngle / 180.0 * OpenDFN_PI);
                        geometry->node[geometry->node_num].y =
                            submovingPoint.y - pow(-1, i) * 0.5 * traceLength * sin(dipAngle / 180.0 * OpenDFN_PI);
                        geometry->node_num++;
                    }

                    /* add geometry geometry->line */
                    // malloc line
                    buildmemory(&geometry->line, (geometry->line_num + 1), geometry->line_num);
                    // copy the tags
                    copyChars(geometry->line[geometry->line_num].tag, newToolTag);
                    // fill the node into containers
                    geometry->line[geometry->line_num].node_1 = geometry->node_num - 2;
                    geometry->line[geometry->line_num].node_2 = geometry->node_num - 1;
                    // update the flags
                    geometry->line[geometry->line_num].iselement    = false;
                    geometry->line[geometry->line_num].tobeReserved = false;
                    geometry->line[geometry->line_num].isembeded    = true;

                    // add to bool groups for cut
                    addLinetoSufaceBoolGroups(geometry, ObjectTag, geometry->line_num);
                    // update the lines
                    geometry->line_num++;

                    // get the trace
                    getRandomTrace(Jsetdata, r, traceLength);
                    // too small segments
                    if (traceLength < 2.0 * geometry->minmuumSize)
                        continue;
                    // get the gap
                    getRandomGap(Jsetdata, r, gapDistance);

                    // ���ŶԽ����ƶ�
                    if (dipAngle < 90) {
                        submovingPoint.x +=
                            0.5 * (oldtraceLength + traceLength + 2.0 * gapDistance) * cos(dipAngle / 180 * OpenDFN_PI);
                        submovingPoint.y +=
                            0.5 * (oldtraceLength + traceLength + 2.0 * gapDistance) * sin(dipAngle / 180 * OpenDFN_PI);
                    } else {
                        submovingPoint.x -=
                            0.5 * (oldtraceLength + traceLength + 2.0 * gapDistance) * cos(dipAngle / 180 * OpenDFN_PI);
                        submovingPoint.y -=
                            0.5 * (oldtraceLength + traceLength + 2.0 * gapDistance) * sin(dipAngle / 180 * OpenDFN_PI);
                    }
                } while (isinStrictBox(submovingPoint, Region));

                // ���ŶԽ����ƶ�
                movingPoint.x += spaceDistance * sin(dipAngle / 180 * OpenDFN_PI);
                movingPoint.y += -spaceDistance * cos(dipAngle / 180 * OpenDFN_PI);
            } else { /* get the dip */
                getRandomDipAngle(Jsetdata, r, dipAngle);

                /* get the space */
                getRandomSpace(Jsetdata, r, spaceDistance);

                Real container[4];
                if (!findIntersectionPoints(movingPoint, dipAngle / 180 * OpenDFN_PI, Point0, Point1, Point2, Point3, container))
                    continue;

                if (!GeometryRules::isLinesWellIntersection(geometry, container, true) && tryTimes < geometry->iteration) {
                    OpenDFNMessage::RuntimeStringStringString("Warning: Joint ", newToolTag,
                                                               "is not added due to bad angle or line size.");
                    tryTimes++;
                    continue;
                }
                tryTimes = 0;
                /* add geometry geometry->node */
                for (int i = 0; i < 2; i++) {
                    // malloc node
                    buildmemory(&geometry->node, (geometry->node_num + 1), geometry->node_num);
                    geometry->node[geometry->node_num].x = container[2 * i];
                    geometry->node[geometry->node_num].y = container[2 * i + 1];
                    geometry->node_num++;
                }

                /* add geometry geometry->line */
                // malloc line
                buildmemory(&geometry->line, (geometry->line_num + 1), geometry->line_num);
                // copy the tags
                copyChars(geometry->line[geometry->line_num].tag, newToolTag);
                // fill the node into containers
                geometry->line[geometry->line_num].node_1 = geometry->node_num - 2;
                geometry->line[geometry->line_num].node_2 = geometry->node_num - 1;
                // update the flags
                geometry->line[geometry->line_num].iselement    = false;
                geometry->line[geometry->line_num].tobeReserved = false;
                geometry->line[geometry->line_num].isembeded    = true;

                // add to bool groups for cut
                addLinetoSufaceBoolGroups(geometry, ObjectTag, geometry->line_num);
                // update the lines
                geometry->line_num++;

                // ���ŶԽ����ƶ�
                movingPoint.x += spaceDistance * sin(dipAngle / 180 * OpenDFN_PI);
                movingPoint.y += -spaceDistance * cos(dipAngle / 180 * OpenDFN_PI);
            }

            // to jump out
        } while (isinStrictBox(movingPoint, Region));

    }
    // add to node groups
    addtoLinetoNodeGroups(geometry, newToolTag);
    // build for line groups
    addtoLineGroups(geometry, newToolTag);
}
}  // namespace ns_geometry
