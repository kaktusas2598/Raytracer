#include "Renderer.hpp"

#include <iostream>
#include <fstream>

void Renderer::raytraceWorld(const Hittable& world, uint32_t width, uint32_t height) {
    onResize(width, height);

    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            Color pixelColor(0,0,0);
            for (int s = 0; s < samplesPerPixel; ++s) {
                auto u = (i + randomDouble()) / (width - 1);
                auto v = (j + randomDouble()) / (height - 1);
                Ray r = camera.getRay(u, v);
                pixelColor += rayColor(r, world, maxDepth);
            }
            writeColorToBuffer(buffer, i, j, width, pixelColor, samplesPerPixel);
        }
    }

    image->setData(buffer);
    image->init(width, height);

}

void Renderer::exportRaytracedPPM(const Hittable& world, uint32_t width, uint32_t height) {
    std::ofstream outFile;
    outFile.open("image.ppm");
    outFile << "P3\n" << (int)width << ' ' << (int)height << "\n255\n";

    for (int j = height-1; j >= 0; --j) {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < width; ++i) {
            Color pixelColor(0,0,0);
            for (int s = 0; s < samplesPerPixel; ++s) {
                auto u = (i + randomDouble()) / (width-1);
                auto v = (j + randomDouble()) / (height-1);
                Ray r = camera.getRay(u, v);
                pixelColor += Renderer::rayColor(r, world, maxDepth);
            }
            writeColorPPM(outFile, pixelColor, samplesPerPixel);
        }
    }
    std::cerr << "Done.\n";
    outFile.close();
}

void Renderer::generateCheckerTexture(uint32_t width, uint32_t height) {
    onResize(width, height);

    int c;
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            // Generate checkers
            c = ((((j&0x8)==0)^((i&0x8))==0))*255;

            Color pixelColor(randomDouble(), c, c);
            writeColorToBuffer(buffer, i, j, width, pixelColor, 1);
        }
    }

    image->setData(buffer);
    image->init(width, height);
}

void Renderer::onResize(uint32_t width, uint32_t height) {
    if (!image) {
        image = make_shared<Texture>(GL_TEXTURE_2D);
    } else {
        // NOTE: resize image here?
    }

    // FIXME: segfault
    //delete[] buffer;

    // Assume RGBA
    buffer = new unsigned char[height * width * 4];
}

