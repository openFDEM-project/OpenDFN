/******************************************************************************
 * \file      generateMesh.cpp
 * \brief     Basic classes parser mesh fron selfdeveloped format parameters.
 *
 * \details   Implementation of basic classes parser mesh fron selfdeveloped
 *            format parameters.
 *
 * \author    Xiaofeng Li
 * \email     xfli@whrsm.ac.cn
 * \date      Created:       June 24, 2020
 * \date      Last modified: 2026-07-20 00:51:30
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

#include <cstdlib>
#include <filesystem>
#include <string>
#include "common/opendfn_common.h"
#include "geometry/gmsh.h"
#include "geometry/opendfn_geometry_module.h"

using namespace ns_common;

namespace ns_geometry {
namespace {

bool isGmshGuiEnabled() {
    const char* value = std::getenv("OPENDFN_GMSH_GUI");
    return value != nullptr && (value[0] == '1' || value[0] == 'y' || value[0] == 'Y');
}

bool writeModelFormats(const std::filesystem::path& meshPath, int& ierr) {
    std::filesystem::path vtkPath = meshPath;
    vtkPath.replace_extension(".vtk");

    std::filesystem::path geoPath = meshPath;
    geoPath.replace_extension(".geo");
    std::filesystem::path unrolledGeoPath = meshPath;
    unrolledGeoPath.replace_extension(".geo_unrolled");

    gmshWrite(meshPath.string().c_str(), &ierr);
    if (ierr != 0)
        return false;

    gmshWrite(vtkPath.string().c_str(), &ierr);
    if (ierr != 0)
        return false;

    // Gmsh exports editable GEO syntax through the .geo_unrolled writer.
    // Rename the completed file so users receive the requested .geo extension.
    gmshWrite(unrolledGeoPath.string().c_str(), &ierr);
    if (ierr != 0)
        return false;

    std::error_code fileError;
    std::filesystem::remove(geoPath, fileError);
    fileError.clear();
    std::filesystem::rename(unrolledGeoPath, geoPath, fileError);
    if (fileError) {
        OpenDFNMessage::RuntimebackStringString("Error: cannot finalize GEO file: ", fileError.message().c_str());
        return false;
    }
    return true;
}

}  // namespace
/**
 * \brief Build the full 2D geometry inside the Gmsh/OpenCASCADE kernel, mesh it,
 *        and write the resulting geometry and mesh formats.
 *
 * This is the main driver that translates the OpenDFN geometry description into
 * Gmsh entities and produces a finite-element mesh. The high level steps are:
 *   1. Initialize the Gmsh environment.
 *   2. Add points, lines, curve loops and plane surfaces (tags start from 1).
 *   4. Apply surface-surface boolean operations (cut / fragment).
 *   5. Apply curve-surface intersect + fragment operations to embed DFNs,
 *      discarding fragments shorter than \c geometry->minmuumSize.
 *   6. Select the meshing algorithm from \p keyword, assign physical groups for
 *      points, surfaces and lines, reassign global and per-group mesh sizes,
 *      optionally recombine to quads, and generate the mesh.
 *   7. Write matching .msh, .geo and .vtk files.
 *
 * \param context   OpenDFN runtime context.
 * \param geometry   Geometry description; its \c DimTags / \c DimTags1 / \c DimTags2
 *                   fields are mutated in place to track Gmsh entity tags, and the
 *                   whole structure is freed via \c geometry->clear() at the end.
 * \param argv       Program argument vector forwarded to \c gmshInitialize.
 * \param keyword    Meshing algorithm keyword ("meshadapt", "delaunay",
 *                   "frontal-delaunay"); also used as output file name when
 *                   \p write_flag is 1.
 * \param write_flag When 1, uses \p keyword as the mesh output path; when 0,
 *                   uses "<InputFileName>.msh". Matching .geo and .vtk files
 *                   are always written beside the mesh.
 *
 * \note Side effects: initializes and finalizes the global Gmsh state, launches the
 *       FLTK GUI (\c gmshFltkRun), performs disk I/O, mutates \p geometry and \p opendfn,
 *       and emits runtime messages. Exits the program if no mesh size is assigned.
 */
