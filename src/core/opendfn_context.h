#ifndef OPENDFN_CONTEXT_H
#define OPENDFN_CONTEXT_H

#include "common/opendfn_general.h"
#include "geometry/opendfn_geometry.h"

struct OpenDFNContext {
    General_structure general;
    Geometry_structure geometry;
};

using OpenDFNHandle = OpenDFNContext*;

#endif
