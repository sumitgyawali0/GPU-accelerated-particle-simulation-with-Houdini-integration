# Coursework Submission Guide

## What to submit

| Item | Location | Notes |
|------|----------|--------|
| Source code | Full repo (or zip) | Exclude `build/`, `export/`, `render/` |
| Technical spec | `techspec.md` | Project definition |
| Benchmark results | `benchmarks/results.csv` | CPU vs GPU timings |
| Houdini setup | `houdini/` | Python SOP + VEX |
| Build instructions | `README.md` | Build, Houdini, demos |
| Screenshots | Your report | See checklist below |

## Reproduce everything (one script)

From repo root (after building Release):

```powershell
.\scripts\run_submission_demos.ps1
```

Or build manually:

```powershell
mkdir build; cd build
cmake -DBUILD_CUDA=ON ..
cmake --build . --config Release
cmake --build . --config Release --target ray_march
cd ..
.\scripts\run_submission_demos.ps1
```

## Demo commands (for report / video)

### 1. GPU correctness

```powershell
.\build\Release\particle_sim.exe --validate-gpu --particles 10000 --frames 60
```

Expected: `GPU validation passed`, max position error &lt; 1e-3.

### 2. Performance

```powershell
.\build\Release\particle_sim.exe --benchmark --frames 120
```

Output: `benchmarks/results.csv`

### 3. CPU vs GPU run

```powershell
.\build\Release\particle_sim.exe --particles 500000 --frames 120
.\build\Release\particle_sim.exe --gpu --particles 500000 --frames 120
```

### 4. Houdini pipeline

```powershell
.\build\Release\particle_sim.exe --particles 5000 --frames 48 --export-dir export --export-format csv
```

In Houdini: Python SOP → paste `houdini/python/particle_csv_sop.py` → timeline 0–48.  
See **Houdini integration** in root `README.md`.

### 5. Ray marching (Sprint 6)

```powershell
.\build\Release\ray_march.exe --view --width 960 --height 540
.\build\Release\ray_march.exe --output render/submission.ppm --width 1280 --height 720
```

## Screenshot checklist

- [ ] Houdini viewport: particles with velocity color (Point Wrangle + `color_by_velocity.vfl`)
- [ ] Optional: Trail SOP
- [ ] `ray_march` window or `render/submission.ppm`
- [ ] Terminal: `--validate-gpu` passed
- [ ] Table or chart from `benchmarks/results.csv`

## Report outline (suggested)

1. **Introduction** — GPU particles + Houdini + SDF rendering goals  
2. **Architecture** — see `docs/architecture.md`  
3. **CPU implementation** — Euler/Verlet, data layout  
4. **CUDA implementation** — kernels, memory transfer, validation  
5. **Benchmarks** — see `docs/benchmarks.md`  
6. **Houdini integration** — CSV export, attributes `@P` `@v` `@Cd`  
7. **Ray marching** — SDF primitives, sphere tracing, shading  
8. **Conclusion** — speedups, limitations, future work (Sprint 7)

## Success criteria (from spec)

| Criterion | Evidence |
|-----------|----------|
| GPU faster than CPU | `benchmarks/results.csv` (up to ~50× kernel at 1M particles) |
| Stable runtime | `--validate-gpu` pass |
| Houdini visualization | `export/` + Python SOP |
| Extensible rendering | `src/rendering/`, `ray_march` |

## Environment (tested)

- Windows 10, MSVC 2022, CUDA 12.3  
- CMake 3.20+, C++17  
- Houdini 20+ (for viewport; export is CSV-based)
