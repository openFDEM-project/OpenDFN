$ErrorActionPreference = "Stop"

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$caseRoot = Join-Path $repoRoot "examples"
$errors = [System.Collections.Generic.List[string]]::new()

function Add-Error([string]$Message) {
    $script:errors.Add($Message)
}

if (-not (Test-Path -LiteralPath $caseRoot -PathType Container)) {
    throw "Examples directory not found: $caseRoot"
}

$caseDirectories = @(Get-ChildItem -LiteralPath $caseRoot -Directory | Sort-Object Name)
if ($caseDirectories.Count -eq 0) {
    Add-Error "No example case directories were found."
}

foreach ($directory in $caseDirectories) {
    if ($directory.Name -notmatch '^\d{2}_[A-Za-z0-9_]+$') {
        Add-Error "Case directory must start with a two-digit ID and contain only portable characters: $($directory.Name)"
    }

    foreach ($nestedDirectory in @(Get-ChildItem -LiteralPath $directory.FullName -Directory -Recurse)) {
        Add-Error "Nested directory is not allowed in an example case: $($nestedDirectory.FullName)"
    }

    $decks = @(Get-ChildItem -LiteralPath $directory.FullName -File -Filter "*.dfn")
    if ($decks.Count -ne 1) {
        Add-Error "Each case directory must contain exactly one DFN deck: $($directory.FullName)"
    }
}

$generatedExtensions = @(".geo", ".msh", ".vtk", ".vtu", ".xao", ".log", ".of")
foreach ($file in @(Get-ChildItem -LiteralPath $caseRoot -Recurse -File | Where-Object {
    $_.Extension.ToLowerInvariant() -in $generatedExtensions
})) {
    Add-Error "Generated output must not be stored in the input package: $($file.FullName)"
}

$absolutePathPattern = '(?i)(/workspace/|/Users/|(?<![A-Za-z0-9_-])[A-Z]:\\|project004)'
foreach ($file in @(Get-ChildItem -LiteralPath $caseRoot -Recurse -File | Where-Object {
    $_.Extension.ToLowerInvariant() -in @(".txt", ".json", ".dfn", ".ps1")
})) {
    $content = Get-Content -LiteralPath $file.FullName -Raw
    if ($content -match $absolutePathPattern) {
        Add-Error "Absolute or machine-specific path in $($file.FullName)"
    }
}

$deckFiles = @(Get-ChildItem -LiteralPath $caseRoot -Recurse -File -Filter "*.dfn" | Sort-Object FullName)
foreach ($deck in $deckFiles) {
    $content = Get-Content -LiteralPath $deck.FullName -Raw
    if ($content -notmatch '(?m)^\s*odfn\.geometry\.') {
        Add-Error "No odfn.geometry command found in $($deck.FullName)"
    }
    if ($content -match '(?m)^\s*of\.geometry\.') {
        Add-Error "Invalid of.geometry prefix in $($deck.FullName)"
    }

    foreach ($line in ($content -split "`r?`n")) {
        if ($line -notmatch '^\s*odfn\.geometry\.import\.rdfn\b') {
            continue
        }
        $quotedValues = @([regex]::Matches($line, "'([^']+)'") | ForEach-Object {
            $_.Groups[1].Value
        })
        if ($quotedValues.Count -lt 3) {
            Add-Error "Malformed import.rdfn command in $($deck.FullName): $line"
            continue
        }
        $dependencyName = $quotedValues[-1]
        if ([System.IO.Path]::IsPathRooted($dependencyName)) {
            Add-Error "Coordinate dependency must use a relative path in $($deck.FullName): $dependencyName"
            continue
        }
        $dependencyPath = Join-Path $deck.DirectoryName $dependencyName
        if (-not (Test-Path -LiteralPath $dependencyPath -PathType Leaf)) {
            Add-Error "Missing coordinate dependency for $($deck.FullName): $dependencyName"
        }
    }
}

$rootManifestPath = Join-Path $caseRoot "MANIFEST_SHA256.json"
if (-not (Test-Path -LiteralPath $rootManifestPath -PathType Leaf)) {
    Add-Error "Missing example manifest: $rootManifestPath"
    $rootManifest = @()
} else {
    $manifestJson = Get-Content -LiteralPath $rootManifestPath -Raw | ConvertFrom-Json
    if ($manifestJson -is [System.Array]) {
        $rootManifest = @($manifestJson)
    } elseif ($manifestJson.path -is [System.Array]) {
        $rootManifest = @(
            for ($i = 0; $i -lt $manifestJson.path.Count; $i++) {
                [pscustomobject]@{
                    path = $manifestJson.path[$i]
                    bytes = $manifestJson.bytes[$i]
                    sha256 = $manifestJson.sha256[$i]
                }
            }
        )
    } else {
        $rootManifest = @($manifestJson)
    }
}

$manifestPaths = @{}
foreach ($entry in $rootManifest) {
    $manifestPath = [string]$entry.path
    if ([string]::IsNullOrWhiteSpace($manifestPath)) {
        Add-Error "Manifest contains an entry without a path."
        continue
    }
    if ($manifestPaths.ContainsKey($manifestPath)) {
        Add-Error "Duplicate manifest path: $manifestPath"
        continue
    }
    $manifestPaths[$manifestPath] = $true

    $relativePath = $manifestPath.Replace('/', [System.IO.Path]::DirectorySeparatorChar)
    $path = [System.IO.Path]::GetFullPath((Join-Path $caseRoot $relativePath))
    if (-not $path.StartsWith($caseRoot + [System.IO.Path]::DirectorySeparatorChar,
            [System.StringComparison]::OrdinalIgnoreCase)) {
        Add-Error "Manifest path escapes examples: $manifestPath"
        continue
    }
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) {
        Add-Error "Missing manifest file: $manifestPath"
        continue
    }

    $item = Get-Item -LiteralPath $path
    $hash = (Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash.ToLowerInvariant()
    if ($item.Length -ne [int64]$entry.bytes -or $hash -ne [string]$entry.sha256) {
        Add-Error "Manifest checksum or size mismatch: $manifestPath"
    }
}

$packageFiles = @(Get-ChildItem -LiteralPath $caseRoot -Recurse -File | Where-Object {
    $_.FullName -ne $rootManifestPath
})
foreach ($file in $packageFiles) {
    $relativePath = $file.FullName.Substring($caseRoot.Length + 1).Replace('\', '/')
    if (-not $manifestPaths.ContainsKey($relativePath)) {
        Add-Error "File is absent from the example manifest: $relativePath"
    }
}
if ($rootManifest.Count -ne $packageFiles.Count) {
    Add-Error "Manifest entry count does not match the example package file count."
}

if ($errors.Count -gt 0) {
    $errors | ForEach-Object { Write-Error $_ }
    exit 1
}

Write-Output ("Example package validation passed: {0} cases and {1} files checked." -f `
    $deckFiles.Count, $rootManifest.Count)
