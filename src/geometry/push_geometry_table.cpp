/******************************************************************************
 * \file      push_geometry_table.cpp
 * \brief     Basic classes parser mesh fron selfdeveloped format parameters.
 *
 * \details   Implementation of basic classes parser mesh fron selfdeveloped
 *            format parameters.
 *
 * \author    Xiaofeng Li
 * \email     xfli@whrsm.ac.cn
 * \date      Created:       June 24, 2020
 * \date      Last modified: 2026-07-20 00:06:08
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

#include "geometry/opendfn_geometry_module.h"
namespace ns_geometry {
/**
 * \brief Build a geometry primitive from a table file of node coordinates and
 *        add it as a surface or a cutting tool.
 *
 * Parses the new tool tag (and, for CUT/REMOVE, the target object tag) and the
 * table file name from the input stream. The file starts with an integer node
 * count N followed by N (x, y) coordinate pairs, each of which becomes a
 * geometry node. For CUT the routine creates N-1 open embedded/reserved lines
 * registered into the target boolean-cut group; otherwise it creates N closed
 * lines, wraps them in a curve and a surface, and for REMOVE registers the
 * surface into the removal group so it excavates the matching object.
 *
 * \param general    Runtime context; its \c inputfile stream is advanced while
 *                   reading the tags and file name.
 * \param geometry   Geometry container that is mutated: node, line, curve,
 *                   surface and related group arrays are grown and the
 *                   \c *_num counters are updated.
 * \param embedType  Operation mode: ENTIRE (plain surface), REMOVE (excavation
 *                   surface), or CUT (open embedded cutting lines only).
 * \return void
 *
 * \note Side effects: reads the table file from disk, allocates geometry memory
 *       via buildmemory, mutates the shared \p geometry object, and terminates
 *       the process via OPENDFN_EXIT if the file cannot be opened.
 */
void GeometryBuilder::push_geometry_table(General general, Geometry geometry, OperationType embedType) {
    int n_node    = geometry->node_num;
    int n_line    = geometry->line_num;
    int n_curve   = geometry->curve_num;
    int n_surface = geometry->surface_num;

    char newToolTag[MAXARGC], ObjectTag[MAXARGC], filename[300];
    parseStringinQuotation(general->inputfile, newToolTag);
    if ((embedType == OperationType::CUT) || (embedType == OperationType::REMOVE)) {
        parseStringinQuotation(general->inputfile, ObjectTag);
    }

    parseStringinQuotation(general->inputfile, filename);
    Real  value;
    Int   N;
    FILE* fp = FileNull;
    /* open file */
    fp = fopen(filename, "r");
    if (fp == FileNull) {
        OpenDFNMessage::RuntimeInfo("Error: fails to open table file.");
        OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
    }

    getIntValue(fp, &N);

    /* add geometry node */
    for (int i = 0; i < N; i++) {
        // malloc node
        buildmemory(&geometry->node, (n_node + 1), n_node);

        getRealValue(fp, &value);
        geometry->node[n_node].x = value;
        getRealValue(fp, &value);
        geometry->node[n_node].y = value;
        getGeoNodeGroupbyCoord(geometry, newToolTag, geometry->node[n_node].x, geometry->node[n_node].y);
        n_node++;
    }
    fclose(fp);

    /* add geometry line */
    // for cut lines
    if (embedType == OperationType::CUT) {
        /* add geometry line */
        for (int i = 0; i < N - 1; i++) {
            buildmemory(&geometry->line, (n_line + 1), n_line);

            copyChars(geometry->line[n_line].tag, newToolTag);

            geometry->line[n_line].node_1 = geometry->node_num + i;

            geometry->line[n_line].iselement    = false;
            geometry->line[n_line].isembeded    = true;
            geometry->line[n_line].tobeReserved = true;

            geometry->line[n_line].node_2 = geometry->node_num + i + 1;
            // add to bool groups for cut
            addLinetoSufaceBoolGroups(geometry, ObjectTag, n_line);
            n_line++;
        }
        // build for line groups

        addtoLineGroups(geometry, newToolTag);
    }
    // for entire blocks or excavation
    // shhould be closed by default
    else {
        for (int i = 0; i < N; i++) {
            buildmemory(&geometry->line, (n_line + 1), n_line);
            // copy the tag
            copyChars(geometry->line[n_line].tag, newToolTag);

            geometry->line[n_line].node_1 = geometry->node_num + i;

            geometry->line[n_line].iselement = false;

            if (i == (N - 1))
                geometry->line[n_line].node_2 = geometry->node_num;
            else
                geometry->line[n_line].node_2 = geometry->node_num + i + 1;
            n_line++;
        }

        // add curves
        buildmemory(&geometry->curve, (n_curve + 1), n_curve);
        // copy the tag
        copyChars(geometry->curve[n_curve].tag, newToolTag);
        // assign the line count
        geometry->curve[n_curve].n_line = N;
        // assign lines
        buildmemory(&geometry->curve[n_curve].line, N);
        for (int i = 0; i < N; i++) {
            geometry->curve[n_curve].line[i] = geometry->line_num + i;
        }
        n_curve++;
        // update curve
        geometry->curve_num = n_curve;

        // create the new surface
        buildmemory(&geometry->surface, (n_surface + 1), n_surface);
        copyChars(geometry->surface[n_surface].tag, newToolTag);
        geometry->surface[n_surface].n_curve = 1;
        buildmemory(&geometry->surface[n_surface].curve, 1);
        // assing the curve id
        geometry->surface[n_surface].curve[0] = geometry->curve_num - 1;
        switch (embedType) {
            case OperationType::ENTIRE: break;
            case OperationType::REMOVE: {
                // to find the object
                addLinetoSufaceRemoveGroups(geometry, ObjectTag, geometry->surface_num);
            } break;
            case OperationType::CUT: break;
            default: break;
        }
        // update surface
        n_surface++;
        geometry->surface_num = n_surface;
    }

    /* update to geometry */
    geometry->node_num = n_node;
    geometry->line_num = n_line;

    // add to node groups
    addtoLinetoNodeGroups(geometry, newToolTag);
}

