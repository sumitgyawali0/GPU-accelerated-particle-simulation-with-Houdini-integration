#include <cstdlib>
#include <iostream>
#include <string>

#include "rendering/framebuffer.h"
#include "rendering/gl_viewer.h"
#include "rendering/renderer.h"
#include "utils/logger.h"
#include "utils/timer.h"

namespace
{

void printUsage()
{
    std::cout << "Usage: ray_march [options]\n"
              << "  --width <N>       Image width (default: 1280)\n"
              << "  --height <N>      Image height (default: 720)\n"
              << "  --output <path>   Save PPM image (default: render/output.ppm)\n"
              << "  --view            Open interactive OpenGL window (requires GLFW)\n"
              << "  --no-save         Skip writing PPM file\n";
}

} // namespace

int main(int argc, char* argv[])
{
    RenderSettings settings;
    std::string outputPath = "render/output.ppm";
    bool saveImage = true;
    bool openViewer = false;

    for (int i = 1; i < argc; ++i)
    {
        const std::string arg(argv[i]);
        if (arg == "--help" || arg == "-h")
        {
            printUsage();
            return 0;
        }
        if (arg == "--view")
        {
            openViewer = true;
            continue;
        }
        if (arg == "--no-save")
        {
            saveImage = false;
            continue;
        }
        if (i + 1 >= argc)
        {
            Logger::error("Missing value for " + arg);
            printUsage();
            return 1;
        }

        if (arg == "--width")
        {
            settings.width = std::atoi(argv[++i]);
        }
        else if (arg == "--height")
        {
            settings.height = std::atoi(argv[++i]);
        }
        else if (arg == "--output")
        {
            outputPath = argv[++i];
        }
        else
        {
            Logger::error("Unknown argument: " + arg);
            printUsage();
            return 1;
        }
    }

    Logger::info("Sprint 6 — SDF ray marching");
    Logger::info("Resolution: " + std::to_string(settings.width) + "x" +
                 std::to_string(settings.height));

    Framebuffer framebuffer;
    Timer timer;
    timer.start();
    renderScene(framebuffer, settings);
    const double renderSeconds = timer.elapsedSeconds();

    Logger::info("Render time: " + std::to_string(renderSeconds) + " s");

    if (saveImage)
    {
        if (framebuffer.savePpm(outputPath))
        {
            Logger::info("Saved " + outputPath);
        }
        else
        {
            Logger::error("Failed to save " + outputPath);
            return 1;
        }
    }

#ifdef RAYMARCH_USE_GLFW
    if (openViewer)
    {
        if (!runInteractiveViewer(framebuffer, settings))
        {
            Logger::error("Failed to open viewer");
            return 1;
        }
    }
#else
    if (openViewer)
    {
        Logger::warn("Built without GLFW — use PPM output only (omit --view)");
    }
#endif

    return 0;
}
