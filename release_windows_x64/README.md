# Windows x64 release bundle

The `windows-x64` bundle contains the prebuilt OpenDFN executable, MSVC
import/static libraries and runtime DLLs used by the current Windows build:

- Gmsh 4.10 (`gmsh.lib`, `gmsh-4.10.dll`)
- GNU Scientific Library (`gsl.lib`, `gsl.dll`, `gslcblas.lib`, `gslcblas.dll`)
- Triangle (`triangle.lib`)

Run `bin\opendfn.exe -in <path-to-deck.dfn>` from a terminal. The executable
and its DLLs are kept together in `bin/`.

These files are optional. Replace this directory with locally built
dependencies by passing `-DOPENDFN_THIRD_PARTY_ROOT=<path>` to CMake. Review
the upstream licenses for each dependency before redistributing binaries.
