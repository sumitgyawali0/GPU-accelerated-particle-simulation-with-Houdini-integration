#pragma once

#include "rendering/camera.h"
#include "rendering/framebuffer.h"
#include "rendering/raymarch.h"
#include "rendering/shading.h"

struct RenderSettings
{
    int width = 1280;
    int height = 720;
    Camera camera;
    RayMarchSettings rayMarch;
    ShadingSettings shading;
};

void renderScene(Framebuffer& framebuffer, const RenderSettings& settings);
