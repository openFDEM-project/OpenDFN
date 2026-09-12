/******************************************************************************
 * \file      push_geometry_joint.cpp
 * \brief     Basic classes parser mesh fron selfdeveloped format parameters.
 *
 * \details   Implementation of basic classes parser mesh fron selfdeveloped
 *            format parameters.
 *
 * \author    Xiaofeng Li
 * \email     xfli@whrsm.ac.cn
 * \date      Created:       June 24, 2020
 * \date      Last modified: 2026-07-20 00:04:42
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

#include <geometry/geometry_rules.h>
#include "geometry/opendfn_geometry_module.h"
#include "mesh/group/geometry_keyword_reader.h"

namespace ns_geometry {
/**
 * @brief Parse a single deterministic joint definition and add it as an embedded line.
 *
 * Reads the joint tag, the target object (surface) tag, and the two endpoint
 * coordinates {x0, y0, x1, y1} from @p general->inputfile. The candidate segment is
 * validated for intersection quality; if it fails, a warning is emitted and no
 * geometry is added. Otherwise two nodes and one connecting line are appended to
 * @p geometry, the line is flagged as embedded (non-element), and it is registered
 * with the surface boolean-cut groups, node groups, and line groups.
 *
 * Side effects: mutates @p geometry (allocates/appends nodes and a line, updates
 * counters and group registrations); advances the input-file parse cursor; may emit
 * a runtime warning message.
 *
 * @param general  Global context providing the input-file stream to parse.
 * @param geometry Geometry container that is mutated with the joint line.
 * @return true if the joint was added; false if it was rejected due to a bad
 *         intersection angle or line size.
 */
bool GeometryBuilder::push_geometry_joint(General general, Geometry geometry) {
    int n_node = geometry->node_num;

    char newToolTag[MAXARGC], ObjectTag[MAXARGC];
    parseStringinQuotation(general->inputfile, newToolTag);
    parseStringinQuotation(general->inputfile, ObjectTag);
    // container = {x0, y0, x1, y1}
    Real container[4] = {0.0, 0.0, 0.0, 0.0};
    ns_mesh::GeometryKeywordReader::pushPlaneKeywords(general->inputfile, container);

    if (!GeometryRules::isLinesWellIntersection(geometry, container, false)) {
        OpenDFNMessage::RuntimeStringStringString("Warning: Joint ", newToolTag, "is not added due to bad angle or line size.");
        return false;
    }

    UInt index = 0;
    Real value;
    /* add geometry node */
    for (int i = 0; i < 2; i++) {
        // malloc node
        buildmemory(&geometry->node, (geometry->node_num + 1), geometry->node_num);
        geometry->node[geometry->node_num].x = container[index];
        index++;
        geometry->node[geometry->node_num].y = container[index];
        index++;

        geometry->node_num++;
    }

    /* add geometry line */
    // malloc line
    buildmemory(&geometry->line, (geometry->line_num + 1), geometry->line_num);
    // copy the tag
    copyChars(geometry->line[geometry->line_num].tag, newToolTag);

    geometry->line[geometry->line_num].node_1 = n_node;
    geometry->line[geometry->line_num].node_2 = n_node + 1;

    geometry->line[geometry->line_num].iselement = false;
    geometry->line[geometry->line_num].isembeded = true;
    // add to bool groups for cut
    addLinetoSufaceBoolGroups(geometry, ObjectTag, geometry->line_num);

    geometry->line_num++;

    // add to node groups
    addtoLinetoNodeGroups(geometry, newToolTag);

    // build for line groups
    addtoLineGroups(geometry, newToolTag);
    return true;
}

/**
 * @brief Parse a 1D line (wall) definition and add it as a meshable element line.
 *
 * Reads the line name and its two endpoint coordinates {x0, y0, x1, y1} from
 * @p general->inputfile. The name is recorded as a wall tag so it can later be
 * exported as a 1D element group, then two nodes and one connecting line are
 * appended. Unlike an embedded joint, this line is flagged as an element
 * (@c iselement = true) so it participates directly in the mesh.
 *
 * Side effects: mutates @p geometry (allocates/appends a wall tag, two nodes, and a
 * line, and updates the node and line counters); registers the line with the node
 * groups; advances the input-file parse cursor.
 *
 * @param general  Global context providing the input-file stream to parse.
 * @param geometry Geometry container that is mutated with the wall line.
 * @return void
 */
