#pragma once

#include "math/vec3.h"

float sdSphere(const Vec3& p, const Vec3& center, float radius);
float sdBox(const Vec3& p, const Vec3& center, const Vec3& halfExtents);
float sdTorus(const Vec3& p, const Vec3& center, float majorRadius, float minorRadius);

float opUnion(float a, float b);
float opSmoothUnion(float a, float b, float k);

float sceneSDF(const Vec3& p);