void GeometryBuilder::generateMesh(OpenDFNHandle context, Geometry geometry, char** argv, char* keyword, int writeFlag) {
    General general = &context->general;

    if (geometry->min_size == 0.0) {
        OpenDFNMessage::RuntimeInfo("Error: no mesh size assigned.");
        OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
    }

    int ierr, i, j;
    int physical_id;
    /* initialize gmash enviroment */
    gmshInitialize(1, argv, 1, 0, &ierr);

    /* add gmsh points start form 1 */
    for (i = 0; i < geometry->node_num; i++) {
        gmshModelOccAddPoint(geometry->node[i].x, geometry->node[i].y, geometry->node[i].z, geometry->mesh_size, i + 1, &ierr);
    }

    /* add gmsh lines */
    for (i = 0; i < geometry->line_num; i++) {
        // set dimtag
        geometry->line[i].DimTags_n = 0;
        int ret;
        ret = gmshModelOccAddLine(geometry->line[i].node_1 + 1, geometry->line[i].node_2 + 1, i + 1, &ierr);
        // set dimtag
        geometry->line[i].DimTags = (int*)malloc(sizeof(int) * 2);
        // dimesion
        geometry->line[i].DimTags[0] = 1;
        // id
        geometry->line[i].DimTags[1] = ret;
        // size
        geometry->line[i].DimTags_n = 2;
    }

    /* add gmsh curve */
    // curve should be closed, anticlockwise?
    // the embeded lines are not added in curve
    for (i = 0; i < geometry->curve_num; i++) {
        // curve list should be int not Int, indetical to gmsh arguments
        int* curve;
        malloc1Darray(geometry->curve[i].n_line, &curve, -1);
        for (j = 0; j < geometry->curve[i].n_line; j++) {
            curve[j] = geometry->curve[i].line[j] + 1;
        }
        gmshModelOccAddCurveLoop(curve, geometry->curve[i].n_line, i + 1, &ierr);
        free(curve);
    }

    /* add gmsh surface */
    for (i = 0; i < geometry->surface_num; i++) {
        // set dimtag
        geometry->surface[i].DimTags_n = 0;
        int ret;
        // no need to consider create from occ, opendfn 2d only consider create from curve
        int* surface;
        malloc1Darray(geometry->surface[i].n_curve, &surface, -1);
        for (j = 0; j < geometry->surface[i].n_curve; j++) {
            surface[j] = geometry->surface[i].curve[j] + 1;
        }
        ret = gmshModelOccAddPlaneSurface(surface, geometry->surface[i].n_curve, i + 1, &ierr);
        free(surface);
        // set dimtag
        geometry->surface[i].DimTags = (int*)malloc(sizeof(int) * 2);
        // dimesion
        geometry->surface[i].DimTags[0] = 2;
        // id
        geometry->surface[i].DimTags[1] = ret;
        // size
        geometry->surface[i].DimTags_n = 2;
    }

    /* do gmsh surface-surface operator */
    for (i = 0; i < geometry->surface_num; i++) {
        // do difference operation, the oveject will be kept as the same tag
        // and the tool will be deleted, should be marked otherwise will assign
        // physical group to this tag
        // do surface-surface remove operation
        if (geometry->surface[i].n_removetool) {
            /*  int *  ov, **ovv;
              size_t ov_n, *ovv_n, ovv_nn;*/
            // the current surface
            const int o2[] = {2, i + 1};
            // bool cut, and delete the origional operator tool solids, the object tag will not change
            // object is removed, tool is removed, new generated will be used
            gmshModelOccCut(o2, 2, geometry->surface[i].removetool, 2 * geometry->surface[i].n_removetool, NULL, NULL, NULL, NULL,
                            NULL, -1, 1, 1, &ierr);
            /*    for (size_t ii = 0; ii < ovv_nn; ii++) {
                    printf(" (after ii %d)\n", ii);
                    for (size_t j = 0; j < ovv_n[ii]; j += 2) {
                        printf(" (after %d, %d)", ovv[ii][j], ovv[ii][j + 1]);
                    }
                    printf("\n");
                }*/
            // unmark the tools surfaces becuase ther are deleted, avoid errors when add physcial groups
            for (size_t ii = 0; ii < geometry->surface[i].n_removetool; ii++) {
                // back to geometry id, shoud -1
                geometry->surface[geometry->surface[i].removetool[2 * ii + 1] - 1].DimTags_n = 0;
            }
        }
        // do fragment operation, the split will be done by cut rather than fragment, becasue
        // fragment will change the tags of the object, the tool should be kept after difference
        // object is removed, but the tool is left
        //###########################the following function is deleted
        if (geometry->surface[i].n_fragmenttool) {
            const int o2[] = {2, i + 1};
            gmshModelOccFragment(o2, 2, geometry->surface[i].fragmenttool, 2 * geometry->surface[i].n_fragmenttool, NULL, NULL,
                                 NULL, NULL, NULL, -1, 1, 1, &ierr);
        }
    }
    // update the above operation
    gmshModelOccSynchronize(&ierr);

    /* do gmsh curve-surface operator for DFNs */
    for (i = 0; i < geometry->surface_num; i++) {
        // do fragment operation
        // surface - curve fragment
        if (geometry->surface[i].n_boollines) {
            int *  ov, **ovv;
            size_t ov_n, *ovv_n, ovv_nn;
            // remove the old surface tools and new surface tools are stored in ovv
            /*for (size_t j = 0; j < geometry->surface[i].n_boollines; j++) {
                printf(" (old %d, %d)", geometry->surface[i].boollines[2 * j], geometry->surface[i].boollines[2 * j + 1]);
                printf("\n");
            }*/
            // setp 1, obtain the intersected joints in the surface，the tools will be deleted, the object is left
            /* obtain the intersection of DFN sets*/
            // ov is the output DimTag of the entities, and  ov_n is the count of the
            // entities. ov is 1D array with 2ov_n count.
            // ovv is the map of the out DimTag, and ovv_nn is the id of the first
            // dimesion of the input and ovv_n is the count of that the origional 1D lines was broken to fragments
            // ovv is a 2D array and ovv_n is a 1D array
            // ovv stores the parent-child relationship before and after intersects
            // ovv[i][j] is the old ith line is broken, and the new jth new lines
            // ovv firstly shows the object, for example, if the sruface DimTags_n is 4, and the line DimTags will start from 2,
            // (0 and 1 are for surfaces)
            gmshModelOccIntersect(geometry->surface[i].DimTags, geometry->surface[i].DimTags_n, geometry->surface[i].boollines,
                                  2 * geometry->surface[i].n_boollines, &ov, &ov_n, &ovv, &ovv_n, &ovv_nn, -1, 0, 1, &ierr);
            // ovv contains the parent-child relationships for all the input entities:
            /* for (size_t ii = 0; ii < ovv_nn; ii++) {
                 printf(" (ii %d)\n", ii);
                 for (size_t j = 0; j < ovv_n[ii]; j += 2) {
                     printf(" (%d, %d)", ovv[ii][j], ovv[ii][j + 1]);
                 }
                 printf("\n");
             }*/
            // update the operation
            gmshModelOccSynchronize(&ierr);

            // put the intersected surfaces
            int*    multiple_dfn;
            int     multiple_dfn_n = 0;
            int     oldLineid;
            int*    out;
            size_t  n;
            int     short_line[2] = {1, 1};
            double* coord;
            double  parametricCoord[] = {0};
            size_t  coord_n;
            double  position[4];
            int*    short_dfn;
            int     short_dfn_n = 0;
            // get the broken dfns lines
            // ovv_nn 是输入lines的总数
            // ovv_n[i] is the information of new broken lines, starts from 1 because 0 is for the surface
            // ii is the id cooresponding to the input DimTags, rather the real line id, therefore should be changed to
            // boollines[2(ii - 1) + 1] in boollines, 0 is for dimension, which is 1 and 1 is for the tag of lines in gmsh
            // (already + 1)
            for (int ii = 1; ii < ovv_nn; ii++) {
                // ii 对应 boollines的编号，lineid should be boollines[2(ii - 1) + 1], and geometry line id should be lineid - 1
                // line indices = tag - 1, tag is 2k + 1
                // go back to the geometry id
                oldLineid = geometry->surface[i].boollines[2 * (ii - 1) + 1] - 1;
                // to loop the new broken lines
                for (int j = 0; j < ovv_n[ii]; j++) {
                    if (!geometry->line[oldLineid].tobeReserved) {
                        bool ishort = false;
                        // the dimTag pair of the broken line
                        short_line[0] = ovv[ii][j];
                        short_line[1] = ovv[ii][j + 1];
                        // get the node id of the line, the node id is refreshed
                        // out is the DimTag pair of the node
                        gmshModelGetBoundary(short_line, 2, &out, &n, 0, 0, 0, &ierr);
                        for (int m = 0; m < n / 2; m++) {
                            // get the node coords, coord is x, y, z dimesion
                            gmshModelGetValue(0, out[2 * m + 1], parametricCoord, 0, &coord, &coord_n, &ierr);
                            position[2 * m]     = coord[0]; /* x direction */
                            position[2 * m + 1] = coord[1]; /* y direction */
                        }
                        // compute the distance of the short lines
                        double dis = sqrt((position[0] - position[2]) * (position[0] - position[2]) +
                                          (position[1] - position[3]) * (position[1] - position[3]));
                        // add to shot dfn list if the length is small than the
                        // critical value, is set as 0.1 of the minmuum mesh size
                        // small DFNs will increase the mesh burden and
                        // crash the calculation
                        if (dis < geometry->minmuumSize) {
                            // store the short lines because we need to delete them later
                            buildmemory(&short_dfn, (short_dfn_n + 2), short_dfn_n);
                            short_dfn[short_dfn_n] = ovv[ii][j];
                            short_dfn_n++;
                            j++;
                            short_dfn[short_dfn_n] = ovv[ii][j];
                            short_dfn_n++;
                            ishort = true;
                            // no need to store to multiple dfns becasue they are short
                            continue;
                        }
                    }
                    // build the new lines for the next step fragmentation
                    buildmemory(&multiple_dfn, (multiple_dfn_n + 2), multiple_dfn_n);
                    multiple_dfn[multiple_dfn_n] = ovv[ii][j];
                    multiple_dfn_n++;
                    j++;
                    multiple_dfn[multiple_dfn_n] = ovv[ii][j];
                    multiple_dfn_n++;

                    // send to new dimtag1, because we need to link the tags, should note the j is updated
                    buildmemory(&geometry->line[oldLineid].DimTags1, (geometry->line[oldLineid].DimTags1_n + 2),
                                geometry->line[oldLineid].DimTags1_n);
                    geometry->line[oldLineid].DimTags1[geometry->line[oldLineid].DimTags1_n] = ovv[ii][j - 1];
                    geometry->line[oldLineid].DimTags1_n++;
                    geometry->line[oldLineid].DimTags1[geometry->line[oldLineid].DimTags1_n] = ovv[ii][j];
                    geometry->line[oldLineid].DimTags1_n++;
                    // mark the layers, default is zero
                    geometry->line[oldLineid].layers = 1;
                }
                // push back to line structure
                if (ovv_n[ii] == 0) {
                    // unmark the surface for physcial group if it is deleted in the insection operator
                    geometry->line[oldLineid].DimTags_n = 0;
                }
            }
            if (!multiple_dfn_n) {
                OpenDFNMessage::RuntimebackStringString("Warning: no joint created in ", geometry->surface[i].tag);
            }

            // step 3, fragment the dfns
            //  delete the old tools and objects, the tool is not changed but the object may be broken
            if (multiple_dfn_n) {
                gmshModelOccFragment(geometry->surface[i].DimTags, geometry->surface[i].DimTags_n, multiple_dfn, multiple_dfn_n,
                                     &ov, &ov_n, &ovv, &ovv_n, &ovv_nn, -1, 1, 1, &ierr);

                /* for (size_t ii = 0; ii < ovv_nn; ii++) {
                     printf(" (after ii %d)\n", ii);
                     for (size_t j = 0; j < ovv_n[ii]; j += 2) {
                         printf(" (after %d, %d)", ovv[ii][j], ovv[ii][j + 1]);
                     }
                     printf("\n");
                 }*/
                // for the 2nd layer fragment lines
                for (int ii = 1; ii < ovv_nn; ii++) {
                    // ii 对应 boollines的编号，lineid should be boollines[2(ii - 1) + 1], and geometry line id should be lineid -
                    // 1 line indices = tag - 1, tag is 2k + 1
                    // as om step 2, we firstly find back the id in multiples
                    Int oldLineInMutiple = multiple_dfn[2 * (ii - 1) + 1];
                    // oldLineid            = geometry->surface[i].boollines[2 * (oldLineInMutiple - 1) + 1] - 1;
                    // to find the line id by seaching all line DimTags1
                    Int findtag2 = -1;
                    _for(j, 0, geometry->line_num) {
                        if (geometry->line[j].DimTags1_n) {
                            _for(k, 0, geometry->line[j].DimTags1_n) {
                                if (oldLineInMutiple == geometry->line[j].DimTags1[k]) {
                                    findtag2 = j;
                                    break;
                                }
                            }
                            if (findtag2 > -1)
                                break;
                        }
                    }
                    if (findtag2 > -1)
                        oldLineid = findtag2;
                    else
                        printf("Fials to found ##################################\n");

                    // store the broken lines in DiemTags2
                    for (int j = 0; j < ovv_n[ii]; j++) {
                        // send to new dimtag
                        buildmemory(&geometry->line[oldLineid].DimTags2, (geometry->line[oldLineid].DimTags2_n + 2),
                                    geometry->line[oldLineid].DimTags2_n);
                        geometry->line[oldLineid].DimTags2[geometry->line[oldLineid].DimTags2_n] = ovv[ii][j];
                        geometry->line[oldLineid].DimTags2_n++;
                        j++;
                        geometry->line[oldLineid].DimTags2[geometry->line[oldLineid].DimTags2_n] = ovv[ii][j];
                        geometry->line[oldLineid].DimTags2_n++;
                        geometry->line[oldLineid].layers = 2;
                    }
                    // push back to line structure
                    if (ovv_n[ii] == 0) {
                        // unmark the surface for physcial group if it is deleted in the insection operator
                        geometry->line[oldLineid].DimTags_n  = 0;
                        geometry->line[oldLineid].DimTags1_n = 0;
                    }
                }

                // need to update volumes for physcial groups
                if (ovv_n[0] > 0) {
                    geometry->surface[i].DimTags = (int*)realloc(geometry->surface[i].DimTags, sizeof(int) * ovv_n[0]);
                    memcpy(geometry->surface[i].DimTags, ovv[0], sizeof(int) * ovv_n[0]);
                    geometry->surface[i].DimTags_n = ovv_n[0];
                } else {
                    // unmark the surface for physcial group if it is deleted in the insection operator
                    geometry->surface[i].DimTags_n = 0;
                }
            }
            // delete the shorts
            if (short_dfn_n) {
                gmshModelOccSynchronize(&ierr);
                gmshModelOccRemove(short_dfn, short_dfn_n, 1, &ierr);
            }
        }
    }

    gmshOptionSetNumber("Mesh.Smoothing", 1, &ierr);
    gmshOptionSetNumber("Mesh.SaveAll", 0, &ierr);
    /*gmshOptionSetNumber("Mesh.SaveGroupsOfNodes", 1, &ierr);*/

    gmshOptionSetNumber("Mesh.Algorithm", 2, &ierr);              /* default: mesh adapt */
    gmshOptionSetNumber("Mesh.MeshSizeFromCurvature", 20, &ierr); /* default: mesh adapt */

    if (isSameString(keyword, "meshadapt")) {
        gmshOptionSetNumber("Mesh.Algorithm", 1, &ierr); /* mesh adapt */
    } else if (isSameString(keyword, "delaunay")) {
        gmshOptionSetNumber("Mesh.Algorithm", 2, &ierr); /* mesh delaunay */
    } else if (isSameString(keyword, "frontal-delaunay")) {
        gmshOptionSetNumber("Mesh.Algorithm", 6, &ierr); /* mesh frontal-delaunay */
    }

    gmshModelOccSynchronize(&ierr);

    // add point physcial group
    addPhysicalSetsforPoints(geometry, ierr);

    /* add gmsh surface group */
    for (i = 0; i < geometry->surface_num; i++) {
        // only add physcial group for undeleted surfaces
        // assign back the tag for all broken surfaces
        if (geometry->surface[i].DimTags_n) {
            int* surface_id = (int*)malloc(sizeof(int) * geometry->surface[i].DimTags_n / 2);
            _for(j, 0, geometry->surface[i].DimTags_n / 2) surface_id[j] = geometry->surface[i].DimTags[2 * j + 1];
            // create physcial id and get this id of the surface
            physical_id = gmshModelAddPhysicalGroup(2, surface_id, geometry->surface[i].DimTags_n / 2, -1, "", &ierr);
            // assign the tag to the physcial set
            // be caution when the old surface is deleted for DFNs
            gmshModelSetPhysicalName(2, physical_id, geometry->surface[i].tag, &ierr);
        }
    }

    /* add gmsh line element group */
    for (i = 0; i < geometry->line_num; i++) {
        if (geometry->line[i].iselement) {
            bool skip = false;
            _for(iGroup, 0, geometry->linegroups_num) {
                if (isSameString(geometry->line[i].tag, geometry->linegroups[iGroup].tag)) {
                    skip = true;
                    break;
                }
            }
            if (skip)
                continue;
            // get the broken lines
            int* line_id = (int*)malloc(sizeof(int) * geometry->line[i].DimTags_n / 2);
            // set the line set
            _for(j, 0, geometry->line[i].DimTags_n / 2) line_id[j] = geometry->line[i].DimTags[2 * j + 1];
            // create physcial id and get this id of the surface
            physical_id = gmshModelAddPhysicalGroup(1, line_id, geometry->line[i].DimTags_n / 2, -1, "", &ierr);
            // assign the tag to the physcial set
            // be caution when the old surface is deleted for DFNs
            gmshModelSetPhysicalName(1, physical_id, geometry->line[i].tag, &ierr);
        }
    }

    // assemble the line groups for broken dfns
    for (i = 0; i < geometry->line_num; i++) {
        if (geometry->line[i].iselement) {
            if (geometry->line[i].layers == 0) {
                if (geometry->line[i].DimTags_n) {
                    // loop all linegroups
                    _for(iGroup, 0, geometry->linegroups_num) {
                        if (isSameString(geometry->line[i].tag, geometry->linegroups[iGroup].tag)) {
                            buildmemory(&geometry->linegroups[iGroup].DimTags,
                                        (geometry->linegroups[iGroup].DimTags_n + geometry->line[i].DimTags_n),
                                        geometry->linegroups[iGroup].DimTags_n);
                            // copy the dimetag from lines to linegroups if they have the same tags
                            _for(j, 0, geometry->line[i].DimTags_n) {
                                geometry->linegroups[iGroup].DimTags[geometry->linegroups[iGroup].DimTags_n] =
                                    geometry->line[i].DimTags[j];
                                geometry->linegroups[iGroup].DimTags_n++;
                            }
                        }
                    }
                }
            }
        }
        if (geometry->line[i].isembeded) {
            if (geometry->line[i].layers == 1) {
                if (geometry->line[i].DimTags1_n) {
                    // loop all linegroups
                    _for(iGroup, 0, geometry->linegroups_num) {
                        if (isSameString(geometry->line[i].tag, geometry->linegroups[iGroup].tag)) {
                            buildmemory(&geometry->linegroups[iGroup].DimTags,
                                        (geometry->linegroups[iGroup].DimTags_n + geometry->line[i].DimTags1_n),
                                        geometry->linegroups[iGroup].DimTags_n);
                            // copy the dimetag from lines to linegroups if they have the same tags
                            _for(j, 0, geometry->line[i].DimTags_n) {
                                geometry->linegroups[iGroup].DimTags[geometry->linegroups[iGroup].DimTags_n] =
                                    geometry->line[i].DimTags1[j];
                                geometry->linegroups[iGroup].DimTags_n++;
                            }
                        }
                    }
                }
            } else if (geometry->line[i].layers == 2) {
                if (geometry->line[i].DimTags2_n) {
                    // loop all linegroups
                    _for(iGroup, 0, geometry->linegroups_num) {
                        if (isSameString(geometry->line[i].tag, geometry->linegroups[iGroup].tag)) {
                            buildmemory(&geometry->linegroups[iGroup].DimTags,
                                        (geometry->linegroups[iGroup].DimTags_n + geometry->line[i].DimTags2_n),
                                        geometry->linegroups[iGroup].DimTags_n);
                            // copy the dimetag from lines to linegroups if they have the same tags
                            _for(j, 0, geometry->line[i].DimTags2_n) {
                                geometry->linegroups[iGroup].DimTags[geometry->linegroups[iGroup].DimTags_n] =
                                    geometry->line[i].DimTags2[j];
                                geometry->linegroups[iGroup].DimTags_n++;
                            }
                        }
                    }
                }
            }
        }
    }

    /* add gmsh group line for joint(cut) group */
    for (i = 0; i < geometry->linegroups_num; i++) {
        if (geometry->linegroups[i].DimTags_n) {
            // get the broken lines
            int* line_id = (int*)malloc(sizeof(int) * geometry->linegroups[i].DimTags_n / 2);
            // set the line set
            _for(j, 0, geometry->linegroups[i].DimTags_n / 2) line_id[j] = geometry->linegroups[i].DimTags[2 * j + 1];
            // create physcial id and get this id of the surface
            physical_id = gmshModelAddPhysicalGroup(1, line_id, geometry->linegroups[i].DimTags_n / 2, -1, "", &ierr);
            // assign the tag to the physcial set
            // be caution when the old surface is deleted for DFNs
            gmshModelSetPhysicalName(1, physical_id, geometry->linegroups[i].tag, &ierr);
            gmshModelOccSynchronize(&ierr);
        }
    }

    /* reassign the mesh size */
    if (geometry->mesh_size > 0.0) {
        setGlobalMeshSize(geometry, ierr);
    }
    gmshModelOccSynchronize(&ierr);

    /* reassign mesh size of by groups */
    if (geometry->group_num) {
        setMeshSizefromGroups(geometry, ierr);
    }

    /* set recombined type */
    if (geometry->recombined) {
        for (int i = 0; i < geometry->surface_num; i++) {
            if (geometry->surface[i].recombined > -1) {
                gmshModelMeshSetRecombine(2, i + 1, 45, &ierr);
            }
        }
        // refresh
        gmshModelOccSynchronize(&ierr);
    }
    if (isGmshGuiEnabled())
        gmshFltkRun(&ierr);
    /* generate mesh */
    gmshModelMeshGenerate(2, &ierr);

    /* set mesh order*/
    if (geometry->order > 1) {
        const int order = geometry->order;
        gmshModelMeshSetOrder(order, &ierr);
    }
    /* set gmsh version */
    gmshOptionSetNumber("Mesh.MshFileVersion", 2.0, &ierr);

    std::filesystem::path meshPath = writeFlag == 1
                                         ? std::filesystem::path(keyword)
                                         : std::filesystem::path(std::string(general->InputFileName) + ".msh");
    if (!writeModelFormats(meshPath, ierr)) {
        OpenDFNMessage::RuntimeInfo("Error: failed to write MSH, GEO and VTK outputs.");
    }

    if (isGmshGuiEnabled())
        gmshFltkRun(&ierr);
    gmshFinalize(&ierr);

    // aggregate is intentionally disabled. This tool only WRITES the .msh file.
    // GmshIO::gmsh_io here is a no-op stub; the call is left out entirely.
    // free geometry
    geometry->clear();
    OpenDFNMessage::RuntimeInfo("Geometry is freeup.");
}
/**
 * \brief Apply the global mesh size to every point entity of the current Gmsh model.
 *
 * Queries all dimension-0 entities from both the OpenCASCADE and built-in Gmsh
 * kernels and assigns \c geometry->mesh_size as their characteristic length.
 *
 * \param geometry Geometry providing the global \c mesh_size to apply.
 * \param ierr     Gmsh error code, updated by the underlying Gmsh calls.
 *
 * \note Side effects: mutates the Gmsh model's mesh sizing and frees the temporary
 *       dimTags buffer returned by Gmsh.
 */
