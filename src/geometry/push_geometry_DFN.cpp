/******************************************************************************
 * \file      push_geometry_DFN.cpp
 * \brief     Basic classes parser mesh fron selfdeveloped format parameters.
 *
 * \details   Implementation of basic classes parser mesh fron selfdeveloped
 *            format parameters.
 *
 * \author    Xiaofeng Li
 * \email     xfli@whrsm.ac.cn
 * \date      Created:       June 24, 2020
 * \date      Last modified: 2026-07-20 00:04:15
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

#include "common/opendfn_math.h"
#include "external/gsl/gsl_randist.h"
#include "external/gsl/gsl_rng.h"
#include "geometry/opendfn_geometry_module.h"

#define _METHOD_DFN_COUNT 0
#define _METHOD_DFN_P21 1
#define _METHOD_DFN_P10 2

namespace ns_geometry {
/**
 * @brief Parse a Discrete Fracture Network (DFN) joint set from the input file and
 *        stochastically populate the geometry with embedded joint lines.
 *
 * Reads the joint-set name, the embedding surface tag, and the DFN parameters
 * (dip and length distributions, target count, P21 or P10 intensity, and the
 * generation method) from @p general->inputfile. Random joint centres are sampled
 * uniformly inside the geometry bounding box, while joint dip and length are drawn
 * from the configured distributions (constant, uniform, Gaussian, or Fisher via GSL).
 * Each candidate joint is validated for intersection quality before its two end
 * nodes and connecting line are appended to @p geometry. Generation continues until
 * the stopping criterion of the selected method is met:
 *   - _METHOD_DFN_COUNT: a target number of joints is reached.
 *   - _METHOD_DFN_P21:   accumulated length-per-area intensity reaches @c p21.
 *   - _METHOD_DFN_P10:   accumulated line intensity along a scanline reaches @c p10.
 *
 * Side effects: mutates @p geometry by allocating and appending nodes and lines,
 * registers the added lines with the surface boolean-cut groups, node groups, and
 * line groups; advances the input-file parse cursor; may terminate the process via
 * OPENDFN_EXIT on an undefined domain or missing dip/length specification.
 * Allocates and frees a GSL random-number generator internally.
 *
 * @param general  Global context providing the input-file stream to parse.
 * @param geometry Geometry container that is mutated with the generated joints.
 * @return void
 */
