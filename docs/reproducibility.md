# Reproducibility

## Release identity

This documentation describes OpenDFN 1.0.0. The source release should be
archived from the immutable `v1.0.0` tag together with the official Gmsh SDK
dependency record, manuscript inputs, and SHA-256 checksums.

## Manuscript cases

The reproducibility package contains 13 numbered case directories under
`examples/`. Each case directory directly contains one input deck and
coordinate data when required. Generated outputs are deliberately excluded.
`examples/MANIFEST_SHA256.json` records the expected file sizes and SHA-256
hashes of the 13 input decks and four coordinate files.

Run a deck from the repository root. Copy the input deck and any required
coordinate files to a temporary working directory first; running an original
deck directly writes generated outputs beside it under `examples/`. For
example:

```powershell
$work = Join-Path $env:TEMP "opendfn-stochastic"
New-Item -ItemType Directory -Force -Path $work | Out-Null
Copy-Item examples\05_stochastic_dfn\Fig08a_high_density_stochastic_DFN.dfn $work
bin\opendfn.exe -in (Join-Path $work "Fig08a_high_density_stochastic_DFN.dfn")
```

The program writes MSH2, GEO, VTK, and, for OCC exports, a
`.geo_unrolled.xao` sidecar beside the working copy of the deck. The `.geo`
file is a small Gmsh wrapper that merges this sidecar.

## Environment record

Record the following for each published result:

- OpenDFN version and source SHA-256
- Windows version and MSVC version
- CMake version
- Gmsh version and the SHA-256 values of runtime DLLs
- `OPENDFN_RANDOM_SEED` value for stochastic decks
- Input-deck path relative to the release root
- Output-file SHA-256 values
- Mesh-quality summary and any quality-gate warnings

Use seed `0` when reproducing the stochastic case. No absolute workstation path
should be included in a public manifest.

Before packaging a release, run:

```powershell
powershell -ExecutionPolicy Bypass -File tests\check_paper_cases.ps1
powershell -ExecutionPolicy Bypass -File tests\run_regression.ps1
```

Both scripts use temporary working directories and leave the source inputs
unchanged.
