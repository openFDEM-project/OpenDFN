param(
    [string[]]$Case = @("all"),
    [string]$Configuration = "Release"
)

$ErrorActionPreference = "Stop"
$examplesRoot = $PSScriptRoot
$projectRoot = Split-Path -Parent $examplesRoot
$executable = Join-Path $projectRoot "src\bin\$Configuration\opendfn.exe"
$reportPath = Join-Path $projectRoot "tests\format_generation_report.md"

$caseNames = @(
    "single_joint",
    "multiple_joints",
    "continuous_joint_sets",
    "discontinuous_joint_sets",
    "arbitrary_dfn",
    "input_real_dfn",
    "basic_geometry",
    "random_dfn",
    "realistic_dfn"
)

if (-not (Test-Path -LiteralPath $executable -PathType Leaf)) {
    throw "OpenDFN executable not found: $executable"
}

$unknownCases = @($Case | Where-Object { $_ -ne "all" -and $_ -notin $caseNames })
if ($unknownCases.Count -gt 0) {
    throw "Unknown case name(s): $($unknownCases -join ', ')"
}

$selectedCases = if ($Case -contains "all") {
    $caseNames
} else {
    @($caseNames | Where-Object { $_ -in $Case })
}

$gmshCommand = Get-Command gmsh -ErrorAction SilentlyContinue
$env:OPENDFN_GMSH_GUI = "0"
$results = @()

foreach ($caseName in $selectedCases) {
    $caseDirectory = Join-Path $examplesRoot $caseName
    $deck = Join-Path $caseDirectory "$caseName.dfn"
    $mesh = Join-Path $caseDirectory "$caseName.msh"
    $geo = Join-Path $caseDirectory "$caseName.geo"
    $vtk = Join-Path $caseDirectory "$caseName.vtk"
    $temporaryGeo = Join-Path $caseDirectory "$caseName.geo_unrolled"
    $runLog = Join-Path $caseDirectory "$caseName.formats.log"

    Remove-Item -LiteralPath $mesh, $geo, $vtk, $temporaryGeo, $runLog -Force -ErrorAction SilentlyContinue

    $output = & $executable -in $deck 2>&1 | Out-String
    $exitCode = $LASTEXITCODE
    Set-Content -LiteralPath $runLog -Value $output -Encoding UTF8

    $meshText = if (Test-Path -LiteralPath $mesh -PathType Leaf) { Get-Content -LiteralPath $mesh -Raw } else { "" }
    $geoText = if (Test-Path -LiteralPath $geo -PathType Leaf) { Get-Content -LiteralPath $geo -Raw } else { "" }
    $vtkText = if (Test-Path -LiteralPath $vtk -PathType Leaf) { Get-Content -LiteralPath $vtk -Raw } else { "" }

    $meshValid = $meshText.Contains('$MeshFormat') -and $meshText.Contains('$Nodes') -and $meshText.Contains('$Elements')
    $geoValid = $geoText -match '(?m)^Point\(' -and $geoText -match '(?m)^Line\(' -and $geoText -match '(?m)^Plane Surface\('
    $vtkValid = $vtkText.StartsWith('# vtk DataFile Version') -and
                $vtkText.Contains('DATASET UNSTRUCTURED_GRID') -and
                $vtkText.Contains('POINTS ') -and
                $vtkText.Contains('CELLS ')
    $geoSyntaxValid = $true

    if ($gmshCommand -and $geoValid) {
        $geoCheckOutput = & $gmshCommand.Source $geo -parse_and_exit 2>&1 | Out-String
        $geoSyntaxValid = $LASTEXITCODE -eq 0 -and $geoCheckOutput -notmatch '(?im)^Error\s*:'
    }

    $hasRuntimeError = $output -match '(?im)^.*Error:'
    $temporaryFileAbsent = -not (Test-Path -LiteralPath $temporaryGeo)
    $passed = $exitCode -eq 0 -and $meshValid -and $geoValid -and $vtkValid -and
              $geoSyntaxValid -and $temporaryFileAbsent -and -not $hasRuntimeError

    $results += [pscustomobject]@{
        Case = $caseName
        Passed = $passed
        Exit = $exitCode
        MSH = $meshValid
        GEO = $geoValid
        GEOParse = if ($gmshCommand) { $geoSyntaxValid } else { "SKIP" }
        VTK = $vtkValid
        Log = $runLog
    }
}

$timestamp = Get-Date -Format "yyyy-MM-dd HH:mm:ss K"
$gmshStatus = if ($gmshCommand) { $gmshCommand.Source } else { "not found; GEO parser check skipped" }
$report = @(
    "# OpenDFN Format Generation Report",
    "",
    "- Generated: $timestamp",
    "- Executable: ``src\bin\$Configuration\opendfn.exe``",
    "- GEO syntax checker: ``$gmshStatus``",
    "",
    "| Case | Result | Exit | MSH | GEO | GEO parse | VTK |",
    "|---|---:|---:|---:|---:|---:|---:|"
)

foreach ($result in $results) {
    $status = if ($result.Passed) { "PASS" } else { "FAIL" }
    $report += "| $($result.Case) | $status | $($result.Exit) | $($result.MSH) | $($result.GEO) | $($result.GEOParse) | $($result.VTK) |"
}

$report += ""
$report += "Each case writes ``<case>.msh``, ``<case>.geo``, ``<case>.vtk`` and ``<case>.formats.log`` beside its input deck."
Set-Content -LiteralPath $reportPath -Value $report -Encoding UTF8

$results | Format-Table -AutoSize
if ($results.Passed -contains $false) {
    throw "One or more format generation cases failed. See $reportPath"
}

Write-Host "All selected OpenDFN cases generated valid MSH, GEO and VTK files."
Write-Host "Report: $reportPath"
