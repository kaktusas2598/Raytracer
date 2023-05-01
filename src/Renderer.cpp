#include "Renderer.hpp"

#include <iostream>
#include <fstream>

int Renderer::SamplesPerPixel = 1;

void Renderer::raytraceWorld(const Hittable& world, uint32_t width, uint32_t height) {
    onResize(width, height);

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            Color pixelColor(0,0,0);
            for (int s = 0; s < SamplesPerPixel; ++s) {
                auto u = (j + randomDouble()) / (width - 1);
                auto v = (i + randomDouble()) / (height - 1);
                Ray r = camera.getRay(u, v);
                pixelColor += rayColor(r, world);
            }
            writeColorToBuffer(buffer, j, i, width, pixelColor, SamplesPerPixel);
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
            for (int s = 0; s < SamplesPerPixel; ++s) {
                auto u = (i + randomDouble()) / (width-1);
                auto v = (j + randomDouble()) / (height-1);
                Ray r = camera.getRay(u, v);
                pixelColor += Renderer::rayColor(r, world);
            }
            writeColorPPM(outFile, pixelColor, SamplesPerPixel);
        }
    }
    std::cerr << "Done.\n";
    outFile.close();
}

void Renderer::generateCheckerTexture(uint32_t width, uint32_t height) {
    onResize(width, height);

    int c;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            // Generate checkers
            c = ((((i&0x8)==0)^((j&0x8))==0))*255;

            Color pixelColor(randomDouble(), c, c);
            writeColorToBuffer(buffer, j, i, width, pixelColor, 1);
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

