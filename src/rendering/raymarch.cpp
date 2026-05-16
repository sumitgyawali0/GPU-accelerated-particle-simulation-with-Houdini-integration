#include "rendering/raymarch.h"

#include "rendering/sdf.h"

RayHit sphereTrace(const Ray& ray, const RayMarchSettings& settings)
{
    RayHit result;
    float traveled = 0.0f;

    for (int step = 0; step < settings.maxSteps; ++step)
    {
        const Vec3 p = ray.origin + ray.direction * traveled;
        const float dist = sceneSDF(p);

        if (dist < settings.surfaceThreshold)
        {
            result.hit = true;
            result.distance = traveled;
            result.position = p;
            result.steps = step + 1;
            return result;
        }

        traveled += dist;
        if (traveled > settings.maxDistance)
        {
            break;
        }
    }

    result.steps = settings.maxSteps;
    return result;
}

Vec3 sdfGradient(const Vec3& point, float epsilon)
{
    const Vec3 e{epsilon, 0.0f, 0.0f};
    const float dx = sceneSDF(point + Vec3{e.x, 0, 0}) - sceneSDF(point - Vec3{e.x, 0, 0});
    const float dy = sceneSDF(point + Vec3{0, e.y, 0}) - sceneSDF(point - Vec3{0, e.y, 0});
    const float dz = sceneSDF(point + Vec3{0, 0, e.z}) - sceneSDF(point - Vec3{0, 0, e.z});
    return normalize({dx, dy, dz});
}
