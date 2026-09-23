cmake_minimum_required(VERSION 3.14)

project(OpenDFN VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/bin)

# The validated Windows build uses the official Gmsh SDK import library and
# shared runtime. Override this path when using another Gmsh SDK.
set(OPENDFN_THIRD_PARTY_ROOT "${PROJECT_SOURCE_DIR}/release_windows_x64/windows-x64" CACHE PATH
    "Compatibility root for the bundled Gmsh SDK files")
set(OPENDFN_GMSH_SDK_LIB_DIR "${OPENDFN_THIRD_PARTY_ROOT}/lib" CACHE PATH
    "Directory containing gmsh.dll.lib and gmsh-5.0.dll")

if(MSVC)
    add_compile_options(/EHsc)
    add_compile_options(/bigobj)
    add_definitions(-w)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

add_subdirectory(src)