void GeometryBuilder::setGlobalMeshSize(Geometry geometry, int& ierr) {
    int*   dimTags;
    size_t dimTags_n;
    gmshModelOccGetEntities(&dimTags, &dimTags_n, 0, &ierr); /* get the list of dimesion 0 */
    gmshModelOccMeshSetSize(dimTags, dimTags_n, geometry->mesh_size, &ierr);
    gmshModelGetEntities(&dimTags, &dimTags_n, 0, &ierr);
    gmshModelMeshSetSize(dimTags, dimTags_n, geometry->mesh_size, &ierr);
    gmshFree(dimTags);
}

/**
 * \brief Assign per-group mesh sizes to Gmsh point entities selected by geometry groups.
 *
 * Retrieves the coordinates of every point entity, then for each geometry group
 * that defines a \c mesh_size selects the points falling inside/on/outside the
 * group region (box, circle, plane half-spaces) or belonging to referenced line
 * or surface tags, and applies the group's mesh size to those points. Groups that
 * match no entity emit a warning. Selection is dispatched by \c GroupType.
 *
 * \param geometry Geometry holding the group definitions and their mesh sizes.
 * \param ierr     Gmsh error code, updated by the underlying Gmsh calls.
 *
 * \note Side effects: mutates the Gmsh model's mesh sizing, allocates/frees
 *       temporary buffers, and emits warning messages for empty groups.
 */
