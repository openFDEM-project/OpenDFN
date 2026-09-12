/******************************************************************************
 * \file      parser_geometry.cpp
 * \brief     Basic classes parser geometry keyword.keywords. for generating
 *            models in OpenDFN
 *
 * \details   Implementation of basic classes parser geometry keyword.keywords.
 *            for generating models in OpenDFN.
 *
 * \author    Xiaofeng Li
 * \email     xfli@whrsm.ac.cn
 * \date      Created:       June 24, 2020
 * \date      Last modified: 2026-07-20 00:59:44
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

#include <string>
#include "geometry/opendfn_geometry_module.h"
#include "mesh/group/geometry_keyword_reader.h"
#include "parser/parser_module.h"
// NOTE (standalone DFN-geometry tool): the Voronoi tessellation and full-mesh
// modules are intentionally NOT included. Their parser branches (voronoi /
// voronoi.hole / voronoi.seed) are removed below because they require the
// solver's Nodal/Element/Edge/DFN structures, which this tool does not build.
using namespace ns_geometry;
using namespace ns_mesh;

namespace ns_io {
/**
 * @brief Parse a "geometry" keyword line and build/modify the model geometry.
 *
 * Dispatches on the sub-keyword embedded in @p keywordfull (full form at +18,
 * short form at +12; some commands match the fully qualified keyword) to drive
 * geometry construction, meshing, and related settings. Supported operations
 * include:
 *  - Primitive shapes with ENTIRE/CUT/REMOVE operation type: square, polygon,
 *    circle, ellipse, table, arc, joint, jset, DFN, points and lines.
 *  - Line import from .msh/.inp, line flipping, and joints from SRC/table
 *  - Voronoi tessellation: create region, add holes, generate seeds and convert
 *    to OpenDFN nodes/elements, followed by MeshModule::meshUpdate.
 *  - Meshing parameters (minangle, minsize, iteration, domain bounds, mesh.size,
 *    recombine) and mesh generation/writing through the Gmsh API
 *    (delaunay / meshadapt / frontal-delaunay algorithms and element order).
 *  - Particle generation/import and geometry node group selection by range.
 *
 * Side effects: reads tokens from general->inputfile, emits runtime/log
 * messages, mutates the geometry object and mesh containers, and may invoke the
 * external Gmsh API. Uses static Voronoi region/mesh state that persists across
 * calls. Unknown keywords or unsupported range methods terminate the process
 * via OPENDFN_EXIT.
 *
 * @param opendfn    The OpenDFN model handle; REGISTER_FULL_CLASS unpacks it
 *                    into module-local class pointers (general, geometry, nodal,
 *                    element, edge, dfn, ...).
 * @param argv        Command-line argument vector forwarded to the Gmsh API.
 * @param keywordfull The full keyword string read from the input file, used for
 *                    sub-keyword dispatch and logging.
 * @return void
 */
