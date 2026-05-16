#include "simulation/integrators.h"

#include <string>

void integrateEuler(Particle& particle, const Vec3& acceleration, float dt)
{
    particle.velocity += acceleration * dt;
    particle.position += particle.velocity * dt;
}

void integrateVerlet(Particle& particle, const Vec3& acceleration, float dt)
{
    const Vec3 current = particle.position;
    const Vec3 delta = current - particle.previousPosition;
    const float dt2 = dt * dt;

    particle.position = current + delta + acceleration * dt2;
    particle.velocity = (particle.position - particle.previousPosition) / dt;
    particle.previousPosition = current;
}

const char* integratorName(IntegratorType type)
{
    switch (type)
    {
    case IntegratorType::Verlet:
        return "verlet";
    case IntegratorType::Euler:
    default:
        return "euler";
    }
}

bool parseIntegrator(const std::string& value, IntegratorType& out)
{
    if (value == "euler" || value == "Euler")
    {
        out = IntegratorType::Euler;
        return true;
    }
    if (value == "verlet" || value == "Verlet")
    {
        out = IntegratorType::Verlet;
        return true;
    }
    return false;
}