void GeometryBuilder::setMeshSizefromGroups(Geometry geometry, int& ierr) {
    /* fresh mesh size of group */
    int      i, j;
    int*     dimTag; /* {dim, tag, dim, tag, dim, tag...}*/
    size_t   dimTag_n;
    double*  coord = NULL;
    Vector2* Coords;
    int*     ov;
    size_t   ov_n;
    double   parametricCoord[] = {0};
    size_t   coord_n;
    // get all nodes after gmsh processing
    gmshModelGetEntities(&dimTag, &dimTag_n, 0, &ierr);
    /* initialize coords for all nodes */
    buildmemory(&Coords, dimTag_n / 2);

    /* get the coords of all nodes */
    for (i = 0; i < dimTag_n; i++) {
        gmshModelGetValue(0, dimTag[i + 1], parametricCoord, 0, &coord, &coord_n, &ierr);
        Coords[i / 2].x = coord[0];
        Coords[i / 2].y = coord[1];
        i++;
    }
    if (coord != NULL)
        gmshFree(coord);

    int*   dimTags;
    size_t dimTags_n = 0;
    for (i = 0; i < geometry->group_num; i++) {
        if (geometry->group[i].mesh_size) {
            switch (geometry->group[i].type) {
                case GroupType::RETANGULARIN:
                    dimTags_n = 0;
                    for (j = 0; j < dimTag_n; j++) {
                        if (MeshUtils::isinBox(Coords[j / 2], geometry->group[i].range)) {
                            if (!dimTags_n)
                                dimTags = (int*)malloc(sizeof(int) * 2);
                            else
                                dimTags = (int*)realloc(dimTags, sizeof(int) * (dimTags_n + 2));
                            dimTags[dimTags_n]     = dimTag[j];
                            dimTags[dimTags_n + 1] = dimTag[j + 1];
                            dimTags_n              = dimTags_n + 2;
                        }
                        j++;
                    }
                    if (dimTags_n) {
                        gmshModelOccMeshSetSize(dimTags, dimTags_n, geometry->group[i].mesh_size, &ierr);
                        gmshModelMeshSetSize(dimTags, dimTags_n, geometry->group[i].mesh_size, &ierr);
                        free(dimTags);
                    } else {
                        OpenDFNMessage::RuntimebackStringString("Warning: no mesh size assigned in geometry group ",
                                                                 geometry->group[i].tag);
                    }
                    break;
                case GroupType::RETANGULARON:
                    dimTags_n = 0;
                    for (j = 0; j < dimTag_n; j++) {
                        if (MeshUtils::isonBox(Coords[j / 2], geometry->group[i].range)) {
                            if (!dimTags_n)
                                dimTags = (int*)malloc(sizeof(int) * 2);
                            else
                                dimTags = (int*)realloc(dimTags, sizeof(int) * (dimTags_n + 2));
                            dimTags[dimTags_n]     = dimTag[j];
                            dimTags[dimTags_n + 1] = dimTag[j + 1];
                            dimTags_n              = dimTags_n + 2;
                        }
                        j++;
                    }
                    if (dimTags_n) {
                        gmshModelOccMeshSetSize(dimTags, dimTags_n, geometry->group[i].mesh_size, &ierr);
                        gmshModelMeshSetSize(dimTags, dimTags_n, geometry->group[i].mesh_size, &ierr);
                        free(dimTags);
                    } else {
                        OpenDFNMessage::RuntimebackStringString("Warning: no mesh size assigned in geometry group ",
                                                                 geometry->group[i].tag);
                    }
                    break;
                case GroupType::RETANGULAROUT:
                    dimTags_n = 0;
                    for (j = 0; j < dimTag_n; j++) {
                        if (MeshUtils::isoutBox(Coords[j / 2], geometry->group[i].range)) {
                            if (!dimTags_n)
                                dimTags = (int*)malloc(sizeof(int) * 2);
                            else
                                dimTags = (int*)realloc(dimTags, sizeof(int) * (dimTags_n + 2));
                            dimTags[dimTags_n]     = dimTag[j];
                            dimTags[dimTags_n + 1] = dimTag[j + 1];
                            dimTags_n              = dimTags_n + 2;
                        }
                        j++;
                    }
                    if (dimTags_n) {
                        gmshModelOccMeshSetSize(dimTags, dimTags_n, geometry->group[i].mesh_size, &ierr);
                        gmshModelMeshSetSize(dimTags, dimTags_n, geometry->group[i].mesh_size, &ierr);
                        free(dimTags);
                    } else {
                        OpenDFNMessage::RuntimebackStringString("Warning: no mesh size assigned in geometry group ",
                                                                 geometry->group[i].tag);
                    }
                    break;
                case GroupType::CIRCLEIN:
                    dimTags_n = 0;
                    for (j = 0; j < dimTag_n; j++) {
                        if (MeshUtils::isinCircle(Coords[j / 2], geometry->group[i].range)) {
                            if (!dimTags_n)
                                dimTags = (int*)malloc(sizeof(int) * 2);
                            else
                                dimTags = (int*)realloc(dimTags, sizeof(int) * (dimTags_n + 2));
                            dimTags[dimTags_n]     = dimTag[j];
                            dimTags[dimTags_n + 1] = dimTag[j + 1];
                            dimTags_n              = dimTags_n + 2;
                        }
                        j++;
                    }
                    if (dimTags_n) {
                        gmshModelOccMeshSetSize(dimTags, dimTags_n, geometry->group[i].mesh_size, &ierr);
                        gmshModelMeshSetSize(dimTags, dimTags_n, geometry->group[i].mesh_size, &ierr);
                        free(dimTags);
                    } else {
                        OpenDFNMessage::RuntimebackStringString("Warning: no mesh size assigned in geometry group ",
                                                                 geometry->group[i].tag);
                    }
                    break;
                case GroupType::CIRCLEON:
                    dimTags_n = 0;
                    for (j = 0; j < dimTag_n; j++) {
                        if (MeshUtils::isonCircle(Coords[j / 2], geometry->group[i].range)) {
                            if (!dimTags_n)
                                dimTags = (int*)malloc(sizeof(int) * 2);
                            else
                                dimTags = (int*)realloc(dimTags, sizeof(int) * (dimTags_n + 2));
                            dimTags[dimTags_n]     = dimTag[j];
                            dimTags[dimTags_n + 1] = dimTag[j + 1];
                            dimTags_n              = dimTags_n + 2;
                        }
                        j++;
                    }
                    if (dimTags_n) {
                        gmshModelOccMeshSetSize(dimTags, dimTags_n, geometry->group[i].mesh_size, &ierr);
                        gmshModelMeshSetSize(dimTags, dimTags_n, geometry->group[i].mesh_size, &ierr);
                        free(dimTags);
                    } else {
                        OpenDFNMessage::RuntimebackStringString("Warning: no mesh size assigned in geometry group ",
                                                                 geometry->group[i].tag);
                    }
                    break;
                case GroupType::CIRCLEOUT:
                    dimTags_n = 0;
                    for (j = 0; j < dimTag_n; j++) {
                        if (MeshUtils::isoutCircle(Coords[j / 2], geometry->group[i].range)) {
                            if (!dimTags_n)
                                dimTags = (int*)malloc(sizeof(int) * 2);
                            else
                                dimTags = (int*)realloc(dimTags, sizeof(int) * (dimTags_n + 2));
                            dimTags[dimTags_n]     = dimTag[j];
                            dimTags[dimTags_n + 1] = dimTag[j + 1];
                            dimTags_n              = dimTags_n + 2;
                        }
                        j++;
                    }
                    if (dimTags_n) {
                        gmshModelOccMeshSetSize(dimTags, dimTags_n, geometry->group[i].mesh_size, &ierr);
                        gmshModelMeshSetSize(dimTags, dimTags_n, geometry->group[i].mesh_size, &ierr);
                        free(dimTags);
                    } else {
                        OpenDFNMessage::RuntimebackStringString("Warning: no mesh size assigned in geometry group ",
                                                                 geometry->group[i].tag);
                    }
                    break;
                case GroupType::PLANEON:
                    dimTags_n = 0;
                    for (j = 0; j < dimTag_n; j++) {
                        if (MeshUtils::isonPlane(Coords[j / 2], geometry->group[i].range) &&
                            !MeshUtils::isoutAABB(Coords[j / 2], geometry->group[i].range)) {
                            if (!dimTags_n)
                                dimTags = (int*)malloc(sizeof(int) * 2);
                            else
                                dimTags = (int*)realloc(dimTags, sizeof(int) * (dimTags_n + 2));
                            dimTags[dimTags_n]     = dimTag[j];
                            dimTags[dimTags_n + 1] = dimTag[j + 1];
                            dimTags_n              = dimTags_n + 2;
                        }
                        j++;
                    }
                    if (dimTags_n) {
                        gmshModelOccMeshSetSize(dimTags, dimTags_n, geometry->group[i].mesh_size, &ierr);
                        gmshModelMeshSetSize(dimTags, dimTags_n, geometry->group[i].mesh_size, &ierr);
                        free(dimTags);
                    } else {
                        OpenDFNMessage::RuntimebackStringString("Warning: no mesh size assigned in geometry group ",
                                                                 geometry->group[i].tag);
                    }
                    break;
                case GroupType::PLANELEFT:
                    dimTags_n = 0;
                    for (j = 0; j < dimTag_n; j++) {
                        if (MeshUtils::isonleftofPlane(Coords[j / 2], geometry->group[i].range) &&
                            !MeshUtils::isoutAABB(Coords[j / 2], geometry->group[i].range)) {
                            if (!dimTags_n)
                                dimTags = (int*)malloc(sizeof(int) * 2);
                            else
                                dimTags = (int*)realloc(dimTags, sizeof(int) * (dimTags_n + 2));
                            dimTags[dimTags_n]     = dimTag[j];
                            dimTags[dimTags_n + 1] = dimTag[j + 1];
                            dimTags_n              = dimTags_n + 2;
                        }
                        j++;
                    }
                    if (dimTags_n) {
                        gmshModelOccMeshSetSize(dimTags, dimTags_n, geometry->group[i].mesh_size, &ierr);
                        gmshModelMeshSetSize(dimTags, dimTags_n, geometry->group[i].mesh_size, &ierr);
                        free(dimTags);
                    } else {
                        OpenDFNMessage::RuntimebackStringString("Warning: no mesh size assigned in geometry group ",
                                                                 geometry->group[i].tag);
                    }
                    break;
                case GroupType::PLANERIGHT:
                    dimTags_n = 0;
                    for (j = 0; j < dimTag_n; j++) {
                        if (MeshUtils::isonrightofPlane(Coords[j / 2], geometry->group[i].range) &&
                            !MeshUtils::isoutAABB(Coords[j / 2], geometry->group[i].range)) {
                            if (!dimTags_n)
                                dimTags = (int*)malloc(sizeof(int) * 2);
                            else
                                dimTags = (int*)realloc(dimTags, sizeof(int) * (dimTags_n + 2));
                            dimTags[dimTags_n]     = dimTag[j];
                            dimTags[dimTags_n + 1] = dimTag[j + 1];
                            dimTags_n              = dimTags_n + 2;
                        }
                        j++;
                    }
                    if (dimTags_n) {
                        gmshModelOccMeshSetSize(dimTags, dimTags_n, geometry->group[i].mesh_size, &ierr);
                        gmshModelMeshSetSize(dimTags, dimTags_n, geometry->group[i].mesh_size, &ierr);
                        free(dimTags);
                    } else {
                        OpenDFNMessage::RuntimebackStringString("Warning: no mesh size assigned in geometry group ",
                                                                 geometry->group[i].tag);
                    }
                    break;
                case GroupType::FROMLINETAGS: {
                    bool notsetmeshsize = true;
                    _for(j, 0, geometry->line_num) {
                        dimTags_n = 0;
                        if (isSameString(geometry->line[j].tag, geometry->group[i].tag)) {
                            if (geometry->line[j].layers == 0 && geometry->line[j].DimTags_n) {
                                dimTags_n = geometry->line[j].DimTags_n;
                                buildmemory(&dimTags, dimTags_n);
                                copyRawArray(geometry->line[j].DimTags, dimTags, dimTags_n);
                            } else if (geometry->line[j].layers == 1 && geometry->line[j].DimTags1_n) {
                                dimTags_n = geometry->line[j].DimTags1_n;
                                buildmemory(&dimTags, dimTags_n);
                                copyRawArray(geometry->line[j].DimTags1, dimTags, dimTags_n);
                            } else if (geometry->line[j].layers == 2 && geometry->line[j].DimTags2_n) {
                                dimTags_n = geometry->line[j].DimTags2_n;
                                buildmemory(&dimTags, dimTags_n);
                                copyRawArray(geometry->line[j].DimTags2, dimTags, dimTags_n);
                            }
                        }
                        if (dimTags_n) {
                            notsetmeshsize = false;
                            gmshModelGetBoundary(dimTags, dimTags_n, &ov, &ov_n, 0, 0, 1, &ierr);
                            gmshModelOccMeshSetSize(ov, ov_n, geometry->group[i].mesh_size, &ierr);
                            gmshModelMeshSetSize(ov, ov_n, geometry->group[i].mesh_size, &ierr);
                            free(dimTags);
                        }
                    }
                    if (notsetmeshsize) {
                        OpenDFNMessage::RuntimebackStringString("Warning: no mesh size assigned in geometry group ",
                                                                 geometry->group[i].tag);
                    }
                } break;
                case GroupType::FROMSURFACETAGS:
                    dimTags_n = 0;
                    _for(j, 0, geometry->surface_num) {
                        if (isSameString(geometry->surface[j].tag, geometry->group[i].tag)) {
                            if (geometry->surface[j].DimTags_n) {
                                dimTags_n = geometry->surface[j].DimTags_n;
                                buildmemory(&dimTags, dimTags_n);
                                copyRawArray(geometry->surface[j].DimTags, dimTags, dimTags_n);
                                break;
                            }
                        }
                    }
                    if (dimTags_n) {
                        gmshModelGetBoundary(dimTags, dimTags_n, &ov, &ov_n, 0, 0, 1, &ierr);
                        gmshModelOccMeshSetSize(ov, ov_n, geometry->group[i].mesh_size, &ierr);
                        gmshModelMeshSetSize(ov, ov_n, geometry->group[i].mesh_size, &ierr);
                        free(dimTags);
                    } else {
                        OpenDFNMessage::RuntimebackStringString("Warning: no mesh size assigned in geometry group ",
                                                                 geometry->group[i].tag);
                    }
                    break;
                case GroupType::FROMCOORD:
                    dimTags_n = 0;
                    for (j = 0; j < dimTag_n; j++) {
                        if (MeshUtils::isinCircle(Coords[j / 2], geometry->group[i].range)) {
                            if (!dimTags_n)
                                dimTags = (int*)malloc(sizeof(int) * 2);
                            else
                                dimTags = (int*)realloc(dimTags, sizeof(int) * (dimTags_n + 2));
                            dimTags[dimTags_n]     = dimTag[j];
                            dimTags[dimTags_n + 1] = dimTag[j + 1];
                            dimTags_n              = dimTags_n + 2;
                        }
                        j++;
                    }
                    if (dimTags_n) {
                        gmshModelOccMeshSetSize(dimTags, dimTags_n, geometry->group[i].mesh_size, &ierr);
                        gmshModelMeshSetSize(dimTags, dimTags_n, geometry->group[i].mesh_size, &ierr);
                        free(dimTags);
                    } else {
                        OpenDFNMessage::RuntimebackStringString("Warning: no mesh size assigned in geometry group ",
                                                                 geometry->group[i].tag);
                    }
                    break;
                default: break;
            }
        }
    }
    if (coord != NULL)
        gmshFree(dimTag);
    if (Coords != NULL) {
        free(Coords);
    }
}

