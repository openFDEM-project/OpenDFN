param(
    [string]$Configuration = "Release"
)

$ErrorActionPreference = "Stop"
$projectRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$examplesRoot = Join-Path $projectRoot "examples"
$executable = Join-Path $projectRoot "bin\opendfn.exe"

if (-not (Test-Path -LiteralPath $executable -PathType Leaf)) {
    throw "OpenDFN executable not found: $executable"
}

$decks = @(Get-ChildItem -LiteralPath $examplesRoot -Recurse -File -Filter "*.dfn" | Sort-Object FullName)
if ($decks.Count -eq 0) {
    throw "No DFN decks were found under $examplesRoot"
}

$workspaceRoot = Join-Path ([System.IO.Path]::GetTempPath()) `
    ("OpenDFN-regression-" + [guid]::NewGuid().ToString("N"))
$env:OPENDFN_GMSH_GUI = "0"
$env:OPENDFN_RANDOM_SEED = "0"
$results = @()

New-Item -ItemType Directory -Path $workspaceRoot | Out-Null
try {
    foreach ($deckSource in $decks) {
        $caseId = $deckSource.Directory.Name
        $workDirectory = Join-Path $workspaceRoot $caseId
        New-Item -ItemType Directory -Path $workDirectory | Out-Null

        $deck = Join-Path $workDirectory $deckSource.Name
        Copy-Item -LiteralPath $deckSource.FullName -Destination $deck
        $deckText = Get-Content -LiteralPath $deck -Raw
        foreach ($line in ($deckText -split "`r?`n")) {
            if ($line -notmatch '^\s*odfn\.geometry\.import\.rdfn\b') {
                continue
            }
            $quotedValues = @([regex]::Matches($line, "'([^']+)'") | ForEach-Object {
                $_.Groups[1].Value
            })
            if ($quotedValues.Count -lt 3) {
                throw "Malformed import.rdfn command in $($deckSource.FullName): $line"
            }
            $dependencyName = $quotedValues[-1]
            $dependencySource = Join-Path $deckSource.DirectoryName $dependencyName
            if (-not (Test-Path -LiteralPath $dependencySource -PathType Leaf)) {
                throw "Required coordinate input not found for $($deckSource.FullName): $dependencyName"
            }
            Copy-Item -LiteralPath $dependencySource -Destination $workDirectory
        }

        $baseName = [System.IO.Path]::GetFileNameWithoutExtension($deck)
        $mesh = Join-Path $workDirectory "$baseName.msh"
        $geo = Join-Path $workDirectory "$baseName.geo"
        $xao = Join-Path $workDirectory "$baseName.geo_unrolled.xao"
        $vtk = Join-Path $workDirectory "$baseName.vtk"

        $expectedGroups = [System.Collections.Generic.List[string]]::new()
        foreach ($line in ($deckText -split "`r?`n")) {
            if ($line -notmatch '^\s*odfn\.geometry\.(square|cut\.(joint|jset|dfn)|import\.rdfn)\b') {
                continue
            }
            $quotedValues = @([regex]::Matches($line, "'([^']+)'") | ForEach-Object {
                $_.Groups[1].Value
            })
            if ($quotedValues.Count -gt 0 -and -not $expectedGroups.Contains($quotedValues[0])) {
                $expectedGroups.Add($quotedValues[0])
            }
        }

        $output = & $executable -in $deck 2>&1 | Out-String
        $exitCode = $LASTEXITCODE

        $meshExists = Test-Path -LiteralPath $mesh -PathType Leaf
        $meshText = if ($meshExists) { Get-Content -LiteralPath $mesh -Raw } else { "" }
        $meshValid = $meshText.Contains('$MeshFormat') -and
                     $meshText.Contains('$Nodes') -and
                     $meshText.Contains('$Elements')

        $geoText = if (Test-Path -LiteralPath $geo -PathType Leaf) {
            Get-Content -LiteralPath $geo -Raw
        } else {
            ""
        }
        $geoUsesXao = $geoText -match '(?m)^\s*Merge\s+'
        $geoHasEntities = $geoText -match '(?m)^Point\(' -and
                          $geoText -match '(?m)^Line\(' -and
                          $geoText -match '(?m)^Plane Surface\('
        $geoValid = ($geoUsesXao -and (Test-Path -LiteralPath $xao -PathType Leaf)) -or
                    $geoHasEntities

        $vtkText = if (Test-Path -LiteralPath $vtk -PathType Leaf) {
            Get-Content -LiteralPath $vtk -Raw
        } else {
            ""
        }
        $vtkValid = $vtkText.StartsWith('# vtk DataFile Version') -and
                    $vtkText.Contains('DATASET UNSTRUCTURED_GRID') -and
                    $vtkText.Contains('POINTS ') -and
                    $vtkText.Contains('CELLS ')

        $physicalGroups = @(
            [regex]::Matches($meshText, '(?m)^\d+\s+\d+\s+"([^"]+)"\s*$') |
                ForEach-Object { $_.Groups[1].Value }
        )
        $missingGroups = @($expectedGroups | Where-Object { $_ -notin $physicalGroups })
        $groupsValid = $missingGroups.Count -eq 0
        $hasRuntimeError = $output -match '(?im)^.*Error:'

        $nodeCount = 0
        $triangleCount = 0
        if ($meshValid) {
            $meshLines = @($meshText -split "`r?`n")
            $nodeMarker = [Array]::IndexOf($meshLines, '$Nodes')
            if ($nodeMarker -ge 0) {
                [void][int]::TryParse($meshLines[$nodeMarker + 1], [ref]$nodeCount)
            }
            $elementMarker = [Array]::IndexOf($meshLines, '$Elements')
            if ($elementMarker -ge 0) {
                $elementCount = 0
                [void][int]::TryParse($meshLines[$elementMarker + 1], [ref]$elementCount)
                for ($i = $elementMarker + 2; $i -lt $elementMarker + 2 + $elementCount; $i++) {
                    $parts = $meshLines[$i].Split(' ', [System.StringSplitOptions]::RemoveEmptyEntries)
                    if ($parts.Count -gt 1 -and $parts[1] -eq '2') {
                        $triangleCount++
                    }
                }
            }
        }

        $passed = $exitCode -eq 0 -and $meshValid -and $geoValid -and
                  $vtkValid -and $groupsValid -and -not $hasRuntimeError

        $results += [pscustomobject]@{
            Case = $caseId
            Passed = $passed
            Exit = $exitCode
            Nodes = $nodeCount
            Triangles = $triangleCount
            Groups = $groupsValid
            GEO = $geoValid
            VTK = $vtkValid
        }

        if (-not $passed) {
            Write-Warning "Output for failed case $caseId`n$output"
            if ($missingGroups.Count -gt 0) {
                Write-Warning "Missing physical groups: $($missingGroups -join ', ')"
            }
        }
    }
} finally {
    if (Test-Path -LiteralPath $workspaceRoot -PathType Container) {
        [System.IO.Directory]::Delete($workspaceRoot, $true)
    }
}

$results | Format-Table -AutoSize
if ($results.Passed -contains $false) {
    throw "One or more OpenDFN example cases failed regression testing."
}

Write-Host "All $($results.Count) OpenDFN example cases passed. Configuration: $Configuration"
Write-Host "OPENDFN_RANDOM_SEED=0 was used and source example inputs were not modified."
