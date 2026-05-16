# Architecture

```text
+---------------------------+
|  Houdini (Python SOP/VEX) |
+-------------+-------------+
              |  CSV / GEO per frame
              v
+---------------------------+
|  particle_sim (C++/CUDA)  |
|  CPU solver | GPU solver  |
|  Integrators | Benchmark  |
+-------------+-------------+
              |
              v
+---------------------------+
|  CUDA kernels (optional)  |
|  DeviceParticle buffers   |
+---------------------------+

+---------------------------+
|  ray_march (C++/OpenGL)   |
|  SDF -> sphere trace      |
|  shading -> PPM / viewer  |
+---------------------------+
```

## Modules

| Path | Role |
|------|------|
| `src/simulation/` | `Particle`, CPU/GPU solvers, integrators |
| `src/cuda/` | Device memory, `UpdateParticles` kernels |
| `src/benchmark/` | CPU/GPU timing, CSV report |
| `src/utils/` | Export, logging, timers |
| `src/rendering/` | SDF, camera, ray march, shading |
| `houdini/` | Import scripts and VEX |

## Data flow (simulation)

1. Initialize particles on host  
2. **CPU:** loop `CpuSolver::step`  
3. **GPU:** `upload` → kernel per frame → `download`  
4. Optional: `exportParticlesFrame` → `export/particles_NNNN.csv`

## Data flow (rendering)

1. Primary ray from `Camera`  
2. `sphereTrace` on `sceneSDF`  
3. `sdfGradient` → normal → `shadeSurface`  
4. Write `Framebuffer` → PPM or GLFW texture
