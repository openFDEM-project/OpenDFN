param(
    [string]$Configuration = "Release"
)

$ErrorActionPreference = "Stop"
$projectRoot = Split-Path -Parent $PSScriptRoot
$executable = Join-Path $projectRoot "src\bin\$Configuration\opendfn.exe"
$reportPath = Join-Path $PSScriptRoot "test_report.md"

if (-not (Test-Path -LiteralPath $executable -PathType Leaf)) {
    throw "OpenDFN executable not found: $executable"
}

$cases = @(
    @{ Name = "Single joint"; Deck = "examples\single_joint\single_joint.dfn"; Mesh = "examples\single_joint\single_joint.msh"; Groups = @("rock", "single_joint") },
    @{ Name = "Multiple joints"; Deck = "examples\multiple_joints\multiple_joints.dfn"; Mesh = "examples\multiple_joints\multiple_joints.msh"; Groups = @("rock", "joint_01", "joint_02", "joint_03", "joint_04", "joint_05") },
    @{ Name = "Continuous joint sets"; Deck = "examples\continuous_joint_sets\continuous_joint_sets.dfn"; Mesh = "examples\continuous_joint_sets\continuous_joint_sets.msh"; Groups = @("rock", "continuous_set_01", "continuous_set_02") },
    @{ Name = "Discontinuous joint sets"; Deck = "examples\discontinuous_joint_sets\discontinuous_joint_sets.dfn"; Mesh = "examples\discontinuous_joint_sets\discontinuous_joint_sets.msh"; Groups = @("rock", "discontinuous_set_01", "discontinuous_set_02") },
    @{ Name = "Arbitrary DFN"; Deck = "examples\arbitrary_dfn\arbitrary_dfn.dfn"; Mesh = "examples\arbitrary_dfn\arbitrary_dfn.msh"; Groups = @("rock", "arbitrary_network") },
    @{ Name = "Input real DFN"; Deck = "examples\input_real_dfn\input_real_dfn.dfn"; Mesh = "examples\input_real_dfn\input_real_dfn.msh"; Groups = @("rock", "input_fractures") },
    @{ Name = "Basic geometry"; Deck = "examples\basic_geometry\basic_geometry.dfn"; Mesh = "examples\basic_geometry\basic_geometry.msh"; Groups = @("rock", "main_joint", "cross_joint") },
    @{ Name = "Random DFN"; Deck = "examples\random_dfn\random_dfn.dfn"; Mesh = "examples\random_dfn\random_dfn.msh"; Groups = @("rock", "set_1", "set_2") },
    @{ Name = "Realistic rDFN"; Deck = "examples\realistic_dfn\realistic_dfn.dfn"; Mesh = "examples\realistic_dfn\realistic_dfn.msh"; Groups = @("rock", "mapped_fractures") }
)

$env:OPENDFN_GMSH_GUI = "0"
$results = @()

foreach ($case in $cases) {
    $deck = Join-Path $projectRoot $case.Deck
    $mesh = Join-Path $projectRoot $case.Mesh
    $runLog = [System.IO.Path]::ChangeExtension($deck, ".run.log")
    Remove-Item -LiteralPath $mesh, $runLog -Force -ErrorAction SilentlyContinue

    $output = & $executable -in $deck 2>&1 | Out-String
    $exitCode = $LASTEXITCODE
    Set-Content -LiteralPath $runLog -Value $output -Encoding UTF8

    $meshExists = Test-Path -LiteralPath $mesh -PathType Leaf
    $meshSize = if ($meshExists) { (Get-Item -LiteralPath $mesh).Length } else { 0 }
    $meshText = if ($meshExists) { Get-Content -LiteralPath $mesh -Raw } else { "" }
    $hasNodes = $meshText.Contains('$Nodes') -and $meshText.Contains('$EndNodes')
    $hasElements = $meshText.Contains('$Elements') -and $meshText.Contains('$EndElements')
    $physicalGroups = @([regex]::Matches($meshText, '(?m)^\d+\s+\d+\s+"([^"]+)"\s*$') | ForEach-Object { $_.Groups[1].Value })
    $groupsValid = @($case.Groups | Where-Object { $_ -notin $physicalGroups }).Count -eq 0
    $hasError = $output -match '(?im)^.*Error:'
    $passed = $exitCode -eq 0 -and $meshSize -gt 100 -and $hasNodes -and $hasElements -and $groupsValid -and -not $hasError

    $results += [pscustomobject]@{
        Case = $case.Name
        Passed = $passed
        ExitCode = $exitCode
        MeshBytes = $meshSize
        NodesSection = $hasNodes
        ElementsSection = $hasElements
        PhysicalGroups = $groupsValid
        Log = $runLog
    }
}

$timestamp = Get-Date -Format "yyyy-MM-dd HH:mm:ss K"
$lines = @(
    "# OpenDFN Regression Test Report",
    "",
    "- Generated: $timestamp",
    "- Executable: ``src\\bin\\$Configuration\\opendfn.exe``",
    "- Gmsh GUI: disabled",
    "",
    "| Case | Result | Exit | Mesh bytes | Nodes | Elements | Physical groups |",
    "|---|---:|---:|---:|---:|---:|---:|"
)

foreach ($result in $results) {
    $status = if ($result.Passed) { "PASS" } else { "FAIL" }
    $lines += "| $($result.Case) | $status | $($result.ExitCode) | $($result.MeshBytes) | $($result.NodesSection) | $($result.ElementsSection) | $($result.PhysicalGroups) |"
}

$lines += ""
$lines += "Each case writes a ``.run.log`` file beside its input deck."
Set-Content -LiteralPath $reportPath -Value $lines -Encoding UTF8

$results | Format-Table -AutoSize
if ($results.Passed -contains $false) {
    throw "One or more OpenDFN regression cases failed. See $reportPath"
}

Write-Host "All OpenDFN regression cases passed. Report: $reportPath"
