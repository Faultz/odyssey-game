# Rename Project Script for Windows
# Usage: .\scripts\rename-project.ps1 -NewName "my-project-name"

param(
    [Parameter(Mandatory=$true)]
    [string]$NewName
)

$OldName = "odyssey-game"

# Validate input
if ($NewName -notmatch '^[a-zA-Z][a-zA-Z0-9_-]*$') {
    Write-Error "Project name must start with a letter and contain only letters, numbers, hyphens, and underscores"
    exit 1
}

Write-Host "Renaming project from '$OldName' to '$NewName'" -ForegroundColor Cyan

# Check if already renamed
if (-not (Test-Path "$OldName.sln")) {
    Write-Error "Project appears to already be renamed (no $OldName.sln found)"
    exit 1
}

# Step 1: Update content inside files
Write-Host "Updating file contents..." -ForegroundColor Yellow

$filesToUpdate = @(
    "$OldName.sln",
    "$OldName\$OldName.vcxproj",
    "$OldName\$OldName.vcxproj.filters",
    "$OldName\$OldName.vcxproj.user",
    "README.md"
)

foreach ($file in $filesToUpdate) {
    if (Test-Path $file) {
        (Get-Content $file -Raw) -replace $OldName, $NewName | Set-Content $file -NoNewline
        Write-Host "  Updated: $file" -ForegroundColor Green
    }
}

# Update source files
Get-ChildItem -Path $OldName -Recurse -Include *.cpp,*.h,*.hpp | ForEach-Object {
    $content = Get-Content $_.FullName -Raw
    if ($content -match $OldName) {
        $content -replace $OldName, $NewName | Set-Content $_.FullName -NoNewline
        Write-Host "  Updated: $($_.Name)" -ForegroundColor Green
    }
}

# Step 2: Rename files
Write-Host "Renaming files..." -ForegroundColor Yellow

$filesToRename = @(
    @{ Old = "$OldName\$OldName.vcxproj"; New = "$OldName\$NewName.vcxproj" },
    @{ Old = "$OldName\$OldName.vcxproj.filters"; New = "$OldName\$NewName.vcxproj.filters" },
    @{ Old = "$OldName\$OldName.vcxproj.user"; New = "$OldName\$NewName.vcxproj.user" }
)

foreach ($item in $filesToRename) {
    if (Test-Path $item.Old) {
        Rename-Item -Path $item.Old -NewName (Split-Path $item.New -Leaf)
        Write-Host "  Renamed: $($item.Old) -> $($item.New)" -ForegroundColor Green
    }
}

# Step 3: Rename the project folder
if (Test-Path $OldName) {
    Rename-Item -Path $OldName -NewName $NewName
    Write-Host "  Renamed folder: $OldName -> $NewName" -ForegroundColor Green
}

# Step 4: Rename the solution file
if (Test-Path "$OldName.sln") {
    Rename-Item -Path "$OldName.sln" -NewName "$NewName.sln"
    Write-Host "  Renamed: $OldName.sln -> $NewName.sln" -ForegroundColor Green
}

# Step 5: Clean up build directories
$dirsToRemove = @("ORBIS_Debug", "ORBIS_Release", "$NewName\ORBIS_Debug", "$NewName\ORBIS_Release")
foreach ($dir in $dirsToRemove) {
    if (Test-Path $dir) {
        Remove-Item -Path $dir -Recurse -Force
        Write-Host "  Removed: $dir" -ForegroundColor Gray
    }
}

Write-Host "`nProject renamed successfully to '$NewName'!" -ForegroundColor Green
Write-Host "You can now open $NewName.sln in Visual Studio" -ForegroundColor Cyan
