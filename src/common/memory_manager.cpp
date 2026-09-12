#include "common/memory_manager.h"
#include "common/opendfn_vector2.h"

#include <cstdlib>
#include <new>

#include "geometry/opendfn_geometry.h"

namespace ns_common {
namespace {

template <typename T>
void growAndInitialize(T** target, Int newSize, Int oldSize) {
    if (newSize <= 0) {
        std::free(*target);
        *target = nullptr;
        return;
    }

    void* resized = oldSize == 0 ? std::malloc(sizeof(T) * newSize) : std::realloc(*target, sizeof(T) * newSize);
    if (resized == nullptr)
        throw std::bad_alloc();

    auto* values = static_cast<T*>(resized);
    for (Int i = oldSize; i < newSize; ++i)
        values[i] = T{};
    *target = values;
}

template <typename T>
void growAndFill(T** target, Int newSize, Int oldSize, T value) {
    if (newSize <= 0) {
        std::free(*target);
        *target = nullptr;
        return;
    }

    void* resized = oldSize == 0 ? std::malloc(sizeof(T) * newSize) : std::realloc(*target, sizeof(T) * newSize);
    if (resized == nullptr)
        throw std::bad_alloc();

    auto* values = static_cast<T*>(resized);
    for (Int i = oldSize; i < newSize; ++i)
        values[i] = value;
    *target = values;
}

}  // namespace

void malloc1Darray(Int size, int** array, int value) {
    *array = static_cast<int*>(std::malloc(sizeof(int) * size));
    if (*array == nullptr)
        throw std::bad_alloc();
    for (Int i = 0; i < size; ++i)
        (*array)[i] = value;
}

void buildmemory(Real** target, Int newSize, Int oldSize, Real value) {
    growAndFill(target, newSize, oldSize, value);
}

void buildmemory(int** target, Int newSize, Int oldSize, int value) {
    growAndFill(target, newSize, oldSize, value);
}

void buildmemory(char** target, Int newSize, Int oldSize) {
    growAndFill(target, newSize, oldSize, '\0');
}

void buildmemory(char*** target, Int newSize, Int oldSize) {
    growAndFill(target, newSize, oldSize, static_cast<char*>(nullptr));
}

void buildmemory(Vector2** target, Int newSize, Int oldSize) {
    growAndInitialize(target, newSize, oldSize);
}

void buildmemory(geo_node_type** target, Int newSize, Int oldSize) {
    growAndInitialize(target, newSize, oldSize);
}

void buildmemory(geo_line_type** target, Int newSize, Int oldSize) {
    growAndInitialize(target, newSize, oldSize);
}

void buildmemory(geo_curve_type** target, Int newSize, Int oldSize) {
    growAndInitialize(target, newSize, oldSize);
}

void buildmemory(geo_surface_type** target, Int newSize, Int oldSize) {
    growAndInitialize(target, newSize, oldSize);
}

void buildmemory(geo_group_type** target, Int newSize, Int oldSize) {
    growAndInitialize(target, newSize, oldSize);
}

void buildmemory(geo_EmbededlineGroups_type** target, Int newSize, Int oldSize) {
    growAndInitialize(target, newSize, oldSize);
}

}  // namespace ns_common