/**
 * \brief Create Gmsh point physical groups for geometry groups defined from coordinates.
 *
 * Retrieves the coordinates of every point entity, then for each group flagged
 * with \c isInsertedPoints and of type \c GroupType::FROMCOORD, collects all
 * points lying inside the circular ranges of same-tag groups and registers them
 * as a dimension-0 physical group named after the group tag. Processed groups are
 * marked handled so they are not registered twice.
 *
 * \param geometry Geometry holding the group definitions; \c isInsertedPoints flags
 *                 are cleared as groups are processed.
 * \param ierr     Gmsh error code, updated by the underlying Gmsh calls.
 *
 * \note Side effects: adds physical groups to the Gmsh model, mutates
 *       \c geometry->group[*].isInsertedPoints, and allocates/frees temporary buffers.
 */
void GeometryBuilder::addPhysicalSetsforPoints(Geometry geometry, int& ierr) {
    /* fresh mesh size of group */
    int      i, j;
    int*     dimTag; /* {dim, tag, dim, tag, dim, tag...}*/
    size_t   dimTag_n;
    double*  coord = NULL;
    Vector2* Coords;
    int*     ov;
    size_t   ov_n;
    double   parametricCoord[] = {0};
    size_t   coord_n;
    // get all nodes after gmsh processing
    gmshModelGetEntities(&dimTag, &dimTag_n, 0, &ierr);
    /* initialize coords for all nodes */
    buildmemory(&Coords, dimTag_n / 2);
    int physical_id;
    /* get the coords of all nodes */
    for (i = 0; i < dimTag_n; i++) {
        gmshModelGetValue(0, dimTag[i + 1], parametricCoord, 0, &coord, &coord_n, &ierr);
        Coords[i / 2].x = coord[0];
        Coords[i / 2].y = coord[1];
        i++;
    }
    if (coord != NULL)
        gmshFree(coord);

    int*   dimTags;
    size_t dimTags_n = 0;
    for (i = 0; i < geometry->group_num; i++) {
        if (geometry->group[i].isInsertedPoints) {
            if (geometry->group[i].type == GroupType::FROMCOORD) {
                dimTags_n = 0;
                for (int k = 0; k < geometry->group_num; k++) {
                    if (isSameString(geometry->group[i].tag, geometry->group[k].tag) &&
                        geometry->group[k].type == GroupType::FROMCOORD) {
                        // add the same tag points
                        for (j = 0; j < dimTag_n; j++) {
                            if (MeshUtils::isinCircle(Coords[j / 2], geometry->group[k].range)) {
                                if (!dimTags_n)
                                    dimTags = (int*)malloc(sizeof(int) * 1);
                                else
                                    dimTags = (int*)realloc(dimTags, sizeof(int) * (dimTags_n + 1));
                                dimTags[dimTags_n] = dimTag[j + 1];
                                dimTags_n          = dimTags_n + 1;
                            }
                            j++;
                        }
                        geometry->group[k].isInsertedPoints = false;
                    }
                }

                if (dimTags_n) {
                    physical_id = gmshModelAddPhysicalGroup(0, dimTags, dimTags_n, -1, "", &ierr);
                    // assign the tag to the physcial set
                    // be caution when the old surface is deleted for DFNs
                    gmshModelSetPhysicalName(0, physical_id, geometry->group[i].tag, &ierr);
                }
                geometry->group[i].isInsertedPoints = false;
            }
        }
    }
    if (coord != NULL)
        gmshFree(dimTag);
    if (Coords != NULL) {
        free(Coords);
    }
}
}  // namespace ns_geometry
