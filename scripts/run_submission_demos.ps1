# CUDA Coursework — reproduce all submission demos (run from repo root)
$ErrorActionPreference = "Stop"
$Root = Split-Path -Parent $PSScriptRoot
Set-Location $Root

$ParticleSim = Join-Path $Root "build\Release\particle_sim.exe"
$RayMarch = Join-Path $Root "build\Release\ray_march.exe"

if (-not (Test-Path $ParticleSim)) {
    Write-Host "Build first: cmake --build build --config Release"
    exit 1
}

Write-Host "`n=== 1. GPU validation (CPU vs GPU) ===" -ForegroundColor Cyan
& $ParticleSim --validate-gpu --particles 10000 --frames 60

Write-Host "`n=== 2. Benchmark suite ===" -ForegroundColor Cyan
& $ParticleSim --benchmark --frames 120

Write-Host "`n=== 3. Houdini export (48 frames, 5000 particles) ===" -ForegroundColor Cyan
& $ParticleSim --particles 5000 --frames 48 --export-dir export --export-format csv

Write-Host "`n=== 4. Ray march (PPM) ===" -ForegroundColor Cyan
New-Item -ItemType Directory -Force -Path render | Out-Null
if (Test-Path $RayMarch) {
    & $RayMarch --width 1280 --height 720 --output render/submission.ppm
    Write-Host "Open: $Root\render\submission.ppm (or run: ray_march.exe --view)"
} else {
    Write-Host "ray_march not built — run: cmake --build build --config Release --target ray_march"
}

Write-Host "`n=== Done ===" -ForegroundColor Green
Write-Host "Artifacts:"
Write-Host "  benchmarks/results.csv"
Write-Host "  export/particles_*.csv  -> load in Houdini (see README.md)"
Write-Host "  render/submission.ppm"