/**
 * \brief Load a joint polyline from a table file, apply start/scale/rotate
 *        transforms, and insert it as embedded cutting lines.
 *
 * Parses the new tool tag and target object tag, then a keyword line that may
 * specify the source \c table file, a \c start offset, a \c scale factor, and a
 * \c rotate angle (degrees). The table file supplies an integer node count N
 * followed by N (x, y) pairs. Nodes are recentred on the first point, scaled,
 * offset by \c start, and rotated by the (negated, deg-to-rad) angle about the
 * first node. The resulting N-1 segments are added as embedded/reserved lines
 * registered into the target surface boolean-cut group.
 *
 * \param general   Runtime context; its \c inputfile stream is advanced while
 *                  reading the tags and keyword options.
 * \param geometry  Geometry container that is mutated: node, line and related
 *                  group arrays are grown and \c node_num / \c line_num updated.
 * \return void
 *
 * \note Side effects: reads the table file from disk, allocates geometry memory
 *       via buildmemory, mutates the shared \p geometry object, and terminates
 *       the process via OPENDFN_EXIT on an unrecognised keyword or a file that
 *       cannot be opened.
 */
void GeometryBuilder::pushJointFromSRC(General general, Geometry geometry) {
    int n_node = geometry->node_num;
    int n_line = geometry->line_num;

    char newToolTag[MAXARGC], ObjectTag[MAXARGC], filename[OpenDFN_ARGC_LENGTH];
    parseStringinQuotation(general->inputfile, newToolTag);
    parseStringinQuotation(general->inputfile, ObjectTag);

    Keywords keyword;
    parseStringinLine(general->inputfile, keyword);
    Vector2 start = {0, 0}, scale = {1, 1};
    Real    rotation = 0;
    _for(j, 0, keyword.count) {
        if (isSameString(keyword.keywords[j], "table")) {
            copyChars(filename, keyword.keywords[j + 1]);
            j++;
        } else if (isSameString(keyword.keywords[j], "start") && keyword.findbracket) {
            start.x = strtod(keyword.keywords[j + 1], NULL);
            j++;
            start.y = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSameString(keyword.keywords[j], "start.x")) {
            start.x = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSameString(keyword.keywords[j], "start.y")) {
            start.y = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSameString(keyword.keywords[j], "scale") && keyword.findbracket) {
            scale.x = strtod(keyword.keywords[j + 1], NULL);
            j++;
            scale.y = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSameString(keyword.keywords[j], "scale.x")) {
            scale.x = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSameString(keyword.keywords[j], "scale.y")) {
            scale.y = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSameString(keyword.keywords[j], "rotate")) {
            rotation = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else {
            OpenDFNMessage::RuntimebackStringString("Error: unrecognised keyword ", keyword.keywords[j]);
            OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
        }
    }
    keyword.clean();

    Real  value;
    Int   N;
    FILE* fp = FileNull;
    /* open file */
    fp = fopen(filename, "r");
    if (fp == FileNull) {
        OpenDFNMessage::RuntimeInfo("Error: fails to open table file.");
        OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
    }

    getIntValue(fp, &N);

    /* add geometry node */
    for (int i = 0; i < N; i++) {
        // malloc node
        buildmemory(&geometry->node, (n_node + 1), n_node);

        getRealValue(fp, &value);
        geometry->node[n_node].x = value;
        getRealValue(fp, &value);
        geometry->node[n_node].y = value;
        n_node++;
    }
    fclose(fp);
    // get the origion
    Vector2 point0 = {geometry->node[geometry->node_num].x, geometry->node[geometry->node_num].y};

    // offset the data
    _for(i, 0, N) {
        geometry->node[n_node - i - 1].x = scale.x * (geometry->node[n_node - i - 1].x - point0.x) + start.x;
        geometry->node[n_node - i - 1].y = scale.y * (geometry->node[n_node - i - 1].y - point0.y) + start.y;
    }
    // rotate the joint
    Vector2 ret;
    rotation = -rotation * OpenDFN_Deg2Rad;
    _for(i, 0, N - 1) {
        ret = MeshUtils::rotateSegment({geometry->node[n_node - N].x, geometry->node[n_node - N].y},
                                       {geometry->node[n_node - i - 1].x, geometry->node[n_node - i - 1].y}, rotation);

        geometry->node[n_node - i - 1].x = ret.x;
        geometry->node[n_node - i - 1].y = ret.y;
    }
    /* add geometry line */
    // for cut lines
    /* add geometry line */
    for (int i = 0; i < N - 1; i++) {
        buildmemory(&geometry->line, (n_line + 1), n_line);

        copyChars(geometry->line[n_line].tag, newToolTag);

        geometry->line[n_line].node_1 = geometry->node_num + i;

        geometry->line[n_line].iselement    = false;
        geometry->line[n_line].isembeded    = true;
        geometry->line[n_line].tobeReserved = true;

        geometry->line[n_line].node_2 = geometry->node_num + i + 1;
        // add to bool groups for cut
        addLinetoSufaceBoolGroups(geometry, ObjectTag, n_line);
        n_line++;
    }
    // build for line groups

    addtoLineGroups(geometry, newToolTag);

    /* update to geometry */
    geometry->node_num = n_node;
    geometry->line_num = n_line;

    // add to node groups
    addtoLinetoNodeGroups(geometry, newToolTag);
}

/**
 * \brief Load a polyline from a table file, apply start/scale/rotate transforms,
 *        and insert it as element (wall/truss) lines registered as a wall.
 *
 * Parses the new tool tag and registers it as a new wall tag, then parses a
 * keyword line supporting the same \c table / \c start / \c scale / \c rotate
 * options as pushJointFromSRC. The table file supplies an integer node count N
 * followed by N (x, y) pairs. Nodes are recentred on the first point, scaled,
 * offset by \c start, and rotated by the (negated, deg-to-rad) angle about the
 * first node. Unlike the joint variant, the resulting N-1 segments are marked
 * as element lines (\c iselement = true, \c isembeded = false) and are not added
 * to a boolean-cut group.
 *
 * \param general   Runtime context; its \c inputfile stream is advanced while
 *                  reading the tag and keyword options.
 * \param geometry  Geometry container that is mutated: wallTags, node, line and
 *                  related group arrays are grown, \c wall_num is incremented,
 *                  and \c node_num / \c line_num are updated.
 * \return void
 *
 * \note Side effects: reads the table file from disk, allocates geometry memory
 *       via buildmemory/malloc, mutates the shared \p geometry object, and
 *       terminates the process via OPENDFN_EXIT on an unrecognised keyword or a
 *       file that cannot be opened.
 */
void GeometryBuilder::pushLineFromTable(General general, Geometry geometry) {
    int n_node = geometry->node_num;
    int n_line = geometry->line_num;

    char newToolTag[MAXARGC], filename[OpenDFN_ARGC_LENGTH];
    parseStringinQuotation(general->inputfile, newToolTag);

    buildmemory(&geometry->wallTags, (geometry->wall_num + 1), geometry->wall_num);
    geometry->wallTags[geometry->wall_num] = (char*)malloc(sizeof(char) * MAXARGC);

    copyChars(geometry->wallTags[geometry->wall_num], newToolTag);
    geometry->wall_num++;

    Keywords keyword;
    parseStringinLine(general->inputfile, keyword);
    Vector2 start = {0, 0}, scale = {1, 1};
    Real    rotation = 0;
    _for(j, 0, keyword.count) {
        if (isSameString(keyword.keywords[j], "table")) {
            copyChars(filename, keyword.keywords[j + 1]);
            j++;
        } else if (isSameString(keyword.keywords[j], "start") && keyword.findbracket) {
            start.x = strtod(keyword.keywords[j + 1], NULL);
            j++;
            start.y = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSameString(keyword.keywords[j], "start.x")) {
            start.x = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSameString(keyword.keywords[j], "start.y")) {
            start.y = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSameString(keyword.keywords[j], "scale") && keyword.findbracket) {
            scale.x = strtod(keyword.keywords[j + 1], NULL);
            j++;
            scale.y = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSameString(keyword.keywords[j], "scale.x")) {
            scale.x = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSameString(keyword.keywords[j], "scale.y")) {
            scale.y = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else if (isSameString(keyword.keywords[j], "rotate")) {
            rotation = strtod(keyword.keywords[j + 1], NULL);
            j++;
        } else {
            OpenDFNMessage::RuntimebackStringString("Error: unrecognised keyword ", keyword.keywords[j]);
            OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
        }
    }
    keyword.clean();

    Real  value;
    Int   N;
    FILE* fp = FileNull;
    /* open file */
    fp = fopen(filename, "r");
    if (fp == FileNull) {
        OpenDFNMessage::RuntimeInfo("Error: fails to open table file.");
        OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
    }

    getIntValue(fp, &N);

    /* add geometry node */
    for (int i = 0; i < N; i++) {
        // malloc node
        buildmemory(&geometry->node, (n_node + 1), n_node);

        getRealValue(fp, &value);
        geometry->node[n_node].x = value;
        getRealValue(fp, &value);
        geometry->node[n_node].y = value;
        n_node++;
    }
    fclose(fp);
    // get the origion
    Vector2 point0 = {geometry->node[geometry->node_num].x, geometry->node[geometry->node_num].y};

    // offset the data
    _for(i, 0, N) {
        geometry->node[n_node - i - 1].x = scale.x * (geometry->node[n_node - i - 1].x - point0.x) + start.x;
        geometry->node[n_node - i - 1].y = scale.y * (geometry->node[n_node - i - 1].y - point0.y) + start.y;
    }
    // rotate the joint
    Vector2 ret;
    rotation = -rotation * OpenDFN_Deg2Rad;
    _for(i, 0, N - 1) {
        ret = MeshUtils::rotateSegment({geometry->node[n_node - N].x, geometry->node[n_node - N].y},
                                       {geometry->node[n_node - i - 1].x, geometry->node[n_node - i - 1].y}, rotation);

        geometry->node[n_node - i - 1].x = ret.x;
        geometry->node[n_node - i - 1].y = ret.y;
    }
    /* add geometry line */
    // for cut lines
    /* add geometry line */
    for (int i = 0; i < N - 1; i++) {
        buildmemory(&geometry->line, (n_line + 1), n_line);

        copyChars(geometry->line[n_line].tag, newToolTag);

        geometry->line[n_line].node_1 = geometry->node_num + i;

        geometry->line[n_line].iselement    = true;
        geometry->line[n_line].isembeded    = false;
        geometry->line[n_line].tobeReserved = true;

        geometry->line[n_line].node_2 = geometry->node_num + i + 1;

        n_line++;
    }
    // build for line groups

    addtoLineGroups(geometry, newToolTag);

    /* update to geometry */
    geometry->node_num = n_node;
    geometry->line_num = n_line;

    // add to node groups
    addtoLinetoNodeGroups(geometry, newToolTag);
}
}  // namespace ns_geometry
