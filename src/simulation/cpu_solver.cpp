#include "simulation/cpu_solver.h"

void CpuSolver::applyForces(Particle& particle, const Vec3& gravity) const
{
    particle.force += gravity * particle.mass;
}

Vec3 CpuSolver::acceleration(const Particle& particle) const
{
    if (particle.mass <= 0.0f)
    {
        return {};
    }
    return particle.force / particle.mass;
}

void CpuSolver::step(std::vector<Particle>& particles, float dt, const Vec3& gravity,
                     IntegratorType integrator)
{
    for (auto& particle : particles)
    {
        particle.force = {};
        applyForces(particle, gravity);

        const Vec3 accel = acceleration(particle);

        switch (integrator)
        {
        case IntegratorType::Verlet:
            integrateVerlet(particle, accel, dt);
            break;
        case IntegratorType::Euler:
        default:
            integrateEuler(particle, accel, dt);
            break;
        }

        particle.force = {};
    }
}
