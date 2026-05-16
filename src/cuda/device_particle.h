#pragma once

// GPU-friendly layout (AoS). Converted to/from host Particle in gpu_solver.

struct DeviceParticle
{
    float px, py, pz;
    float vx, vy, vz;
    float mass;
    float prevPx, prevPy, prevPz;
};
