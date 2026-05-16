#pragma once

#include "math/vec3.h"

struct Ray
{
    Vec3 origin;
    Vec3 direction;
};

struct Camera
{
    Vec3 position{0.0f, 1.2f, 3.5f};
    Vec3 target{0.0f, 0.4f, 0.0f};
    Vec3 up{0.0f, 1.0f, 0.0f};
    float fovYDegrees = 45.0f;
    float aspect = 16.0f / 9.0f;

    Ray primaryRay(float u, float v) const;
};
