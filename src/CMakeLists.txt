# ---------------------------------------------------------------------------
# Standalone jset/joint/DFN geometry pre-processor.
# Compiles ONLY the modules on the geometry -> mesh path:
#   common/, geometry/, mesh/group/geometry_keyword_parser.cpp,
#   iohelper/parser/parser_geometry.cpp, and the driver main.cpp.
# ---------------------------------------------------------------------------

set(SRC_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

# --- header search paths (mirror the parent project's layout) --------------
include_directories(${SRC_ROOT})
include_directories(${SRC_ROOT}/common)
include_directories(${SRC_ROOT}/external)
include_directories(${SRC_ROOT}/geometry)
include_directories(${SRC_ROOT}/mesh)
include_directories(${SRC_ROOT}/iohelper)
include_directories(${SRC_ROOT}/core)

# --- source selection ------------------------------------------------------
set(COMMON_SRC
    ${SRC_ROOT}/common/logger.cpp
    ${SRC_ROOT}/common/memory_manager.cpp
    ${SRC_ROOT}/common/opendfn_common.cpp
    ${SRC_ROOT}/common/opendfn_error.cpp
    ${SRC_ROOT}/common/opendfn_math.cpp)

file(GLOB GEOMETRY_SRC ${SRC_ROOT}/geometry/*.cpp)

set(MESH_SRC ${SRC_ROOT}/mesh/group/geometry_keyword_parser.cpp)
set(PARSER_SRC
    ${SRC_ROOT}/iohelper/parser/parser_geometry.cpp
    ${SRC_ROOT}/iohelper/parser/parser.cpp)

add_executable(opendfn
    ${SRC_ROOT}/main.cpp
    ${COMMON_SRC}
    ${GEOMETRY_SRC}
    ${MESH_SRC}
    ${PARSER_SRC})

# --- third-party libraries --------------------------------------------------
set(OPENDFN_THIRD_PARTY_LIB_DIR "${OPENDFN_THIRD_PARTY_ROOT}/lib")
set(OPENDFN_THIRD_PARTY_BIN_DIR "${OPENDFN_THIRD_PARTY_ROOT}/bin")
target_link_libraries(opendfn
    "${OPENDFN_THIRD_PARTY_LIB_DIR}/gmsh.lib"
    "${OPENDFN_THIRD_PARTY_LIB_DIR}/triangle.lib"
    "${OPENDFN_THIRD_PARTY_LIB_DIR}/gsl.lib"
    "${OPENDFN_THIRD_PARTY_LIB_DIR}/gslcblas.lib")

if(WIN32)
    foreach(runtime_file gmsh-4.10.dll gsl.dll gslcblas.dll)
        if(EXISTS "${OPENDFN_THIRD_PARTY_BIN_DIR}/${runtime_file}")
            add_custom_command(TARGET opendfn POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${OPENDFN_THIRD_PARTY_BIN_DIR}/${runtime_file}"
                    "$<TARGET_FILE_DIR:opendfn>/${runtime_file}")
        endif()
    endforeach()
endif()

if(MSVC)
    target_link_options(opendfn PRIVATE /NODEFAULTLIB:LIBCMT)
endif()
