# Examples

The `examples/` directory contains the manuscript and validation cases. Each
numbered case directory is flat and contains one input deck and any required
coordinate file. Generated meshes, geometry files, logs, and statistics are
not stored in the example directories.

| ID | Input deck | Purpose |
| --- | --- | --- |
| `01` | `01_deterministic_joint/fig05_single_fracture.dfn` | One deterministic fracture |
| `02` | `02_intersecting_continuous_joint_sets/fig06_double_extended_domain.dfn` | Two intersecting continuous joint sets |
| `03` | `03_single_discontinuous_joint_set/fig07a_nonpersistent_single_set.dfn` | One discontinuous joint set |
| `04` | `04_intersecting_discontinuous_joint_sets/fig07b_nonpersistent_double_set.dfn` | Two intersecting discontinuous joint sets |
| `05` | `05_stochastic_dfn/Fig08a_high_density_stochastic_DFN.dfn` | Screened stochastic DFN |
| `06` | `06_image_derived_dfn_area_1/runtime.dfn` | Screened mapped DFN, area 1 |
| `07` | `07_image_derived_dfn_area_2/runtime.dfn` | Screened mapped DFN, area 2 |
| `08` | `08_Stochastic_DFN_Unfiltered_geometry/case.dfn` | Unfiltered stochastic DFN |
| `09` | `09_Area_1_DFN_Unfiltered_geometry/case.dfn` | Unfiltered mapped DFN, area 1 |
| `10` | `10_Area_2_DFN_Unfiltered_geometry/case.dfn` | Unfiltered mapped DFN, area 2 |
| `11` | `11_stochastic_dfn_10degrees/a10.dfn` | Stochastic DFN with a 10-degree threshold |
| `12` | `12_stochastic_dfn_15degrees/a15.dfn` | Stochastic DFN with a 15-degree threshold |
| `13` | `13_stochastic_dfn_20degrees/a20.dfn` | Stochastic DFN with a 20-degree threshold |

Run one deck from the repository root. Copy it to a temporary directory first
so generated files do not appear in the example package:

```powershell
$work = Join-Path $env:TEMP "opendfn-example"
New-Item -ItemType Directory -Force -Path $work | Out-Null
Copy-Item examples\01_deterministic_joint\fig05_single_fracture.dfn $work
bin\opendfn.exe -in (Join-Path $work "fig05_single_fracture.dfn")
```

Run all 13 decks and validate their output formats and physical groups:

```powershell
powershell -ExecutionPolicy Bypass -File tests\run_regression.ps1
```

The regression script works in temporary directories and does not write
results into the source case directories. Coordinate files must remain beside
the corresponding mapped-network decks.

`examples/MANIFEST_SHA256.json` records the file size and SHA-256 checksum of
the 13 input decks and four coordinate files.
