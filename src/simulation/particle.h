#pragma once

#include "math/vec3.h"

struct Particle
{
    Vec3 position;
    Vec3 velocity;
    float mass = 1.0f;

    Vec3 force;
    Vec3 previousPosition;
    Vec3 color{1.0f, 1.0f, 1.0f};
    float lifetime = -1.0f;
};
