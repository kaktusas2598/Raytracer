#pragma once
#include "Common.hpp"

void writeColorPPM(std::ostream &out, Color pixelColor, int samplesPerPixel = 1);
void writeColorToBuffer(unsigned char* buffer, int x, int y, int width, Color pixelColor, int samplesPerPixel = 1);
