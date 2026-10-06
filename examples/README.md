# OpenDFN manuscript examples

This file connects the examples in the manuscript **OpenDFN: an open-source
kernel for generating conforming discrete-fracture-network meshes** to the
input decks distributed in this directory. Paths below are relative to
`examples/`. The mapping follows the manuscript's current section and figure
numbers, not the historical figure numbers retained in some filenames.

## Manuscript-to-input correspondence

| Manuscript location | Example | Input deck | Additional coordinate data |
| --- | --- | --- | --- |
| Section 3.1; Fig. 2(a,b) | One prescribed deterministic fracture | `01_deterministic_joint/fig05_single_fracture.dfn` | None |
| Section 3.1; Fig. 2(c,d) | Two intersecting continuous joint sets | `02_intersecting_continuous_joint_sets/fig06_double_extended_domain.dfn` | None |
| Section 3.2; Fig. 3(a) | One discontinuous joint set | `03_single_discontinuous_joint_set/fig07a_nonpersistent_single_set.dfn` | None |
| Section 3.2; Fig. 3(b) | Two intersecting discontinuous joint sets | `04_intersecting_discontinuous_joint_sets/fig07b_nonpersistent_double_set.dfn` | None |
| Section 3.3; Fig. 4 | Stochastic DFN with geometric screening | `05_stochastic_dfn/Fig08a_high_density_stochastic_DFN.dfn` | None |
| Section 3.4; Fig. 5(b,d) | Image-derived DFN, area 1 | `06_image_derived_dfn_area_1/runtime.dfn` | `06_image_derived_dfn_area_1/leftsideFaults_space.txt` |
| Section 3.4; Fig. 5(c,e) | Image-derived DFN, area 2 | `07_image_derived_dfn_area_2/runtime.dfn` | `07_image_derived_dfn_area_2/leftsideJoints_space.txt` |

Fig. 5(a) is the source photograph and selected regions. OpenDFN reads the
digitized coordinate data listed above; it does not use the photograph as an
input deck. Likewise, generating a mesh is separate from assembling the final
manuscript panels or plotting realized distributions.

For example, `fig05_single_fracture.dfn` corresponds to **current Fig. 2(a,b)**,
and `Fig08a_high_density_stochastic_DFN.dfn` corresponds to **current Fig. 4**.
Those input filenames have been retained to keep the distributed decks and
their checksum entries stable.

## Section 3.5 and Table 4 comparisons

| Table 4 case | Filtered input | Unfiltered counterpart |
| --- | --- | --- |
| Stochastic DFN (Section 3.3) | `05_stochastic_dfn/Fig08a_high_density_stochastic_DFN.dfn` | `08_Stochastic_DFN_Unfiltered_geometry/case.dfn` |
| Area 1 DFN (Section 3.4) | `06_image_derived_dfn_area_1/runtime.dfn` | `09_Area_1_DFN_Unfiltered_geometry/case.dfn` and `fractures.txt` in that directory |
| Area 2 DFN (Section 3.4) | `07_image_derived_dfn_area_2/runtime.dfn` | `10_Area_2_DFN_Unfiltered_geometry/case.dfn` and `fractures.txt` in that directory |

The unfiltered decks disable the geometric thresholds while retaining the
other nominal input parameters. Filtering occurs before final meshing, so the
comparison includes differences in the retained fracture geometry as well as
in its mesh. A common seed does not force the same accepted network under
different screening conditions.

Cases `11_stochastic_dfn_10degrees/a10.dfn`,
`12_stochastic_dfn_15degrees/a15.dfn`, and
`13_stochastic_dfn_20degrees/a20.dfn` are additional threshold-variation decks.
They are not the three main filtered/unfiltered pairs in Table 4. The
`OpenFDEM_angle_validation_cases/` directory contains downstream validation
materials with its own existing instructions.

## Random sampling and the reported seed

OpenDFN's random-number helper (`src/common/opendfn_random.h`) initializes a
`std::mt19937_64` engine from the `OPENDFN_RANDOM_SEED` environment variable.
An absent, empty, or unparsable value gives seed **0**. The reported
random-generation examples use seed **0**; set it explicitly when reproducing
them:

```powershell
$env:OPENDFN_RANDOM_SEED = "0"
```

For a stochastic DFN, candidate centres are sampled uniformly over the
generation region. Orientations and lengths follow the models selected in the
deck. In the Section 3.3 case, orientations are uniform over 0 to 45 degrees,
and the underlying normal length model has mean 0.90 m and standard deviation
0.90 m. The implementation resamples lengths below 5% of the prescribed mean
and screens candidates geometrically before accepting them. Rejected
candidates are replaced by new samples. Thus, the input distribution defines
candidate generation, while acceptance and clipping also affect the realized
network. Structured joint sets use random samples when distribution-based
orientation, spacing, trace, or gap parameters are selected.

Record the seed together with the full input deck, coordinate files where
applicable, screening parameters, OpenDFN version, compiler/standard-library
environment, and Gmsh version. Repeat the same deck with the same seed in the
same software environment to reproduce its realization. Standard-library
distribution implementations and meshing environments can differ across
platforms, so bitwise-identical output across different builds is not assumed.

## Running a case on Windows

Use the executable and matching Gmsh runtime supplied with the release. The
following PowerShell example starts in the repository root and runs a working
copy, keeping generated output out of the distributed case directory:

```powershell
$repo = (Get-Location).Path
$exe = Join-Path $repo "bin\opendfn.exe"
$work = Join-Path $env:TEMP ("opendfn-paper-" + [guid]::NewGuid().ToString("N"))
New-Item -ItemType Directory -Path $work | Out-Null
Copy-Item -LiteralPath (Join-Path $repo "examples\05_stochastic_dfn\Fig08a_high_density_stochastic_DFN.dfn") -Destination $work
$env:OPENDFN_RANDOM_SEED = "0"
Push-Location $work
try {
    & $exe -in "Fig08a_high_density_stochastic_DFN.dfn"
} finally {
    Pop-Location
}
```

For an image-derived case, copy the coordinate file together with the deck and
run in the working directory so that its relative filename can be resolved:

```powershell
$repo = (Get-Location).Path
$exe = Join-Path $repo "bin\opendfn.exe"
$work = Join-Path $env:TEMP ("opendfn-area1-" + [guid]::NewGuid().ToString("N"))
New-Item -ItemType Directory -Path $work | Out-Null
Copy-Item -LiteralPath (Join-Path $repo "examples\06_image_derived_dfn_area_1\runtime.dfn") -Destination $work
Copy-Item -LiteralPath (Join-Path $repo "examples\06_image_derived_dfn_area_1\leftsideFaults_space.txt") -Destination $work
$env:OPENDFN_RANDOM_SEED = "0"
Push-Location $work
try {
    & $exe -in "runtime.dfn"
} finally {
    Pop-Location
}
```

Apply the same procedure to the other decks, retaining their coordinate
filenames where required. The executable writes the generated mesh and
geometry files beside the working deck; the log is written in the execution
directory. Keep any OCC `.geo_unrolled.xao` sidecar together with the `.geo`
wrapper when viewing exported geometry.

The repository already provides `MANIFEST_SHA256.json` for the numbered input
decks and coordinate files. From the repository root, the existing checks can
be run as follows:

```powershell
powershell -ExecutionPolicy Bypass -File tests\check_paper_cases.ps1
powershell -ExecutionPolicy Bypass -File tests\run_regression.ps1
```

These scripts use temporary working directories. The new README is a mapping
and usage document; the manifest continues to identify the distributed case
inputs.
