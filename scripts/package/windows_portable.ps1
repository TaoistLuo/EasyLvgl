param(
    [string]$Stage = "",
    [string]$QtRoot = "E:\QT\QT5.12.2\5.12.2\mingw73_64",
    [string]$MingwRoot = "E:\QT\QT5.12.2\Tools\mingw730_64",
    [string]$CmakeRoot = "E:\QT\Tools\cmake",
    [string]$PythonRoot = "C:\Python314",
    [switch]$SkipBuild,
    [switch]$Zip
)

$ErrorActionPreference = "Stop"
$repo = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
if (-not $Stage) {
    $Stage = Join-Path (Split-Path -Parent $repo) "dist\EasyLVGL"
}
$Stage = [IO.Path]::GetFullPath($Stage)
$Bundle = Split-Path -Parent $Stage

$windeployqt = Join-Path $QtRoot "bin\windeployqt.exe"
$designerBuild = Join-Path $repo "tools\lvgl-designer\build-win"
$designerExe = Join-Path $designerBuild "lvgl-designer.exe"
$rootBuild = Join-Path $repo "build-win"

foreach ($p in @($QtRoot, $MingwRoot, $CmakeRoot, $PythonRoot, $windeployqt)) {
    if (-not (Test-Path -LiteralPath $p)) { throw "missing dependency: $p" }
}

function Invoke-Robocopy {
    param([string]$Source, [string]$Dest)
    & robocopy $Source $Dest /E /NFL /NDL /NJH /NJS /NP /R:1 /W:1 /XF .git | Out-Null
    if ($LASTEXITCODE -ge 8) { throw "robocopy failed ($LASTEXITCODE): $Source -> $Dest" }
}

