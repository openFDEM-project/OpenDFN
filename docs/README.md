# OpenDFN Documentation

This documentation describes OpenDFN 1.0.0, a two-dimensional command-line
pre-processor for generating fracture-conforming meshes.

## Scope

The validated release target is Windows 10/11 with the MSVC C++20 toolchain.
Linux and GCC builds are not part of the validated release. OpenDFN writes
Gmsh MSH2, a GEO wrapper with an OCC XAO sidecar, and Legacy VTK output files.

## Guides

- [Installation](installation.md)
- [Input format](input-format.md)
- [Examples](examples.md)
- [Reproducibility](reproducibility.md)
- [Mesh quality](mesh-quality.md)

The first-party source code is licensed under LGPL-2.1-or-later. 
Third-party dependencies retain their respective upstream licenses.
