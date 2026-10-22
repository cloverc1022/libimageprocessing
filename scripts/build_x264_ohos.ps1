param(
    [string]$OhosNativeHome = $env:OHOS_NATIVE_HOME,
    [ValidateSet("arm64-v8a")]
    [string]$Arch = "arm64-v8a",
    [string]$BashPath = "C:\Program Files\Git\bin\bash.exe"
)

$ErrorActionPreference = "Stop"
$Root = Split-Path -Parent $PSScriptRoot
$Cpp = Join-Path $Root "src\main\cpp"
$DestLib = Join-Path $Cpp "x264\$Arch\lib\libx264.a"
$DestInc = Join-Path $Cpp "x264\$Arch\include"
$Work = Join-Path $Root ".build\x264-ohos-$Arch"
$SrcDir = Join-Path $Work "x264"

function Resolve-OhosNativeHome {
    param([string]$Hint)
    $candidates = @()
    if ($Hint) { $candidates += $Hint }
    $candidates += "D:\StudioProjects\Http2p2p_API\.ohos-sdk\native"
    foreach ($path in $candidates) {
        if (Test-Path (Join-Path $path "build\cmake\ohos.toolchain.cmake")) {
            return (Resolve-Path $path).Path
        }
    }
    throw "OpenHarmony Native SDK not found."
}

if (-not (Test-Path $BashPath)) { throw "bash not found at $BashPath" }

$NativeHome = Resolve-OhosNativeHome -Hint $OhosNativeHome
$Clang = Join-Path $NativeHome "llvm\bin\clang.exe"
$Sysroot = Join-Path $NativeHome "sysroot"
$Target = "aarch64-linux-ohos"

New-Item -ItemType Directory -Force -Path $Work | Out-Null
if (-not (Test-Path (Join-Path $SrcDir ".git"))) {
    if (Test-Path $SrcDir) { Remove-Item -Recurse -Force $SrcDir }
    git clone --depth 1 --branch stable https://code.videolan.org/videolan/x264.git $SrcDir
    if ($LASTEXITCODE -ne 0) {
        throw "git clone x264 failed"
    }
}

$SrcUnix = ($SrcDir -replace '\\', '/')
$WorkUnix = ($Work -replace '\\', '/')
$ClangUnix = ($Clang -replace '\\', '/')
$ArUnix = ((Join-Path $NativeHome "llvm\bin\llvm-ar.exe") -replace '\\', '/')
$StripUnix = ((Join-Path $NativeHome "llvm\bin\llvm-strip.exe") -replace '\\', '/')
$RanlibUnix = ((Join-Path $NativeHome "llvm\bin\llvm-ranlib.exe") -replace '\\', '/')
$Flags = "--target=$Target --sysroot=$Sysroot -fPIC"
$ScriptSh = Join-Path $Root ".build\build_x264_ohos.sh"
if (-not (Test-Path $ScriptSh)) { throw "Missing $ScriptSh" }
$env:OHOS_NATIVE_HOME = $NativeHome
& $BashPath $ScriptSh $SrcDir $Work $NativeHome
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

New-Item -ItemType Directory -Force -Path (Split-Path $DestLib) | Out-Null
New-Item -ItemType Directory -Force -Path $DestInc | Out-Null
Copy-Item (Join-Path $Work "out\lib\libx264.a") $DestLib -Force
Copy-Item (Join-Path $Work "out\include\x264.h") $DestInc -Force
Copy-Item (Join-Path $Work "out\include\x264_config.h") $DestInc -Force
Write-Host "Installed OHOS x264 to $DestLib"
