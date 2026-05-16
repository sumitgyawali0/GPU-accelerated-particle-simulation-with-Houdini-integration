#include "cuda/kernels.h"

#include "cuda/cuda_utils.h"

namespace
{

constexpr int kBlockSize = 256;

__global__ void updateParticlesEuler(DeviceParticle* particles, int count, float dt, float gravityY)
{
    const int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx >= count)
    {
        return;
    }

    DeviceParticle& p = particles[idx];
    const float invMass = (p.mass > 0.0f) ? (1.0f / p.mass) : 0.0f;
    const float ay = gravityY;

    p.vx += 0.0f * invMass * dt;
    p.vy += ay * dt;
    p.vz += 0.0f * invMass * dt;

    p.px += p.vx * dt;
    p.py += p.vy * dt;
    p.pz += p.vz * dt;
}

__global__ void updateParticlesVerlet(DeviceParticle* particles, int count, float dt,
                                      float gravityY)
{
    const int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx >= count)
    {
        return;
    }

    DeviceParticle& p = particles[idx];
    const float invMass = (p.mass > 0.0f) ? (1.0f / p.mass) : 0.0f;
    const float ay = gravityY * invMass;
    const float dt2 = dt * dt;

    const float cx = p.px;
    const float cy = p.py;
    const float cz = p.pz;

    const float dx = cx - p.prevPx;
    const float dy = cy - p.prevPy;
    const float dz = cz - p.prevPz;

    const float nx = cx + dx + 0.0f * dt2;
    const float ny = cy + dy + ay * dt2;
    const float nz = cz + dz + 0.0f * dt2;

    p.vx = (nx - p.prevPx) / dt;
    p.vy = (ny - p.prevPy) / dt;
    p.vz = (nz - p.prevPz) / dt;

    p.prevPx = cx;
    p.prevPy = cy;
    p.prevPz = cz;

    p.px = nx;
    p.py = ny;
    p.pz = nz;
}

} // namespace

void launchUpdateParticles(DeviceParticle* deviceParticles, int count, float dt, float gravityY,
                           IntegratorType integrator, bool syncAfterLaunch)
{
    if (count <= 0)
    {
        return;
    }

    const int blocks = blockCount(count, kBlockSize);

    switch (integrator)
    {
    case IntegratorType::Verlet:
        updateParticlesVerlet<<<blocks, kBlockSize>>>(deviceParticles, count, dt, gravityY);
        break;
    case IntegratorType::Euler:
    default:
        updateParticlesEuler<<<blocks, kBlockSize>>>(deviceParticles, count, dt, gravityY);
        break;
    }

    CUDA_CHECK(cudaGetLastError());
    if (syncAfterLaunch)
    {
        CUDA_CHECK(cudaDeviceSynchronize());
    }
}
