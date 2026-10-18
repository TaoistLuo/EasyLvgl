# Build lv_port_linux on Windows with MinGW-w64 (GCC).
#
# Usage (from the repository root):
#   powershell -ExecutionPolicy Bypass -File scripts/win/build.ps1
#   powershell -ExecutionPolicy Bypass -File scripts/win/build.ps1 -Clean
#   powershell -ExecutionPolicy Bypass -File scripts/win/build.ps1 -Target main -Target luic
#
# Requirements:
#   - MinGW-w64 (g++, gcc, mingw32-make) available in PATH
#   - CMake >= 3.12 available in PATH
#
# On Windows no SDL2 is required: the LVGL native Win32 backend is used
# (see lv_conf.h: LV_USE_WINDOWS=1). Linux/macOS still use SDL2.

param(
    [string]$BuildDir = "build-win",
    [string]$Config = "Release",
    [int]$Jobs = 8,
    [switch]$Clean,
    [string[]]$Target = @()
)

$ErrorActionPreference = "Stop"

$repoRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)

if (-not (Get-Command g++ -ErrorAction SilentlyContinue)) {
    throw "g++ not found in PATH. Add your MinGW-w64 bin directory (e.g. Qt's Tools\mingw*_64\bin) to PATH."
}
if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) {
    throw "cmake not found in PATH."
}

$buildPath = Join-Path $repoRoot $BuildDir
if ($Clean -and (Test-Path $buildPath)) {
    Write-Host "[build] Removing $buildPath"
    Remove-Item -LiteralPath $buildPath -Recurse -Force
}

Write-Host "[build] Configuring ($Config) -> $buildPath"
& cmake -S $repoRoot -B $buildPath -G "MinGW Makefiles" `
    -DCMAKE_BUILD_TYPE=$Config `
    -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++
if ($LASTEXITCODE -ne 0) { throw "CMake configure failed ($LASTEXITCODE)" }

$buildArgs = @("--build", $buildPath, "--config", $Config, "-j", "$Jobs")
foreach ($t in $Target) { $buildArgs += @("--target", $t) }

Write-Host "[build] Building"
& cmake @buildArgs
if ($LASTEXITCODE -ne 0) { throw "Build failed ($LASTEXITCODE)" }

Write-Host "[build] Done. Executables are in $repoRoot\bin"