void DeckParser::parseGeometry(OpenDFNHandle context, char** argv, char* keywordfull) {
    // Bind the input state and geometry model used by this parser.
    General general = &context->general;
    Geometry geometry = &context->geometry;

    char* keyword;
    char* shortkeyword;
    Real  value;
    keyword = keywordfull + (isSamePrefix(keywordfull, "OpenDFN.geometry.", 17) ? 17 : 14);
    shortkeyword = keyword;
    if (general->debug)
        OpenDFNMessage::Runtime_writelogs(general->logfile, keywordfull);

    if (isSameString(keywordfull, "opendfn.geometry.square") || isSameString(keywordfull, "odfn.geometry.square")) {
        GeometryBuilder::push_geometry_square(general, geometry, OperationType::ENTIRE);
    } else if (isSameString(keyword, "cut.square") || isSameString(shortkeyword, "cut.square")) {
        GeometryBuilder::push_geometry_square(general, geometry, OperationType::CUT);
    } else if (isSameString(keyword, "remove.square") || isSameString(shortkeyword, "remove.square")) {
        GeometryBuilder::push_geometry_square(general, geometry, OperationType::REMOVE);
    } else if (isSameString(keyword, "polygon") || isSameString(shortkeyword, "polygon")) {
        GeometryBuilder::push_geometry_polygon(general, geometry, OperationType::ENTIRE);
    } else if (isSameString(keyword, "insert.points") || isSameString(shortkeyword, "insert.points")) {
        GeometryBuilder::push_geometry_point(general, geometry);
    } else if (isSameString(keyword, "line") || isSameString(shortkeyword, "line")) {
        GeometryBuilder::push_geometry_line(general, geometry);
    } else if (isSameString(keyword, "add.lines.msh") || isSameString(shortkeyword, "add.lines.msh")) {
        Keywords keyword;
        parseStringinLine(general->inputfile, keyword);
        GeometryBuilder::pushGeometryLineGroups(general, geometry, keyword.count, keyword.keywords);
        keyword.clean();
    } else if (isSameString(keyword, "add.lines.inp") || isSameString(shortkeyword, "add.lines.inp")) {
        Keywords keyword;
        parseStringinLine(general->inputfile, keyword);
        GeometryBuilder::addGeometryDFNTags(general, geometry, keyword.count, keyword.keywords);
        keyword.clean();
    } else if (isSameString(keyword, "flip.lines") || isSameString(shortkeyword, "flip.lines")) {
        Keywords keyword;
        parseStringinLine(general->inputfile, keyword);
        GeometryBuilder::addGeometryFlipTags(general, geometry, keyword.count, keyword.keywords);
        keyword.clean();
    } else if (isSameString(keyword, "cut.polygon") || isSameString(shortkeyword, "cut.polygon")) {
        GeometryBuilder::push_geometry_polygon(general, geometry, OperationType::CUT);
    } else if (isSameString(keyword, "remove.polygon") || isSameString(shortkeyword, "remove.polygon")) {
        GeometryBuilder::push_geometry_polygon(general, geometry, OperationType::REMOVE);
    } else if (isSameString(keyword, "table") || isSameString(shortkeyword, "table")) {
        GeometryBuilder::push_geometry_table(general, geometry, OperationType::ENTIRE);
    } else if (isSameString(keyword, "cut.table") || isSameString(shortkeyword, "cut.table")) {
        GeometryBuilder::push_geometry_table(general, geometry, OperationType::CUT);
    } else if (isSameString(keyword, "cut.joint.from.src") || isSameString(shortkeyword, "cut.joint.from.src")) {
        GeometryBuilder::pushJointFromSRC(general, geometry);
    } else if (isSameString(keyword, "line.table") || isSameString(shortkeyword, "line.table")) {
        GeometryBuilder::pushLineFromTable(general, geometry);
    } else if (isSameString(keyword, "remove.table") || isSameString(shortkeyword, "remove.table")) {
        GeometryBuilder::push_geometry_table(general, geometry, OperationType::REMOVE);
    } else if (isSameString(keywordfull, "opendfn.geometry.circle") || isSameString(keywordfull, "odfn.geometry.circle")) {
        GeometryBuilder::push_geometry_circle(general, geometry, OperationType::ENTIRE);
    } else if (isSameString(keyword, "cut.circle") || isSameString(shortkeyword, "cut.circle")) {
        GeometryBuilder::push_geometry_circle(general, geometry, OperationType::CUT);
    } else if (isSameString(keyword, "remove.circle") || isSameString(shortkeyword, "remove.circle")) {
        GeometryBuilder::push_geometry_circle(general, geometry, OperationType::REMOVE);
    } else if (isSameString(keyword, "ellipse") || isSameString(shortkeyword, "ellipse")) {
        GeometryBuilder::push_geometry_ellipse(general, geometry, OperationType::ENTIRE);
    } else if (isSameString(keyword, "cut.ellipse") || isSameString(shortkeyword, "cut.ellipse")) {
        GeometryBuilder::push_geometry_ellipse(general, geometry, OperationType::CUT);
    } else if (isSameString(keyword, "remove.ellipse") || isSameString(shortkeyword, "remove.ellipse")) {
        GeometryBuilder::push_geometry_ellipse(general, geometry, OperationType::REMOVE);
    } else if (isSameString(keyword, "cut.arc") || isSameString(shortkeyword, "cut.arc")) {
        GeometryBuilder::push_geometry_arc(general, geometry);
    } else if (isSameString(keyword, "cut.joint") || isSameString(shortkeyword, "cut.joint")) {
        GeometryBuilder::push_geometry_joint(general, geometry);
    } else if (isSameString(keyword, "cut.jset") || isSameString(shortkeyword, "cut.jset")) {
        GeometryBuilder::push_geometry_jset(general, geometry);
    } else if (isSameString(keyword, "cut.DFN") || isSameString(shortkeyword, "cut.DFN")) {
        GeometryBuilder::push_geometry_DFN(general, geometry);
    } else if (isSamePrefix(keyword, "minangle", 5) || isSamePrefix(shortkeyword, "minangle", 5)) {
        getRealValue(general->inputfile, &geometry->minmuumAngle);
        OpenDFNMessage::RuntimeStringReal("The minmuum angle of geometry is set as ", geometry->minmuumAngle);
    } else if (isSamePrefix(keyword, "minsize", 5) || isSamePrefix(shortkeyword, "minsize", 5)) {
        getRealValue(general->inputfile, &geometry->minmuumSize);
        OpenDFNMessage::RuntimeStringReal("The minmuum size of geometry is set as ", geometry->minmuumSize);
    } else if (isSamePrefix(keyword, "iteration", 5) || isSamePrefix(shortkeyword, "iteration", 5)) {
        getIntValue(general->inputfile, &geometry->iteration);
        OpenDFNMessage::RuntimeStringInt("The iteration of geometry is set as ", geometry->iteration);
    } else if (isSameString(keyword, "domain") || isSameString(shortkeyword, "domain")) {
        Real container[4] = {0.0, 0.0, 0.0, 0.0};
        GeometryKeywordReader::pushSquareKeywords(general->inputfile, container);

        geometry->x0 = container[0];
        geometry->x1 = container[1];
        geometry->y0 = container[2];
        geometry->y1 = container[3];
    } else if (isSameString(keyword, "group") || isSameString(shortkeyword, "group")) {
        parseStringinQuotation(general->inputfile, keyword);
        Real        container[4] = {0.0, 0.0, 0.0, 0.0};
        rangeMethod range;
        GeometryKeywordReader::parseRangeKeywords(general->inputfile, container, range);
        switch (range) {
            case rangeMethod::SQUAREIN:
                GeometryBuilder::getGeoNodeGroupInBoxRange(geometry, keyword, container[0], container[1], container[2],
                                                          container[3]);
                break;
            case rangeMethod::SQUAREOUT:
                GeometryBuilder::getGeoNodeGroupOutBoxRange(geometry, keyword, container[0], container[1], container[2],
                                                              container[3]);
                break;
            case rangeMethod::SQUAREON:
                GeometryBuilder::getGeoNodeGroupOnBoxRange(geometry, keyword, container[0], container[1], container[2],
                                                             container[3]);
                break;
            case rangeMethod::CIRCLEIN:
                GeometryBuilder::getGeoNodeGroupInCircleRange(geometry, keyword, container[0], container[1], container[2]);
                break;
            case rangeMethod::CIRCLEON:
                GeometryBuilder::getGeoNodeGroupOnCircle(geometry, keyword, container[0], container[1], container[2]);
                break;
            case rangeMethod::CIRCLEOUT:
                GeometryBuilder::getGeoNodeGroupOutCircleRange(geometry, keyword, container[0], container[1], container[2]);
                break;
            case rangeMethod::PLANELEFT:
                GeometryBuilder::getGeoNodeGroupOnPlaneLeft(geometry, keyword, container[0], container[1], container[2],
                                                              container[3]);
                break;
            case rangeMethod::PLANEON:
                GeometryBuilder::getGeoNodeGroupOnPlane(geometry, keyword, container[0], container[1], container[2],
                                                         container[3]);
                break;
            case rangeMethod::PLANERIGHT:
                GeometryBuilder::getGeoNodeGroupOnPlaneRight(geometry, keyword, container[0], container[1], container[2],
                                                               container[3]);
                break;
            default:
                OpenDFNMessage::RuntimeInfo("Error: not supported geometry range method.");
                OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
                break;
        }
    } else if (isSameString(keyword, "mesh.size") || isSameString(shortkeyword, "mesh.size")) {
        parseStringinQuotation(general->inputfile, keyword);
        GeometryBuilder::push_mesh_size(geometry, general, keyword);
    } else if (isSameString(keyword, "recombine") || isSameString(shortkeyword, "recombine")) {
        parseStringinQuotation(general->inputfile, keyword);
        GeometryBuilder::push_surface_recombined(geometry, general, keyword);
    } else if (isSameString(keyword, "mesh") || isSameString(shortkeyword, "mesh")) {
        Keywords Parameters;
        /* read a line to avoid \n at the end of the line */
        parseStringinLine(general->inputfile, Parameters);
    
        /* default is delaunay */
        copyChars(keyword, "delaunay");
        for (int i = 0; i < Parameters.count; i++) {
            if (isSamePrefix(Parameters.keywords[i], "delaunay", 3)) /* delaunay */
            {
                copyChars(keyword, "delaunay");
            } else if (isSamePrefix(Parameters.keywords[i], "meshadapt", 3)) /* mesh adapt */
            {
                copyChars(keyword, "meshadapt");
            } else if (isSamePrefix(Parameters.keywords[i], "frontal-delaunay", 3)) /* mesh frontal-delaunay */
            {
                copyChars(keyword, "frontal-delaunay");
            } else if (isSamePrefix(Parameters.keywords[i], "order", 3)) /* order */
            {
                geometry->order = std::stoi(Parameters.keywords[i + 1]);
                i++;
            }
        }
        Parameters.clean();
        GeometryBuilder::generateMesh(context, geometry, argv, keyword, 0);
    } else if (isSameString(keyword, "import.rdfn") || isSameString(shortkeyword, "import.rdfn")) {
        GeometryBuilder::createRealisticDFNs(general, geometry);
    } else if (isSameString(keyword, "mesh.write") || isSameString(shortkeyword, "mesh.write")) {
        Keywords Parameters;
        /* read the whole line: it may carry a "filename" and/or an algorithm */
        parseStringinLine(general->inputfile, Parameters);

        // generateMesh() uses its keyword arg BOTH to pick the algorithm (delaunay/
        // meshadapt/frontal-delaunay) AND, when write_flag==1, as the output
        // file name. mesh.write's real intent is the OUTPUT NAME (e.g.
        // odfn.geometry.mesh.write "test.msh"), so capture the filename token
        // (the one that is not an algorithm/order keyword) and pass THAT to
        // generateMesh. The mesh algorithm keeps generateMesh's built-in default, which
        // is adequate for a geometry pre-processor. If no filename is given we
        // fall back to "<InputFileName>.msh".
        char meshfile[300];
        meshfile[0] = '\0';
        for (int i = 0; i < Parameters.count; i++) {
            if (isSamePrefix(Parameters.keywords[i], "delaunay", 3) ||
                isSamePrefix(Parameters.keywords[i], "meshadapt", 3) ||
                isSamePrefix(Parameters.keywords[i], "frontal-delaunay", 3)) {
                // algorithm token: recognised but not used as the file name
            } else if (isSamePrefix(Parameters.keywords[i], "order", 3)) {
                geometry->order = std::stoi(Parameters.keywords[i + 1]);
                i++;
            } else {
                copyChars(meshfile, Parameters.keywords[i]);  // the output file name
            }
        }
        Parameters.clean();
        if (meshfile[0] == '\0') {
            copyChars(meshfile, general->InputFileName);
            strcat(meshfile, ".msh");
        }
        GeometryBuilder::generateMesh(context, geometry, argv, meshfile, 1);
    } else {
        OpenDFNMessage::RuntimebackStringString("Error: unknown keyword: ", keywordfull);
        OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
    }
}
}  // namespace ns_io
