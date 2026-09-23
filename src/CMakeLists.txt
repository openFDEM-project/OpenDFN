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

# --- Gmsh SDK --------------------------------------------------------------
set(OPENDFN_GMSH_IMPORT_LIB "${OPENDFN_GMSH_SDK_LIB_DIR}/gmsh.dll.lib")
set(OPENDFN_GMSH_RUNTIME_DLL "${OPENDFN_GMSH_SDK_LIB_DIR}/gmsh-5.0.dll")
if(NOT EXISTS "${OPENDFN_GMSH_IMPORT_LIB}")
    message(FATAL_ERROR
        "OpenDFN requires the Gmsh SDK import library at "
        "${OPENDFN_GMSH_IMPORT_LIB}")
endif()
if(NOT EXISTS "${OPENDFN_GMSH_RUNTIME_DLL}")
    message(FATAL_ERROR
        "OpenDFN requires the matching Gmsh SDK runtime at "
        "${OPENDFN_GMSH_RUNTIME_DLL}")
endif()

target_link_libraries(opendfn PRIVATE
    "${OPENDFN_GMSH_IMPORT_LIB}")

if(WIN32)
    add_custom_command(TARGET opendfn POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${OPENDFN_GMSH_RUNTIME_DLL}"
            "$<TARGET_FILE_DIR:opendfn>/gmsh-5.0.dll")
endif()

if(MSVC)
    target_link_options(opendfn PRIVATE /NODEFAULTLIB:LIBCMT)
endif()
