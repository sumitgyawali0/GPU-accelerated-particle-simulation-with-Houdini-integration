# CUDA Coursework — Particle Simulation & SDF Rendering

GPU-accelerated particle simulation with Houdini integration and a procedural SDF ray marcher (C++17, CUDA, OpenGL).

Further detail: [docs/SUBMISSION.md](docs/SUBMISSION.md) · [docs/architecture.md](docs/architecture.md) · [docs/benchmarks.md](docs/benchmarks.md)

---

## Requirements

- CMake 3.20+, MSVC 2022 (Windows) or GCC 11+
- CUDA Toolkit 11.8+ (optional; auto-enabled when found)
- Houdini 20+ (viewport only; simulation runs standalone)

---

## Build

```powershell
mkdir build
cd build
cmake -DBUILD_CUDA=ON ..
cmake --build . --config Release
cmake --build . --config Release --target ray_march
cd ..
```

CPU-only (no CUDA): `cmake -DBUILD_CUDA=OFF ..`

### Reproduce all submission demos

```powershell
.\scripts\run_submission_demos.ps1
```

---

## Features

| Sprint | Deliverable |
|--------|-------------|
| 1–2 | CPU + CUDA GPU particle solvers |
| 3–4 | Euler / Verlet, `--benchmark`, `--validate-gpu` |
| 5 | Per-frame CSV/GEO export, Houdini Python SOP + VEX |
| 6 | `ray_march` — SDF scene, sphere tracing, GLFW viewer |

---

## Simulation (`particle_sim`)

```powershell
.\build\Release\particle_sim.exe --particles 100000 --frames 120
.\build\Release\particle_sim.exe --gpu --particles 1000000 --frames 120
.\build\Release\particle_sim.exe --validate-gpu --particles 10000 --frames 60
.\build\Release\particle_sim.exe --benchmark --frames 120
```

### CLI

| Flag | Description |
|------|-------------|
| `--particles N` | Particle count |
| `--frames N` | Simulation frames |
| `--dt SECONDS` | Timestep |
| `--integrator euler\|verlet` | Integration method |
| `--gpu` | CUDA solver |
| `--validate-gpu` | CPU vs GPU correctness check |
| `--benchmark` | 10K / 100K / 1M benchmark suite |
| `--export-dir PATH` | Export frame sequence |
| `--export-format csv\|geo\|json` | Export format |

---

## Houdini integration

### 1. Export frames (from repo root)

```powershell
.\build\Release\particle_sim.exe --particles 5000 --frames 48 --export-dir export --export-format csv
```

Creates `export/particles_0000.csv` … `particles_0048.csv` (initial state + one file per step).

Formats: `csv` (Python SOP), `geo` (File SOP), `json` (debugging).

### 2. Load in Houdini (Python SOP)

1. Open Houdini 20+.
2. Create a **Python SOP**.
3. Paste the **entire** script from `houdini/python/particle_csv_sop.py`.
4. Edit `DEFAULT_EXPORT_DIR` at the top of the script, e.g. `B:/blogs/CUDA_coursework/export` (use forward slashes).
   - Optional: add a spare **String** parameter named `export_dir` on the node to override the path.
5. Set the playbar to **0–48** (match your `--frames` export).
6. Scrub the timeline — points appear with `@P`, `@v`, `@Cd`, `@mass`.

**Tips:** Paste the full file (do not skip the imports). If the timeline goes past your export, the script holds the last available frame. Re-export with more `--frames` if needed.

### 3. Load with File SOP (.geo)

```powershell
.\build\Release\particle_sim.exe --export-dir export --export-format geo --frames 48
```

1. Add a **File SOP**.
2. File: path to `export/particles_$F4.geo` (adjust for your machine).
3. Enable **Load as Points**.

### 4. VEX visualization

1. Add a **Point Wrangle** after the loader.
2. Paste `houdini/vex/color_by_velocity.vfl` (velocity heatmap + `pscale`).
3. Optional: **Trail SOP** → **Attribute Wrangle** with `houdini/vex/trail_visualization.vfl`.

### Attribute mapping

| Simulation | Houdini |
|------------|---------|
| position   | `@P`    |
| velocity   | `@v`    |
| color      | `@Cd`   |
| mass       | `@mass` |

### Suggested network

```text
[Python SOP or File SOP] → [Point Wrangle: color_by_velocity] → [Trail SOP] → [Render]
```

---

## Ray marching (`ray_march`)

```powershell
.\build\Release\ray_march.exe --view --width 960 --height 540
.\build\Release\ray_march.exe --output render/output.ppm --width 1280 --height 720
```

| Flag | Description |
|------|-------------|
| `--view` | Interactive OpenGL window |
| `--output PATH` | Save PPM image |
| `--width` / `--height` | Resolution |
| `--no-save` | Viewer only, skip PPM |

Open PPM files with IrfanView, GIMP, or use `--view` to display directly.

---

## Project layout

```text
src/simulation/   Particle, CPU/GPU solvers, integrators
src/cuda/         Kernels, device memory
src/rendering/    SDF ray marcher
src/benchmark/    Performance suite
houdini/
  python/         particle_csv_sop.py
  vex/            color_by_velocity.vfl, trail_visualization.vfl
docs/             Submission guide, architecture, benchmarks
scripts/          run_submission_demos.ps1
benchmarks/       results.csv (after --benchmark)
techspec.md       Full technical specification
```

---

## Submission checklist

- [ ] Build succeeds (`particle_sim` + `ray_march`)
- [ ] `--validate-gpu` passes
- [ ] `benchmarks/results.csv` included
- [ ] Houdini screenshot (particles + VEX color)
- [ ] Ray march screenshot (`--view` or `render/submission.ppm`)
- [ ] Zip excludes `build/`, `export/`, `render/` unless samples required

See [docs/SUBMISSION.md](docs/SUBMISSION.md) for report outline and full demo list.
