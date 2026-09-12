# OpenDFN

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

The repository includes a prebuilt Windows x64 release in:

```text
release_windows_x64/windows-x64/bin/opendfn.exe
```

The DLLs required by the executable are stored beside it. From the repository
root, run an example with:

```powershell
release_windows_x64\windows-x64\bin\opendfn.exe `
  -in examples\basic_geometry\basic_geometry.dfn
```

The command writes `basic_geometry.msh`, `basic_geometry.geo`, and
`basic_geometry.vtk` beside the input deck. To enable the optional Gmsh GUI:

```powershell
$env:OPENDFN_GMSH_GUI = "1"
```

## Examples

The `examples/` directory contains small, self-contained input decks:

| Directory | Description |
|---|---|
| `single_joint` | One deterministic joint |
| `multiple_joints` | Several explicit joints with different positions and directions |
| `continuous_joint_sets` | Two persistent joint sets |
| `discontinuous_joint_sets` | Two joint sets with finite traces and gaps |
| `arbitrary_dfn` | DFN with uniformly sampled dips |
| `input_real_dfn` | Coordinate text imported as an image-derived DFN |
| `basic_geometry` | A rectangular domain with explicit joints |
| `random_dfn` | Two statistically controlled random joint sets |
| `realistic_dfn` | Coordinate-driven mapped fractures |

Each case includes a `generate_formats.ps1` helper. To run all standard cases
with the prebuilt executable, use:

```powershell
powershell -ExecutionPolicy Bypass -File examples\generate_all_formats.ps1
```

The reproducibility inputs used for the manuscript figures are collected under
`examples/paper_cases/`. Generated meshes, logs, and figures are intentionally
not part of the source tree.

## Build from Source

OpenDFN currently targets C++20 and the Windows/MSVC toolchain. The bundled
`release_windows_x64/windows-x64` directory contains the prebuilt Gmsh,
Triangle, and GSL libraries used by the current Windows build. To configure and
build with Visual Studio 2019:

```powershell
cmake -S . -B build -G "Visual Studio 16 2019" -A x64
cmake --build build --config Release --parallel
```

To use a locally built dependency bundle, pass its root directory:

```powershell
cmake -S . -B build `
  -DOPENDFN_THIRD_PARTY_ROOT=C:/path/to/windows-x64
```

The generated executable is written to `src/bin/Release/opendfn.exe`.

## Regression Tests

After building, run:

```powershell
powershell -ExecutionPolicy Bypass -File tests\run_regression.ps1
```

The test script checks process exit codes, mesh sections, file sizes, and
expected physical groups for the standard examples.

## Repository Layout

```text
OpenDFN/
├─ CMakeLists.txt
├─ README.md
├─ LICENSE
├─ NOTICE.md
├─ src/                         OpenDFN source code and module headers
├─ include/                     Public-header layout note
├─ examples/                    Standard examples and paper-case inputs
├─ tests/                       Regression scripts
├─ docs/                        Project documentation
└─ release_windows_x64/         Windows x64 executable and bundled libraries
```

## License

The OpenDFN source headers state `LGPL-2.1-or-later` together with an
additional project notice concerning commercial or military use. See
`LICENSE` and `NOTICE.md`, and confirm the final redistribution policy with the
project maintainer before publishing a public release. Third-party components
retain their upstream licenses.
