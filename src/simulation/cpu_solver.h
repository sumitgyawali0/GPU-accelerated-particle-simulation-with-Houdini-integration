#pragma once

#include <vector>

#include "simulation/integrators.h"
#include "simulation/particle.h"

class CpuSolver
{
public:
    void step(std::vector<Particle>& particles, float dt, const Vec3& gravity,
              IntegratorType integrator = IntegratorType::Euler);

private:
    void applyForces(Particle& particle, const Vec3& gravity) const;
    Vec3 acceleration(const Particle& particle) const;
};