void GeometryBuilder::push_geometry_DFN(General general, Geometry geometry) {
    random_method dip_method    = random_method::RANDOM_NULL;
    random_method length_method = random_method::RANDOM_NULL;
    /* DFN method -1 default, 0 count, 1 p21, 2 p10*/
    Int  method = -1;
    Int  count, count_now = 0;
    Real p21, p21_now, p10, p10_now, x, y, area, L1, u;
    p21_now = 0.0;
    p10_now = 0.0;

    Real dip[2];
    Real length[2];

    Real                l_dip, l_length;
    const gsl_rng_type* T;
    gsl_rng*            r;
    gsl_rng_env_setup();
    T = gsl_rng_default;
    r = gsl_rng_alloc(T);

    /* get the name of the jset */
    char newToolTag[MAXARGC], ObjectTag[MAXARGC];
    parseStringinQuotation(general->inputfile, newToolTag);
    parseStringinQuotation(general->inputfile, ObjectTag);

    area = (geometry->x1 - geometry->x0) * (geometry->y1 - geometry->y0);
    L1   = (geometry->x1 - geometry->x0);

    // get the DFN set parameters
    parseDFNs(general, dip, length, count, method, p21, p10, dip_method, length_method);

    if ((geometry->x0 == geometry->x1) || (geometry->y0 == geometry->y1)) {
        OpenDFNMessage::RuntimeInfo("Error: undefined geometry domian.");
        OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
    } else {
        switch (method) {
            case _METHOD_DFN_COUNT: {
                do {
                    /* get the random x y*/
                    /* x xoord */
                    // uniform random value between 0-1
                    u = gsl_rng_uniform(r);
                    x = geometry->x0 + (geometry->x1 - geometry->x0) * u;
                    /* y coord */
                    u = gsl_rng_uniform(r);
                    y = geometry->y0 + (geometry->y1 - geometry->y0) * u;

                    /* get the dip */
                    if (dip_method == random_method::RANDOM_NULL) {
                        OpenDFNMessage::RuntimeInfo("Error: no dips for joints.");
                        OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
                    } else if (dip_method == random_method::RANDOM_CONSTANT) {
                        l_dip = dip[0];
                    } else if (dip_method == random_method::RANDOM_UNIFORM) {
                        u     = gsl_rng_uniform(r);
                        l_dip = dip[0] + (dip[1] - dip[0]) * u;
                    } else if (dip_method == random_method::RANDOM_GAUSSE) {
                        do {
                            // with the deviation of dip[1]
                            u = gsl_ran_gaussian(r, dip[1]);
                        } while ((dip[0] + u) > 180 || (dip[0] + u) < 0);
                        l_dip = dip[0] + u;
                    }

                    /* get the length */
                    if (length_method == random_method::RANDOM_NULL) {
                        OpenDFNMessage::RuntimeInfo("Error: no length for joints.");
                        OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
                    } else if (length_method == random_method::RANDOM_CONSTANT) {
                        l_length = length[0];
                    } else if (length_method == random_method::RANDOM_GAUSSE) {
                        do {
                            u = gsl_ran_gaussian(r, length[1]);
                            // not to small length
                        } while ((length[0] + u) < 0.05 * length[0]);

                        l_length = length[0] + u;
                    } else if (length_method == random_method::RANDOM_FISHER) {
                        do {
                            u = gsl_ran_fdist(r, length[0], length[1]);
                            // not too small length
                        } while ((u) < 0.05 * length[0]);
                        l_length = u;
                    }
                    Real container[4];
                    /* add geometry geometry->node */
                    for (int i = 0; i < 2; i++) {
                        container[2 * i]     = x - pow(-1, i) * 0.5 * l_length * cos(l_dip / 180.0 * OpenDFN_PI);
                        container[2 * i + 1] = y - pow(-1, i) * 0.5 * l_length * sin(l_dip / 180.0 * OpenDFN_PI);
                    }

                    if (!GeometryRules::isLinesWellIntersection(geometry, container, false)) {
                        OpenDFNMessage::RuntimeStringStringString("Warning: Joint ", newToolTag,
                                                                   "is not added due to bad angle or line size.");
                        continue;
                    }
                    count_now++;
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
                    // copy the tag
                    copyChars(geometry->line[geometry->line_num].tag, newToolTag);

                    geometry->line[geometry->line_num].node_1 = geometry->node_num - 2;
                    geometry->line[geometry->line_num].node_2 = geometry->node_num - 1;

                    geometry->line[geometry->line_num].iselement = false;
                    geometry->line[geometry->line_num].isembeded = true;
                    // add to bool groups for cut
                    addLinetoSufaceBoolGroups(geometry, ObjectTag, geometry->line_num);
                    // update the line
                    geometry->line_num++;
                } while (count_now <= count);
            } break;
            case _METHOD_DFN_P21: {
                do {
                    /* get the random x y*/
                    /* x xoord */
                    u = gsl_rng_uniform(r);
                    x = geometry->x0 + (geometry->x1 - geometry->x0) * u;
                    /* y coord */
                    u = gsl_rng_uniform(r);
                    y = geometry->y0 + (geometry->y1 - geometry->y0) * u;

                    /* get the dip */
                    if (dip_method == random_method::RANDOM_NULL) {
                        OpenDFNMessage::RuntimeInfo("Error: no dips for joints.");
                        OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
                    } else if (dip_method == random_method::RANDOM_CONSTANT) {
                        l_dip = dip[0];
                    } else if (dip_method == random_method::RANDOM_UNIFORM) {
                        u     = gsl_rng_uniform(r);
                        l_dip = dip[0] + (dip[1] - dip[0]) * u;
                    } else if (dip_method == random_method::RANDOM_GAUSSE) {
                        do {
                            u = gsl_ran_gaussian(r, dip[1]);
                        } while ((dip[0] + u) > 180 || (dip[0] + u) < 0);

                        l_dip = dip[0] + u;
                    }

                    /* get the length */
                    if (length_method == random_method::RANDOM_NULL) {
                        OpenDFNMessage::RuntimeInfo("Error: no length for joints.");
                        OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
                    } else if (length_method == random_method::RANDOM_CONSTANT) {
                        l_length = length[0];
                    } else if (length_method == random_method::RANDOM_GAUSSE) {
                        do {
                            u = gsl_ran_gaussian(r, length[1]);
                        } while ((length[0] + u) < 0.05 * length[0]);

                        l_length = length[0] + u;
                    } else if (length_method == random_method::RANDOM_FISHER) {
                        do {
                            u = gsl_ran_fdist(r, length[0], length[1]);
                        } while ((u) < 0.05 * length[0]);
                        l_length = u;
                    }

                    /* add geometry geometry->node */
                    Real container[4];
                    for (int i = 0; i < 2; i++) {
                        container[2 * i]     = x - pow(-1, i) * 0.5 * l_length * cos(l_dip / 180.0 * OpenDFN_PI);
                        container[2 * i + 1] = y - pow(-1, i) * 0.5 * l_length * sin(l_dip / 180.0 * OpenDFN_PI);
                    }

                    if (!GeometryRules::isLinesWellIntersection(geometry, container, false)) {
                        OpenDFNMessage::RuntimeStringStringString("Warning: Joint ", newToolTag,
                                                                   "is not added due to bad angle or line size.");
                        continue;
                    }
                    p21_now = p21_now + l_length / area;
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
                    copyChars(geometry->line[geometry->line_num].tag, newToolTag);
                    geometry->line[geometry->line_num].node_1    = geometry->node_num - 2;
                    geometry->line[geometry->line_num].node_2    = geometry->node_num - 1;
                    geometry->line[geometry->line_num].iselement = false;
                    geometry->line[geometry->line_num].isembeded = true;
                    // add to bool groups for cut
                    addLinetoSufaceBoolGroups(geometry, ObjectTag, geometry->line_num);
                    geometry->line_num++;
                } while (p21_now <= p21);
            } break;
            case _METHOD_DFN_P10: {
                do {
                    /* get the random x y*/
                    /* x xoord */
                    u = gsl_rng_uniform(r);
                    x = geometry->x0 + (geometry->x1 - geometry->x0) * u;
                    /* y coord */
                    u = gsl_rng_uniform(r);
                    y = geometry->y0 + (geometry->y1 - geometry->y0) * u;

                    /* get the dip */
                    if (dip_method == random_method::RANDOM_NULL) {
                        OpenDFNMessage::RuntimeInfo("Error: no dips for joints.");
                        OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
                    } else if (dip_method == random_method::RANDOM_CONSTANT) {
                        l_dip = dip[0];
                    } else if (dip_method == random_method::RANDOM_UNIFORM) {
                        u     = gsl_rng_uniform(r);
                        l_dip = dip[0] + (dip[1] - dip[0]) * u;
                    } else if (dip_method == random_method::RANDOM_GAUSSE) {
                        do {
                            u = gsl_ran_gaussian(r, dip[1]);
                        } while ((dip[0] + u) > 180 || (dip[0] + u) < 0);

                        l_dip = dip[0] + u;
                    }

                    /* get the length */
                    if (length_method == random_method::RANDOM_NULL) {
                        OpenDFNMessage::RuntimeInfo("Error: no length for joints.");
                        OPENDFN_EXIT(OPENDFN_EXIT_FAILURE);
                    } else if (length_method == random_method::RANDOM_CONSTANT) {
                        l_length = length[0];
                    } else if (length_method == random_method::RANDOM_GAUSSE) {
                        do {
                            u = gsl_ran_gaussian(r, length[1]);
                        } while ((length[0] + u) < 0.05 * length[0]);

                        l_length = length[0] + u;
                    } else if (length_method == random_method::RANDOM_FISHER) {
                        do {
                            u = gsl_ran_fdist(r, length[0], length[1]);
                        } while ((u) < 0.05 * length[0]); /* to short will induce mesh problem */
                        l_length = u;
                    }

                    /* add geometry geometry->node */
                    Real container[4];
                    for (int i = 0; i < 2; i++) {
                        container[2 * i]     = x - pow(-1, i) * 0.5 * l_length * cos(l_dip / 180.0 * OpenDFN_PI);
                        container[2 * i + 1] = y - pow(-1, i) * 0.5 * l_length * sin(l_dip / 180.0 * OpenDFN_PI);
                    }
                    if (!GeometryRules::isLinesWellIntersection(geometry, container, false)) {
                        OpenDFNMessage::RuntimeStringStringString("Warning: Joint ", newToolTag,
                                                                   "is not added due to bad angle or line size.");
                        continue;
                    }

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

                    copyChars(geometry->line[geometry->line_num].tag, newToolTag);

                    geometry->line[geometry->line_num].node_1 = geometry->node_num - 2;
                    geometry->line[geometry->line_num].node_2 = geometry->node_num - 1;

                    geometry->line[geometry->line_num].iselement = false;
                    geometry->line[geometry->line_num].isembeded = true;
                    // add to bool groups for cut
                    addLinetoSufaceBoolGroups(geometry, ObjectTag, geometry->line_num);
                    geometry->line_num++;

                    if ((0.5 * (geometry->y0 + geometry->y1) >= geometry->node[geometry->node_num - 2].y) &&
                            (0.5 * (geometry->y0 + geometry->y1) <= geometry->node[geometry->node_num - 1].y) ||
                        (0.5 * (geometry->y0 + geometry->y1) >= geometry->node[geometry->node_num - 1].y) &&
                            (0.5 * (geometry->y0 + geometry->y1) <= geometry->node[geometry->node_num - 2].y)) {
                        p10_now = p10_now + 1 / L1;
                    }
                } while (p10_now <= p10);
            } break;
            default: break;
        }
    }
    gsl_rng_free(r);
    // add to node groups
    addtoLinetoNodeGroups(geometry, newToolTag);
    // build for line groups
    addtoLineGroups(geometry, newToolTag);
}
}  // namespace ns_geometry