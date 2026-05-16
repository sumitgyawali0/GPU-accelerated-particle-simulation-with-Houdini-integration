#pragma once

#include <string>

#include "simulation/particle.h"

enum class IntegratorType
{
    Euler,
    Verlet
};

void integrateEuler(Particle& particle, const Vec3& acceleration, float dt);
void integrateVerlet(Particle& particle, const Vec3& acceleration, float dt);

const char* integratorName(IntegratorType type);
bool parseIntegrator(const std::string& value, IntegratorType& out);
