param(
    [string]$OhosNativeHome = $env:OHOS_NATIVE_HOME,
    [ValidateSet("arm64-v8a", "armeabi-v7a")]
    [string]$Arch = "arm64-v8a",
    [switch]$Clean
)

$ErrorActionPreference = "Stop"
$Root = Split-Path -Parent $PSScriptRoot
$CmakeDir = Join-Path $Root "src\main\cpp\ohos"
$BuildDir = Join-Path $Root "build-ohos-$Arch"

function Resolve-OhosNativeHome {
    param([string]$Hint)
    $candidates = @()
    if ($Hint) { $candidates += $Hint }
    $candidates += @(
        "D:\StudioProjects\Http2p2p_API\.ohos-sdk\native",
        (Join-Path $Root ".ohos-sdk\native")
    )
    foreach ($path in $candidates) {
        $toolchain = Join-Path $path "build\cmake\ohos.toolchain.cmake"
        if (Test-Path $toolchain) { return (Resolve-Path $path).Path }
    }
    throw "OpenHarmony Native SDK not found."
}

$NativeHome = Resolve-OhosNativeHome -Hint $OhosNativeHome
$Cmake = Join-Path $NativeHome "build-tools\cmake\bin\cmake.exe"
$Ninja = Join-Path $NativeHome "build-tools\cmake\bin\ninja.exe"
$Toolchain = Join-Path $NativeHome "build\cmake\ohos.toolchain.cmake"

if ($Clean -and (Test-Path $BuildDir)) { Remove-Item -Recurse -Force $BuildDir }
New-Item -ItemType Directory -Force -Path $BuildDir | Out-Null

& $Cmake -S $CmakeDir -B $BuildDir -G Ninja `
    "-DCMAKE_MAKE_PROGRAM=$Ninja" `
    "-DCMAKE_TOOLCHAIN_FILE=$Toolchain" `
    "-DOHOS_ARCH=$Arch" `
    -DOHOS_PLATFORM=OHOS `
    -DOHOS_STL=c++_shared `
    -DCMAKE_BUILD_TYPE=Release
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
& $Cmake --build $BuildDir
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

$OutSo = Join-Path $Root "src\main\cpp\output\ohos\release\$Arch\libimageprocessing.so"
if (-not (Test-Path $OutSo)) { throw "Missing output: $OutSo" }
Write-Host "Build succeeded: $OutSo"
