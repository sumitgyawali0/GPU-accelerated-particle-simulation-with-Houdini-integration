#include "rendering/shading.h"

#include <algorithm>
#include <cmath>

#include "rendering/raymarch.h"
#include "rendering/sdf.h"

Vec3 shadeBackground(const Ray& ray, const ShadingSettings& settings)
{
    const float t = 0.5f * (ray.direction.y + 1.0f);
    return settings.skyColorBottom * (1.0f - t) + settings.skyColorTop * t;
}

Vec3 shadeSurface(const Ray& ray, const RayHit& hit, const ShadingSettings& settings)
{
    const Vec3 normal = sdfGradient(hit.position);
    const Vec3 lightDir = normalize(settings.lightPosition - hit.position);

    const float diffuse = std::max(dot(normal, lightDir), 0.0f);
    const Vec3 half = normalize(lightDir - ray.direction);
    const float spec = std::pow(std::max(dot(normal, half), 0.0f), 48.0f);

    const float ao = std::clamp(1.0f - 0.15f * static_cast<float>(hit.steps), 0.4f, 1.0f);

    Vec3 base = settings.objectColor;
    const float sphere = sdSphere(hit.position, {0.0f, 0.55f, 0.0f}, 0.45f);
    const float box = sdBox(hit.position, {-0.55f, 0.3f, -0.2f}, {0.28f, 0.28f, 0.28f});
    if (box < sphere)
    {
        base = {0.35f, 0.7f, 0.9f};
    }
    if (sdTorus(hit.position, {0.55f, 0.35f, -0.35f}, 0.28f, 0.1f) < std::min(sphere, box) + 0.02f)
    {
        base = {0.9f, 0.75f, 0.3f};
    }

    Vec3 color = base * (0.12f + 0.88f * diffuse * ao);
    color += settings.lightColor * spec * 0.35f;
    return color;
}