function Copy-Headers {
    param([string]$Source, [string]$Dest, [string[]]$ExcludeTop = @())
    $root = [IO.Path]::GetFullPath($Source)
    Get-ChildItem -LiteralPath $root -Recurse -File |
        Where-Object { $_.Extension -eq ".h" -or $_.Extension -eq ".hpp" } |
        ForEach-Object {
        $rel = $_.FullName.Substring($root.Length).TrimStart('\')
        $skip = $false
        foreach ($e in $ExcludeTop) {
            if ($rel -eq $e -or $rel.StartsWith($e + "\")) { $skip = $true; break }
        }
        if ($skip) { return }
        $out = Join-Path $Dest $rel
        New-Item -ItemType Directory -Path (Split-Path -Parent $out) -Force | Out-Null
        Copy-Item -LiteralPath $_.FullName -Destination $out -Force
    }
}

if (-not $SkipBuild) {
    & cmake --build $rootBuild --target easylvgl_archive lvgl_linux --parallel 8
    if ($LASTEXITCODE -ne 0) { throw "SDK build failed" }
    & cmake --build $designerBuild --target lvgl-designer --parallel 8
    if ($LASTEXITCODE -ne 0) { throw "designer build failed" }
}
if (-not (Test-Path -LiteralPath $designerExe)) { throw "designer exe not found: $designerExe" }
$sdkLib = Join-Path $rootBuild "lib\libeasylvgl.a"
$platformLib = Join-Path $rootBuild "liblvgl_linux.a"
$autoconf = Join-Path $rootBuild "lvgl\autoconf.h"
foreach ($p in @($sdkLib, $platformLib, $autoconf)) {
    if (-not (Test-Path -LiteralPath $p)) { throw "SDK artifact not found: $p" }
}

if (Test-Path -LiteralPath $Stage) { Remove-Item -LiteralPath $Stage -Recurse -Force }
New-Item -ItemType Directory -Path $Stage -Force | Out-Null

$toolchain = Join-Path $Bundle "toolchain"
$examplesDir = Join-Path $Bundle "examples"
foreach ($shared in @($toolchain, $examplesDir)) {
    if (Test-Path -LiteralPath $shared) {
        Remove-Item -LiteralPath $shared -Recurse -Force
    }
}

$gxx = Join-Path $MingwRoot "bin\g++.exe"
& $gxx -std=c++17 -O2 -mwindows -static -static-libgcc -static-libstdc++ `
    -o (Join-Path $Bundle "EasyLVGL.exe") (Join-Path $PSScriptRoot "launcher.cpp")
if ($LASTEXITCODE -ne 0) { throw "launcher build failed" }

$designerOut = Join-Path $Stage "tools\lvgl-designer\build-win"
New-Item -ItemType Directory -Path $designerOut -Force | Out-Null
Copy-Item -LiteralPath $designerExe -Destination $designerOut -Force
& $windeployqt --release --compiler-runtime --no-translations --dir $designerOut $designerExe
if ($LASTEXITCODE -ne 0) { throw "windeployqt failed" }

foreach ($dll in @("libgcc_s_seh-1.dll", "libstdc++-6.dll", "libwinpthread-1.dll")) {
    $src = Join-Path $MingwRoot "bin\$dll"
    if (Test-Path -LiteralPath $src) { Copy-Item -LiteralPath $src -Destination $designerOut -Force }
}
$qsvgPlugin = Join-Path $QtRoot "plugins\imageformats\qsvg.dll"
if (Test-Path -LiteralPath $qsvgPlugin) {
    New-Item -ItemType Directory -Path (Join-Path $designerOut "imageformats") -Force | Out-Null
    Copy-Item -LiteralPath $qsvgPlugin -Destination (Join-Path $designerOut "imageformats") -Force
}
$qsvgLib = Join-Path $QtRoot "bin\Qt5Svg.dll"
if (Test-Path -LiteralPath $qsvgLib) {
    Copy-Item -LiteralPath $qsvgLib -Destination $designerOut -Force
}

$binOut = Join-Path $Stage "bin"
New-Item -ItemType Directory -Path $binOut -Force | Out-Null
$luic = Join-Path $repo "bin\luic.exe"
if (-not (Test-Path -LiteralPath $luic)) { throw "luic.exe not found: $luic" }
Copy-Item -LiteralPath $luic -Destination $binOut -Force
foreach ($dll in @("libgcc_s_seh-1.dll", "libstdc++-6.dll", "libwinpthread-1.dll")) {
    $src = Join-Path $MingwRoot "bin\$dll"
    if (Test-Path -LiteralPath $src) { Copy-Item -LiteralPath $src -Destination $binOut -Force }
}

$sdkInclude = Join-Path $Stage "sdk\include"
$sdkLibDir = Join-Path $Stage "sdk\lib"
New-Item -ItemType Directory -Path $sdkInclude, $sdkLibDir -Force | Out-Null
Copy-Item -LiteralPath $sdkLib -Destination $sdkLibDir -Force
Copy-Item -LiteralPath $platformLib -Destination $sdkLibDir -Force
Copy-Item -LiteralPath $autoconf -Destination (Join-Path $sdkInclude "autoconf.h") -Force
Copy-Headers -Source (Join-Path $repo "lvgl") -Dest (Join-Path $sdkInclude "lvgl") `
    -ExcludeTop @("tests", "demos", "docs", "examples", "env_support", "scripts")
Copy-Headers -Source (Join-Path $repo "easylvgl") -Dest (Join-Path $sdkInclude "easylvgl")
Copy-Headers -Source (Join-Path $repo "src\lib") -Dest (Join-Path $Stage "src\lib")

New-Item -ItemType Directory -Path (Join-Path $Stage "scripts\assets") -Force | Out-Null
Copy-Item -LiteralPath (Join-Path $repo "scripts\assets\convert_assets.py") `
    -Destination (Join-Path $Stage "scripts\assets") -Force
Copy-Item -LiteralPath (Join-Path $repo "lvgl\scripts\LVGLImage.py") `
    -Destination (Join-Path $Stage "scripts\assets") -Force
Copy-Item -LiteralPath (Join-Path $PSScriptRoot "sdk-CMakeLists.txt") `
    -Destination (Join-Path $Stage "CMakeLists.txt") -Force

$demoSrc = Join-Path $repo "examples\excavator_dashboard"
$demoDst = Join-Path $examplesDir "excavator_dashboard"
New-Item -ItemType Directory -Path (Split-Path -Parent $demoDst) -Force | Out-Null
Copy-Item -LiteralPath $demoSrc -Destination $demoDst -Recurse -Force
Remove-Item -LiteralPath (Join-Path $demoDst "build"), (Join-Path $demoDst "build_lvgl") `
    -Recurse -Force -ErrorAction SilentlyContinue

New-Item -ItemType Directory -Path $toolchain -Force | Out-Null
Invoke-Robocopy -Source $MingwRoot -Dest (Join-Path $toolchain "mingw64")
Invoke-Robocopy -Source $CmakeRoot -Dest (Join-Path $toolchain "cmake")
Invoke-Robocopy -Source $PythonRoot -Dest (Join-Path $toolchain "python")

$python = Join-Path $toolchain "python\python.exe"
$sitePackages = Join-Path $toolchain "python\Lib\site-packages"
& $python -m pip install --disable-pip-version-check --no-input --upgrade `
    --target $sitePackages pypng lz4
if ($LASTEXITCODE -ne 0) {
    & (Join-Path $PythonRoot "python.exe") -m pip install --disable-pip-version-check `
        --no-input --upgrade --target $sitePackages pypng lz4
    if ($LASTEXITCODE -ne 0) { throw "pip install pypng/lz4 failed" }
}
& $python -c "import png, lz4; print('python deps ok')"
if ($LASTEXITCODE -ne 0) { throw "bundled python cannot import png/lz4" }

$startDesigner = @'
@echo off
setlocal
start "" "%~dp0EasyLVGL.exe"
'@
Set-Content -LiteralPath (Join-Path $Bundle "Start-Designer.cmd") -Value $startDesigner -Encoding ASCII

$startShell = @'
@echo off
setlocal
set "ROOT=%~dp0"
set "TC=%ROOT%toolchain"
if not exist "%TC%\mingw64\bin\g++.exe" set "TC=%ROOT%..\toolchain"
if not exist "%TC%\mingw64\bin\g++.exe" set "TC=%ROOT%..\..\toolchain"
set "PATH=%TC%\mingw64\bin;%TC%\cmake\bin;%TC%\python;%TC%\python\Scripts;%ROOT%EasyLVGL\bin;%PATH%"
set "EASYLVGL_ROOT=%ROOT%EasyLVGL"
set "CMAKE_GENERATOR=MinGW Makefiles"
cmd /k
'@
Set-Content -LiteralPath (Join-Path $Bundle "Start-Command-Prompt.cmd") -Value $startShell -Encoding ASCII

$readme = Join-Path $PSScriptRoot "PORTABLE-README.md"
if (Test-Path -LiteralPath $readme) {
    Copy-Item -LiteralPath $readme -Destination (Join-Path $Bundle "README-PORTABLE.md") -Force
}

$packagePaths = @(
    $Stage,
    $toolchain,
    $examplesDir,
    (Join-Path $Bundle "EasyLVGL.exe"),
    (Join-Path $Bundle "Start-Designer.cmd"),
    (Join-Path $Bundle "Start-Command-Prompt.cmd"),
    (Join-Path $Bundle "README-PORTABLE.md")
)
$sizeBytes = 0
foreach ($p in $packagePaths) {
    if (-not (Test-Path -LiteralPath $p)) { continue }
    $item = Get-Item -LiteralPath $p
    if ($item.PSIsContainer) {
        $sizeBytes += (Get-ChildItem -LiteralPath $p -Recurse -File |
            Measure-Object -Property Length -Sum).Sum
    } else {
        $sizeBytes += $item.Length
    }
}
$sizeMb = [math]::Round(($sizeBytes / 1MB), 0)
Write-Host "portable bundle: $Bundle ($sizeMb MB)"

if ($Zip) {
    $zipPath = Join-Path $Bundle "EasyLVGL-Bundle.zip"
    if (Test-Path -LiteralPath $zipPath) { Remove-Item -LiteralPath $zipPath -Force }
    $zipItems = @(
        "EasyLVGL", "toolchain", "examples", "EasyLVGL.exe",
        "Start-Designer.cmd", "Start-Command-Prompt.cmd", "README-PORTABLE.md"
    )
    & tar.exe -a -c -f $zipPath -C $Bundle @zipItems
    if ($LASTEXITCODE -ne 0) { throw "zip failed" }
    $zipMb = [math]::Round(((Get-Item -LiteralPath $zipPath).Length / 1MB), 0)
    Write-Host "portable zip: $zipPath ($zipMb MB)"
}
