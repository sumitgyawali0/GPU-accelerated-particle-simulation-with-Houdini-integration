#include "rendering/camera.h"

#include <cmath>

Ray Camera::primaryRay(float u, float v) const
{
    const Vec3 forward = normalize(target - position);
    const Vec3 right = normalize(cross(forward, up));
    const Vec3 trueUp = cross(right, forward);

    const float fovY = fovYDegrees * 3.14159265f / 180.0f;
    const float halfH = std::tan(fovY * 0.5f);
    const float halfW = halfH * aspect;

    const float x = (2.0f * u - 1.0f) * halfW;
    const float y = (1.0f - 2.0f * v) * halfH;

    const Vec3 direction = normalize(forward + right * x + trueUp * y);
    return {position, direction};
}
