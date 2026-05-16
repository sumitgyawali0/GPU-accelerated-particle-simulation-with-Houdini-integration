#include "rendering/framebuffer.h"

#include <algorithm>
#include <fstream>

namespace
{

std::uint8_t toByte(float value)
{
    return static_cast<std::uint8_t>(std::clamp(value, 0.0f, 1.0f) * 255.0f + 0.5f);
}

} // namespace

Framebuffer::Framebuffer(int width, int height)
{
    resize(width, height);
}

void Framebuffer::resize(int width, int height)
{
    width_ = std::max(width, 1);
    height_ = std::max(height, 1);
    pixels_.assign(static_cast<size_t>(width_ * height_ * 3), 0);
}

void Framebuffer::clear(const Vec3& color)
{
    const std::uint8_t r = toByte(color.x);
    const std::uint8_t g = toByte(color.y);
    const std::uint8_t b = toByte(color.z);
    for (size_t i = 0; i < pixels_.size(); i += 3)
    {
        pixels_[i] = r;
        pixels_[i + 1] = g;
        pixels_[i + 2] = b;
    }
}

void Framebuffer::setPixel(int x, int y, const Vec3& color)
{
    if (x < 0 || y < 0 || x >= width_ || y >= height_)
    {
        return;
    }

    const size_t index = static_cast<size_t>((y * width_ + x) * 3);
    pixels_[index] = toByte(color.x);
    pixels_[index + 1] = toByte(color.y);
    pixels_[index + 2] = toByte(color.z);
}

bool Framebuffer::savePpm(const std::string& path) const
{
    std::ofstream file(path, std::ios::binary);
    if (!file)
    {
        return false;
    }

    file << "P6\n" << width_ << ' ' << height_ << "\n255\n";
    file.write(reinterpret_cast<const char*>(pixels_.data()),
               static_cast<std::streamsize>(pixels_.size()));
    return static_cast<bool>(file);
}
