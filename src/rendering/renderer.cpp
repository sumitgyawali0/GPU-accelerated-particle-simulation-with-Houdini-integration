#include "rendering/renderer.h"

void renderScene(Framebuffer& framebuffer, const RenderSettings& settings)
{
    framebuffer.resize(settings.width, settings.height);
    framebuffer.clear(settings.shading.skyColorBottom);

    Camera camera = settings.camera;
    camera.aspect = static_cast<float>(settings.width) / static_cast<float>(settings.height);

    for (int y = 0; y < settings.height; ++y)
    {
        for (int x = 0; x < settings.width; ++x)
        {
            const float u = (static_cast<float>(x) + 0.5f) / static_cast<float>(settings.width);
            const float v = (static_cast<float>(y) + 0.5f) / static_cast<float>(settings.height);

            const Ray ray = camera.primaryRay(u, v);
            const RayHit hit = sphereTrace(ray, settings.rayMarch);

            const Vec3 color =
                hit.hit ? shadeSurface(ray, hit, settings.shading) : shadeBackground(ray, settings.shading);
            framebuffer.setPixel(x, y, color);
        }
    }
}
