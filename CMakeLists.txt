cmake_minimum_required(VERSION 3.10)

project(OpenDFN LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/src/bin)

# Prebuilt Windows dependencies are kept outside the source tree.  Override
# this path when using a locally built Gmsh/Triangle/GSL toolchain.
set(OPENDFN_THIRD_PARTY_ROOT "${PROJECT_SOURCE_DIR}/release_windows_x64/windows-x64" CACHE PATH
    "Directory containing the bundled Windows executable, libraries and DLLs")

# GSL is built as a DLL import library (matches the parent project).
add_definitions(-DGSL_DLL)
add_compile_definitions(GSL_DLL)

if(MSVC)
    add_compile_options(/EHsc)
    add_compile_options(/bigobj)
    add_definitions(-w)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

add_subdirectory(src)
