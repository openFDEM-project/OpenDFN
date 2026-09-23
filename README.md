# OpenDFN 1.0.0

OpenDFN is an open-source pre-processor for building two-dimensional discrete
fracture network (DFN) geometries and generating conforming meshes. It reads a
plain-text `.dfn` input deck and writes Gmsh `.msh`, editable `.geo`, and
Legacy VTK `.vtk` files.

## Features

- Rectangular, polygonal, circular, elliptical, and tabulated domains.
- Explicit joints, continuous joint sets, discontinuous joint sets, and random
  DFN generation.
- Coordinate-driven real DFN input for image-derived or measured fractures.
- Configurable mesh size, minimum angle, physical groups, and recombination.
- Batch-friendly command-line execution with no GUI required by default.

## Quick Start on Windows

The Windows x64 executable and its required Gmsh DLL are stored in `bin/`.
From the repository root, run:

```powershell
$work = Join-Path $env:TEMP "opendfn-example"
New-Item -ItemType Directory -Force -Path $work | Out-Null
Copy-Item examples\01_deterministic_joint\fig05_single_fracture.dfn $work
bin\opendfn.exe -in (Join-Path $work "fig05_single_fracture.dfn")
```

The command writes generated mesh and geometry files beside the temporary copy
of the input deck. To enable the optional Gmsh GUI:

```powershell
$env:OPENDFN_GMSH_GUI = "1"
```

## Manuscript Examples

The `examples/` directory contains 13 manuscript and validation cases. Each
numbered directory contains one `.dfn` input deck and, when required, its
coordinate file. The cases cover deterministic, continuous, discontinuous,
stochastic, mapped, unfiltered, and minimum-angle comparison configurations.

Run all 13 cases in temporary working directories and validate their MSH, GEO,
VTK, and physical-group outputs:

```powershell
powershell -ExecutionPolicy Bypass -File tests\run_regression.ps1
```

The case directories contain inputs only. Generated files are removed after
validation. Input sizes and SHA-256 checksums for the 13 decks and four
coordinate files are recorded in `examples/MANIFEST_SHA256.json`.

### OpenFDEM angle-sensitivity cases

The `examples/OpenFDEM_angle_validation_cases/` directory contains three
downstream OpenFDEM calculation cases corresponding to the 10°, 15°, and 20°
minimum intersection-angle settings discussed in the Cover Letter. Each case
folder contains an OpenFDEM input file (`.of`), the matching mesh file (`.msh`),
and the corresponding OpenDFN input deck (`.dfn`). The `.of` files import the
same-name `.msh` files. These downstream calculations are supplementary to,
and are not part of, the 13-case OpenDFN regression suite described above.

To run the calculations, install OpenFDEM and follow the instructions in
[`examples/OpenFDEM_angle_validation_cases/README.md`](examples/OpenFDEM_angle_validation_cases/README.md).

## Build from Source

OpenDFN 1.0.0 supports Windows 10/11 with the MSVC C++20 toolchain. Linux and
GCC are not part of the validated release. Configure the build with the
`lib` directory from the official Gmsh Windows64 SDK:

```powershell
cmake -S . -B build -G "Visual Studio 16 2019" -A x64 `
  -DOPENDFN_GMSH_SDK_LIB_DIR=C:/path/to/gmsh-git-Windows64-sdk/lib
cmake --build build --config Release --parallel
```

The generated executable is written to `bin/opendfn.exe`. Available runtime
DLLs are copied to the same directory.

## Validation

Validate the input manifest and rerun every manuscript deck:

```powershell
powershell -ExecutionPolicy Bypass -File tests\check_paper_cases.ps1
powershell -ExecutionPolicy Bypass -File tests\run_regression.ps1
```

The tests use temporary copies so the published inputs remain unchanged.
Stochastic regression cases use `OPENDFN_RANDOM_SEED=0`.

## Documentation

- [Installation](docs/installation.md)
- [Input format](docs/input-format.md)
- [Examples](docs/examples.md)
- [Reproducibility](docs/reproducibility.md)
- [Mesh quality](docs/mesh-quality.md)

## License

OpenDFN first-party source code is distributed under the GNU Lesser General
Public License, version 2.1 or any later version
(`LGPL-2.1-or-later`). No additional commercial, military, or
maintainer-approval restriction applies. Third-party components retain their
upstream licenses.
