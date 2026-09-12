/******************************************************************************
 * \file      push_geometry_rdfns.cpp
 * \brief     Basic classes parser mesh fron selfdeveloped format parameters.
 *
 * \details   Implementation of basic classes parser mesh fron selfdeveloped
 *            format parameters.
 *
 * \author    Xiaofeng Li
 * \email     xfli@whrsm.ac.cn
 * \date      Created:       June 24, 2020
 * \date      Last modified: 2026-07-20 00:04:41
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

#include "geometry/opendfn_geometry_module.h"

namespace ns_geometry {
/**
 * \brief Build "realistic" Discrete Fracture Networks (DFNs) from an external
 *        polyline image file and inject them into the geometry as embedded lines.
 *
 * The routine parses three quoted tokens from the current input command
 * (\c newToolTag for the new fracture lines, \c ObjectTag for the target surface
 * boolean group, and \c filename for the DFN data file) plus a pixel-to-model
 * scale factor. Each line of the file is tokenised into whitespace-separated
 * coordinate pairs; every pair becomes a geometry node (divided by
 * \c pixel_ratio to convert from pixels to model units), and consecutive nodes
 * on the same file line are connected by embedded line segments. Each new line
 * is flagged non-element/embedded and registered into the target surface's
 * boolean-cut group so it can later slice the host surface.
 *
 * \param general   Runtime context; its \c inputfile stream is advanced while
 *                  reading the tags, pixel ratio, and file name.
 * \param geometry  Geometry container that is mutated: node, line, node-group
 *                  and line-group arrays are grown and \c node_num / \c line_num
 *                  are updated.
 * \return void
 *
 * \note Side effects: reads the DFN file from disk, allocates geometry memory
 *       via buildmemory, mutates the shared \p geometry object, and terminates
 *       the process via OPENDFN_EXIT if the file cannot be opened.
 */
void GeometryBuilder::createRealisticDFNs(General general, Geometry geometry) {
    int n_node    = geometry->node_num;
    int n_line    = geometry->line_num;
    int n_curve   = geometry->curve_num;
    int n_surface = geometry->surface_num;

    Real pixel_ratio;
    getRealValue(general->inputfile, &pixel_ratio);

    char newToolTag[MAXARGC], ObjectTag[MAXARGC], filename[300];
    parseStringinQuotation(general->inputfile, newToolTag);
    parseStringinQuotation(general->inputfile, ObjectTag);
    parse_string(general->inputfile, filename);

    Int   ret;
    char  sn[2048];
    FILE* fp = FileNull;
    /* open file */
    fp = fopen(filename, "r");
    if (fp == FileNull) {
        OpenDFNMessage::RuntimeInfo("Error: fails to open image file.");
        OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
    }
    char  delim[] = " ";
    char* ptr;

    while (ret = fscanf(fp, "%[^\n]%*c", sn) > 0) {
        std::string tempString(sn);
        // clear the \t for UNIX
        tempString.erase(std::remove(tempString.begin(), tempString.end(), '\n'), tempString.end());
        tempString.erase(std::remove(tempString.begin(), tempString.end(), '\r'), tempString.end());

        // copy the keywords
        copyChars(sn, tempString.c_str());

        int keyword_count = 0;
        ptr               = strtok(sn, delim);
        char** keyword;
        /* parse the string to keywords */
        while (ptr != NULL) {
            if (keyword_count == 0)
                keyword = (char**)malloc(sizeof(char*));
            else
                keyword = (char**)realloc(keyword, sizeof(char*) * (keyword_count + 1));

            keyword[keyword_count] = (char*)malloc(sizeof(char) * 20);
            copyChars(keyword[keyword_count], ptr);
            ptr = strtok(NULL, delim);
            keyword_count++;
        }
        /* transfer the string to double and put into geometry->node */
        int start_node = n_node;
        for (int i = 0; i < keyword_count; i++) {
            // malloc node
            buildmemory(&geometry->node, (n_node + 1), n_node);
            geometry->node[n_node].x = strtod(keyword[i], NULL);
            geometry->node[n_node].y = strtod(keyword[i + 1], NULL);
            geometry->node[n_node].x = geometry->node[n_node].x / pixel_ratio;
            geometry->node[n_node].y = geometry->node[n_node].y / pixel_ratio;

            n_node++;
            i++;
        }
        /* add to  geometry->line */
        int node_number = n_node - start_node;
        for (int i = 0; i < (node_number - 1); i++) {
            // malloc line
            buildmemory(&geometry->line, (n_line + 1), n_line);
            copyChars(geometry->line[n_line].tag, newToolTag);

            geometry->line[n_line].node_1 = start_node + i;
            geometry->line[n_line].node_2 = start_node + i + 1;

            geometry->line[n_line].iselement = false;
            geometry->line[n_line].isembeded = true;
            // add to bool groups for cut
            addLinetoSufaceBoolGroups(geometry, ObjectTag, n_line);

            n_line++;
        }

        /* free up keywords*/
        for (int i = 0; i < keyword_count; i++) {
            free(keyword[i]);
        }
        free(keyword);
    }
    fclose(fp);

    /* update to geometry */
    geometry->node_num = n_node;
    geometry->line_num = n_line;
    // add to node groups
    addtoLinetoNodeGroups(geometry, newToolTag);
    // build for line groups
    addtoLineGroups(geometry, newToolTag);
}
}  // namespace ns_geometry