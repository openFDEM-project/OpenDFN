/******************************************************************************
 * \file      geometry_keyword_reader.h  (DFN-Geometry standalone shim)
 * \brief     Minimal GeometryKeywordReader for the standalone geometry pre-processor.
 *
 * \details   The original mesh/group/geometry_keyword_reader.h declares a large class with
 *            dozens of grouping routines whose bodies live in get_*_groups.cpp
 *            and dereference Element/Nodal/Edge/DFN. The geometry path only ever
 *            calls the keyword-parsing helpers (pushSquareKeywords,
 *            pushCircleGeometryKeywords, pushPlaneKeywords,
 *            pushEllipseGeometryKeywords, pushArcGeometryKeywords,
 *            parseRangeKeywords) which are all defined in the GeometryKeywordParser base
 *            class (mesh/group/geometry_keyword_parser.*). Inheriting GeometryKeywordParser re-exposes
 *            them under the GeometryKeywordReader:: name the callers use, without pulling
 *            in any of the heavy group source files.
 ******************************************************************************/

#ifndef _OPENDFN_DFN_GEOMETRY_GROUP_MODULE_SHIM_
#define _OPENDFN_DFN_GEOMETRY_GROUP_MODULE_SHIM_

#include <mesh/group/geometry_keyword_parser.h>

using namespace ns_common;

namespace ns_mesh {
/**
 * \brief Standalone stand-in for the full GeometryKeywordReader.
 *
 * Provides exactly the static keyword parsers the geometry pre-processor needs,
 * inherited verbatim from GeometryKeywordParser. Holds no state.
 */
class GeometryKeywordReader : public GeometryKeywordParser {
public:
    GeometryKeywordReader() {}
    ~GeometryKeywordReader() {}
};
}  // namespace ns_mesh

#endif  // !_OPENDFN_DFN_GEOMETRY_GROUP_MODULE_SHIM_
