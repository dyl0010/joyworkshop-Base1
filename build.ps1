param(
    [ValidateSet('Debug', 'Release', 'RelWithDebInfo', 'MinSizeRel')]
    [string]$Config = 'Debug',

    [switch]$Clean,
    [switch]$NoRun,
    [string]$SDL3Root = ''
)

$ErrorActionPreference = 'Stop'

$Root = Split-Path -Parent $MyInvocation.MyCommand.Path
$BuildDir = Join-Path $Root 'build'
$ExeName = 'sdl3_hello.exe'

function Write-Step {
    param([string]$Message)
    Write-Host "`n==> $Message" -ForegroundColor Cyan
}

function Resolve-Executable {
    param([string]$Directory, [string]$Name)

    $candidate = Get-ChildItem -Path $Directory -Recurse -Filter $Name -File -ErrorAction SilentlyContinue |
        Sort-Object LastWriteTime -Descending |
        Select-Object -First 1

    if ($null -eq $candidate) {
        throw "Build succeeded, but '$Name' was not found under '$Directory'."
    }

    return $candidate.FullName
}

function Invoke-Native {
    param(
        [string]$FilePath,
        [string[]]$Arguments
    )

    & $FilePath @Arguments
    if ($LASTEXITCODE -ne 0) {
        throw "$FilePath exited with code $LASTEXITCODE."
    }
}

try {
    if ($Clean -and (Test-Path $BuildDir)) {
        Write-Step "Removing $BuildDir"
        Remove-Item -Recurse -Force $BuildDir
    }

    $configureArgs = @('-S', $Root, '-B', $BuildDir)

    if ($SDL3Root -ne '') {
        $configureArgs += "-DSDL3_ROOT=$SDL3Root"
    }

    Write-Step 'Configuring CMake project'
    Invoke-Native -FilePath 'cmake' -Arguments $configureArgs

    Write-Step "Building $Config"
    Invoke-Native -FilePath 'cmake' -Arguments @('--build', $BuildDir, '--config', $Config, '--parallel')

    if (-not $NoRun) {
        $exe = Resolve-Executable -Directory $BuildDir -Name $ExeName
        Write-Step "Running $exe"
        & $exe
    }
}
catch {
    Write-Host "`nBuild failed: $($_.Exception.Message)" -ForegroundColor Red
    Write-Host "`nSDL3 offline setup options:" -ForegroundColor Yellow
    Write-Host "  1. Download an SDL3 development package manually."
    Write-Host "  2. Extract it to: $Root\external\SDL3"
    Write-Host '  3. Or run: .\build.ps1 -SDL3Root C:\path\to\SDL3'
    Write-Host "`nThe SDL3 folder should contain SDL3Config.cmake under a cmake or lib\cmake\SDL3 subdirectory."
    exit 1
}
