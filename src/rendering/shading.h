#pragma once

#include "math/vec3.h"
#include "rendering/camera.h"
#include "rendering/raymarch.h"

struct ShadingSettings
{
    Vec3 lightPosition{2.0f, 4.0f, 2.0f};
    Vec3 lightColor{1.0f, 0.98f, 0.95f};
    Vec3 skyColorTop{0.15f, 0.25f, 0.45f};
    Vec3 skyColorBottom{0.02f, 0.03f, 0.08f};
    Vec3 objectColor{0.85f, 0.35f, 0.25f};
};

Vec3 shadeBackground(const Ray& ray, const ShadingSettings& settings);
Vec3 shadeSurface(const Ray& ray, const RayHit& hit, const ShadingSettings& settings);
