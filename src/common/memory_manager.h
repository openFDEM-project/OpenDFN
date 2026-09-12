#ifndef OPENDFN_MEMORY_MANAGER_H
#define OPENDFN_MEMORY_MANAGER_H

#include "common/opendfn_struct.h"

struct geo_node_type;
struct geo_line_type;
struct geo_curve_type;
struct geo_surface_type;
struct geo_group_type;
struct geo_EmbededlineGroups_type;
struct Vector2;

namespace ns_common {

void malloc1Darray(Int size, int** array, int value = 0);

void buildmemory(Real** target, Int newSize, Int oldSize = 0, Real value = 0.0);
void buildmemory(int** target, Int newSize, Int oldSize = 0, int value = 0);
void buildmemory(char** target, Int newSize, Int oldSize = 0);
void buildmemory(char*** target, Int newSize, Int oldSize = 0);
void buildmemory(Vector2** target, Int newSize, Int oldSize = 0);

void buildmemory(geo_node_type** target, Int newSize, Int oldSize = 0);
void buildmemory(geo_line_type** target, Int newSize, Int oldSize = 0);
void buildmemory(geo_curve_type** target, Int newSize, Int oldSize = 0);
void buildmemory(geo_surface_type** target, Int newSize, Int oldSize = 0);
void buildmemory(geo_group_type** target, Int newSize, Int oldSize = 0);
void buildmemory(geo_EmbededlineGroups_type** target, Int newSize, Int oldSize = 0);

}  // namespace ns_common

#endif