void GeometryBuilder::push_geometry_line(General general, Geometry geometry) {
    int n_node  = geometry->node_num;
    int n_line  = geometry->line_num;
    int n_curve = geometry->curve_num;

    char name[MAXARGC];
    parseStringinQuotation(general->inputfile, name);
    // add to 1d element group, in order to parse to opendfn
    buildmemory(&geometry->wallTags, (geometry->wall_num + 1), geometry->wall_num);
    geometry->wallTags[geometry->wall_num] = (char*)malloc(sizeof(char) * 20);

    copyChars(geometry->wallTags[geometry->wall_num], name);
    geometry->wall_num++;

    Real container[4] = {0.0, 0.0, 0.0, 0.0};
    ns_mesh::GeometryKeywordReader::pushPlaneKeywords(general->inputfile, container);
    UInt index = 0;
    Real value;
    /* add geometry node */
    for (int i = 0; i < 2; i++) {
        // malloc node
        buildmemory(&geometry->node, (n_node + 1), n_node);

        geometry->node[n_node].x = container[index];
        index++;
        geometry->node[n_node].y = container[index];
        index++;

        n_node++;
    }

    /* add geometry line */
    // malloc line
    buildmemory(&geometry->line, (n_line + 1), n_line);
    // copy the tag
    copyChars(geometry->line[n_line].tag, name);

    geometry->line[n_line].node_1 = geometry->node_num;
    geometry->line[n_line].node_2 = geometry->node_num + 1;

    geometry->line[n_line].iselement = true;
    n_line++;

    /* update to geometry */
    geometry->node_num = n_node;
    geometry->line_num = n_line;

    // add to node groups
    addtoLinetoNodeGroups(geometry, name);
}

/**
 * @brief Register a batch of tags as 1D line (wall) element groups.
 *
 * Grows the geometry's wall-tag array and copies each of the supplied group tags
 * into it, so the named line groups are treated as 1D wall elements.
 *
 * Side effects: mutates @p geometry by allocating and appending @p groupcount wall
 * tags and advancing @c wall_num.
 *
 * @param general    Global context (unused for parsing here; kept for signature
 *                   consistency).
 * @param geometry   Geometry container whose wall-tag list is extended.
 * @param groupcount Number of tags in @p grouptags to register.
 * @param grouptags  Array of null-terminated tag strings to copy.
 * @return void
 */
void GeometryBuilder::pushGeometryLineGroups(General general, Geometry geometry, const Int groupcount, char** grouptags) {
    buildmemory(&geometry->wallTags, (geometry->wall_num + groupcount), geometry->wall_num);

    _for(i, 0, groupcount) {
        buildmemory(&geometry->wallTags[geometry->wall_num], MAXARGC);
        copyChars(geometry->wallTags[geometry->wall_num], grouptags[i]);
        geometry->wall_num++;
    }
}

/**
 * @brief Register a batch of tags as DFN line-element groups.
 *
 * Grows the geometry's line-element-tag array and copies each supplied group tag
 * into it, marking those line groups as DFN line elements.
 *
 * Side effects: mutates @p geometry by allocating and appending @p groupcount
 * line-element tags and advancing @c LineElement_n.
 *
 * @param general    Global context (kept for signature consistency).
 * @param geometry   Geometry container whose line-element-tag list is extended.
 * @param groupcount Number of tags in @p grouptags to register.
 * @param grouptags  Array of null-terminated tag strings to copy.
 * @return void
 */
void GeometryBuilder::addGeometryDFNTags(General general, Geometry geometry, const Int groupcount, char** grouptags) {
    buildmemory(&geometry->LineElememtTags, (geometry->LineElement_n + groupcount), geometry->LineElement_n);

    _for(i, 0, groupcount) {
        buildmemory(&geometry->LineElememtTags[geometry->LineElement_n], MAXARGC);
        copyChars(geometry->LineElememtTags[geometry->LineElement_n], grouptags[i]);
        geometry->LineElement_n++;
    }
}

void GeometryBuilder::addGeometryFlipTags(General general, Geometry geometry, const Int groupcount, char** grouptags) {
    buildmemory(&geometry->LineFlipTags, (geometry->LineFlip_n + groupcount), geometry->LineFlip_n);
    _for(i, 0, groupcount) {
        buildmemory(&geometry->LineFlipTags[geometry->LineFlip_n], MAXARGC);
        copyChars(geometry->LineFlipTags[geometry->LineFlip_n], grouptags[i]);
        geometry->LineFlip_n++;
    }
}
}  // namespace ns_geometry