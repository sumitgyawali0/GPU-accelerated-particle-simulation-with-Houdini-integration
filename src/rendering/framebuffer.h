#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "math/vec3.h"

class Framebuffer
{
public:
    Framebuffer() = default;
    Framebuffer(int width, int height);

    void resize(int width, int height);
    void setPixel(int x, int y, const Vec3& color);
    void clear(const Vec3& color);

    int width() const { return width_; }
    int height() const { return height_; }
    const std::vector<std::uint8_t>& pixels() const { return pixels_; }

    bool savePpm(const std::string& path) const;

private:
    int width_ = 0;
    int height_ = 0;
    std::vector<std::uint8_t> pixels_;
};
