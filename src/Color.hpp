#include "Common.hpp"

#include <iostream>

void writeColorPPM(std::ostream &out, Color pixelColor, int samplesPerPixel = 1) {
    auto r = pixelColor.x();
    auto g = pixelColor.y();
    auto b = pixelColor.z();

    // Divied by number of samples
    double scale = 1.0 / samplesPerPixel;
    r *= scale;
    g *= scale;
    b *= scale;

    // Write the translated [0,255] value of each color component.
    out << static_cast<int>(256 * clamp(r, 0.0, 0.999)) << ' '
        << static_cast<int>(256 * clamp(g, 0.0, 0.999)) << ' '
        << static_cast<int>(256 * clamp(b, 0.0, 0.999)) << '\n';
}

void writeColorToBuffer(unsigned char* buffer, int x, int y, int width, Color pixelColor, int samplesPerPixel = 1) {
    auto r = pixelColor.x();
    auto g = pixelColor.y();
    auto b = pixelColor.z();

    // Divied by number of samples
    double scale = 1.0 / samplesPerPixel;
    r *= scale;
    g *= scale;
    b *= scale;

    // Write the translated [0,255] value of each color component.
    buffer[(x + y * (int)width) * 4] = static_cast<int>(256 * clamp(r, 0.0, 0.999));
    buffer[(x + y * (int)width) * 4 + 1] = static_cast<int>(256 * clamp(g, 0.0, 0.999));
    buffer[(x + y * (int)width) * 4 + 2] = static_cast<int>(256 * clamp(b, 0.0, 0.999));
    buffer[(x + y * (int)width) * 4 + 3] = 255;
}
