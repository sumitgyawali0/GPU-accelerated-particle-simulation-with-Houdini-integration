#pragma once

#include <cstddef>
#include <memory>
#include <vector>

#include "simulation/integrators.h"
#include "simulation/particle.h"

class GpuSolver
{
public:
    GpuSolver();
    ~GpuSolver();

    GpuSolver(const GpuSolver&) = delete;
    GpuSolver& operator=(const GpuSolver&) = delete;

    void upload(const std::vector<Particle>& hostParticles);
    void step(float dt, const Vec3& gravity, IntegratorType integrator, bool syncAfterLaunch = true);
    void download(std::vector<Particle>& hostParticles);

    std::size_t particleCount() const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};
