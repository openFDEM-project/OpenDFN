# Installation

## Requirements

- Windows 10 or Windows 11
- Visual Studio 2019 or newer with the MSVC C++20 toolchain
- CMake 3.10 or newer
- The official Gmsh Windows64 SDK (`gmsh.dll.lib` and `gmsh-5.0.dll`)

The validated release uses the official Gmsh Windows64 SDK. Set
`OPENDFN_GMSH_SDK_LIB_DIR` to the SDK directory containing `gmsh.dll.lib`
and `gmsh-5.0.dll`. OpenDFN uses the C++ standard-library random-number facilities
and therefore does not require GSL. Linux and GCC are not supported by this
release.

## Build

From the repository root, configure and build a 64-bit Release target:

```powershell
cmake -S . -B build -G "Visual Studio 16 2019" -A x64 `
  -DOPENDFN_GMSH_SDK_LIB_DIR=C:/path/to/gmsh-git-Windows64-sdk/lib
cmake --build build --config Release --parallel
```

The executable is written to `bin/opendfn.exe`. When the dependency bundle
contains runtime DLLs, CMake copies them next to the executable in `bin/`.

To use another Gmsh SDK:

```powershell
cmake -S . -B build `
  -DOPENDFN_GMSH_SDK_LIB_DIR=C:/path/to/gmsh-git-Windows64-sdk/lib
cmake --build build --config Release --parallel
```

## Regression checks

After building, run:

```powershell
powershell -ExecutionPolicy Bypass -File tests\run_regression.ps1
```

The script runs the standard example decks and checks process status, MSH2
sections, physical groups, and generated file sizes.
