/******************************************************************************
 * \file      opendfn_config.h
 * \brief     Build-time configuration macros: version, contact, host and
 *            startup banner strings. /
 *
 * \details   Declarations for build-time configuration macros: version,
 *            contact, host and startup banner strings. /.
 *
 * \author    Xiaofeng Li
 * \email     xfli@whrsm.ac.cn
 * \date      Created:       June 24, 2020
 * \date      Last modified: 2026-07-20 01:06:08
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

/** \brief Human-readable program version string printed at startup. */
#define PRG_VERSION "OpenDFN Version 4.40"
/** \brief Project website URL string. */
#define OPENDFN_WEBSITE "OpenDFN website https://xiaofengli-uoft.github.io/Mainpage/"
/** \brief Maintainer contact email string. */
#define OPENDFN_COTACT "OpenDFN email xfli@whrsm.ac.cn"
/** \brief Copyright notice string. */
#define OPENDFN_COPYRIGHT "Copyright (C) 2017-2026 Xiaofeng Li"                              
/** \brief Build host architecture/OS identifier. */
#define HOST_TYPE "AMD64-Windows"
/** \brief Build host machine name. */
#define HOST_NAME "DREAMER-PC"

/** \brief Multi-line ASCII-art banner printed to the console at program startup. */
#define PRG_HEADER "\
**                                                                                     **\n\
**                                                                                     **\n\
**              ____                   ______ _____  ______ __  __                     **\n\
**             / __ \\                 |  ____|  __ \\|  ____|  \\/  |                    **\n\
**            | |  | |_ __   ___ _ __ | |__  | |  | | |__  | \\  / |                    **\n\
**            | |  | | '_ \\ / _ \\ '_ \\|  __| | |  | |  __| | |\\/| |                    **\n\
**            | |__| | |_) |  __/ | | | |    | |__| | |____| |  | |                    **\n\
**             \\____/| .__/ \\___|_| |_|_|    |_____/|______|_|  |_|                    **\n\
**                   | |                     OpenFree Finite Element                   **\n\
**                   |_|                         and Discrete Element Method Solver    **\n\
**                                                                                     **"
