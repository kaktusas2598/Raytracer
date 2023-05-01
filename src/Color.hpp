#include "Vec3.hpp"

#include <iostream>

void writeColorPPM(std::ostream &out, Color pixelColor) {
    // Write the translated [0,255] value of each color component.
    out << static_cast<int>(255.999 * pixelColor.x()) << ' '
        << static_cast<int>(255.999 * pixelColor.y()) << ' '
        << static_cast<int>(255.999 * pixelColor.z()) << '\n';
}

void writeColorToBuffer(unsigned char* buffer, int x, int y, int width, Color pixelColor) {
    // Write the translated [0,255] value of each color component.
    buffer[(x + y * (int)width) * 4] = static_cast<int>(255.999 * pixelColor.x());
    buffer[(x + y * (int)width) * 4 + 1] = static_cast<int>(255.999 * pixelColor.y());
    buffer[(x + y * (int)width) * 4 + 2] = static_cast<int>(255.999 * pixelColor.z());
    buffer[(x + y * (int)width) * 4 + 3] = 255;
}
