# Build the Qt visual designer (tools/lvgl-designer) on Windows with MinGW-w64.
#
# Usage (from the repository root):
#   powershell -ExecutionPolicy Bypass -File scripts/win/build-designer.ps1
#   powershell -ExecutionPolicy Bypass -File scripts/win/build-designer.ps1 -Clean
#   powershell -ExecutionPolicy Bypass -File scripts/win/build-designer.ps1 -QtDir "E:/Qt/5.15.2/mingw81_64"
#
# Requirements:
#   - MinGW-w64 (g++, gcc) in PATH
#   - CMake >= 3.14 in PATH
#   - Qt5 built for the same MinGW (e.g. Qt's 5.12.2 mingw73_64) via -QtDir

param(
    [string]$QtDir = "E:/QT/QT5.12.2/5.12.2/mingw73_64",
    [string]$BuildDir = "build-win",
    [string]$Config = "Release",
    [int]$Jobs = 8,
    [switch]$Clean
)

$ErrorActionPreference = "Stop"

$repoRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$designerDir = Join-Path $repoRoot "tools\lvgl-designer"

if (-not (Get-Command g++ -ErrorAction SilentlyContinue)) {
    throw "g++ not found in PATH. Add your MinGW-w64 bin directory to PATH."
}
if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) {
    throw "cmake not found in PATH."
}
if (-not (Test-Path (Join-Path $QtDir "lib\cmake\Qt5\Qt5Config.cmake"))) {
    throw "Qt5 not found at -QtDir '$QtDir'. Pass the matching MinGW Qt kit, e.g. -QtDir E:/Qt/5.12.2/mingw73_64"
}

$buildPath = Join-Path $designerDir $BuildDir
if ($Clean -and (Test-Path $buildPath)) {
    Write-Host "[designer] Removing $buildPath"
    Remove-Item -LiteralPath $buildPath -Recurse -Force
}

Write-Host "[designer] Configuring ($Config) -> $buildPath"
& cmake -S $designerDir -B $buildPath -G "MinGW Makefiles" `
    -DCMAKE_BUILD_TYPE=$Config `
    -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ `
    -DCMAKE_PREFIX_PATH=$QtDir
if ($LASTEXITCODE -ne 0) { throw "CMake configure failed ($LASTEXITCODE)" }

Write-Host "[designer] Building"
& cmake --build $buildPath --config $Config -j $Jobs
if ($LASTEXITCODE -ne 0) { throw "Build failed ($LASTEXITCODE)" }

Write-Host "[designer] Done: $buildPath\lvgl-designer.exe"
Write-Host "[designer] Run with Qt bin in PATH, e.g.:"
Write-Host ("  `$env:Path = '{0}\bin;' + `$env:Path; & '{1}\lvgl-designer.exe'" -f $QtDir, $buildPath)
