#pragma once

#include "math/vec3.h"
#include "rendering/camera.h"

struct RayHit
{
    bool hit = false;
    float distance = 0.0f;
    Vec3 position;
    int steps = 0;
};

struct RayMarchSettings
{
    float maxDistance = 20.0f;
    float surfaceThreshold = 0.001f;
    int maxSteps = 128;
};

RayHit sphereTrace(const Ray& ray, const RayMarchSettings& settings);
Vec3 sdfGradient(const Vec3& point, float epsilon = 0.0005f);
