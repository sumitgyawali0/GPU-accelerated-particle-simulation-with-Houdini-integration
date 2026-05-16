#include "rendering/sdf.h"

#include <algorithm>
#include <cmath>

float sdSphere(const Vec3& p, const Vec3& center, float radius)
{
    return length(p - center) - radius;
}

float sdBox(const Vec3& p, const Vec3& center, const Vec3& halfExtents)
{
    const Vec3 q = {std::abs(p.x - center.x) - halfExtents.x,
                    std::abs(p.y - center.y) - halfExtents.y,
                    std::abs(p.z - center.z) - halfExtents.z};
    const float outside =
        length({std::max(q.x, 0.0f), std::max(q.y, 0.0f), std::max(q.z, 0.0f)});
    const float inside = std::min(std::max(q.x, std::max(q.y, q.z)), 0.0f);
    return outside + inside;
}

float sdTorus(const Vec3& p, const Vec3& center, float majorRadius, float minorRadius)
{
    const Vec3 q = p - center;
    const float xz = std::sqrt(q.x * q.x + q.z * q.z) - majorRadius;
    return std::sqrt(xz * xz + q.y * q.y) - minorRadius;
}

float opUnion(float a, float b)
{
    return std::min(a, b);
}

float opSmoothUnion(float a, float b, float k)
{
    const float h = std::max(k - std::abs(a - b), 0.0f) / k;
    return std::min(a, b) - h * h * k * 0.25f;
}

float sceneSDF(const Vec3& p)
{
    const float sphere = sdSphere(p, {0.0f, 0.55f, 0.0f}, 0.45f);
    const float box = sdBox(p, {-0.55f, 0.3f, -0.2f}, {0.28f, 0.28f, 0.28f});
    const float torus = sdTorus(p, {0.55f, 0.35f, -0.35f}, 0.28f, 0.1f);
    const float ground = sdBox(p, {0.0f, -0.05f, 0.0f}, {4.0f, 0.05f, 4.0f});

    float scene = opSmoothUnion(sphere, box, 0.15f);
    scene = opSmoothUnion(scene, torus, 0.12f);
    scene = opUnion(scene, ground);
    return scene;
}
