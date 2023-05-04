#include "Renderer.hpp"

#include <iostream>
#include <fstream>

void Renderer::raytraceWorld(const Hittable& world, uint32_t width, uint32_t height, Camera* camera) {
    onResize(width, height);

    // Important to make sure all rays have same origin here otherwise it will produce screen tearing effect
    glm::vec3 rayOrigin = camera->getPosition();
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            glm::vec3 rayDirection = camera->getRayDirections()[i + j * width];
            //Ray r = Ray(toVec3(rayOrigin), toVec3(rayDirection));
            //Color pixelColor = rayColor(r, world, maxDepth);
            // TODO: make sampling for anti-aliasing work with new camera system
            Color pixelColor(0,0,0);
            //printf ("i: %d, j: %d\n", i, j);
            for (int s = 0; s < samplesPerPixel; ++s) {
                //auto u = (i + randomDouble()) / (width - 1);
                //auto v = (j + randomDouble()) / (height - 1);
                int u = clamp(i + randomInt(-1, 1), 0, width);
                int v = clamp(j + randomInt( -1, 1), 0, height);
                //printf ("u: %d, v: %d\n", u, v);
                //Ray r = camera->getRay(u, v);
                glm::vec3 rayDirection = camera->getRayDirections()[u + v * width];
                Ray r = Ray(toVec3(rayOrigin), toVec3(rayDirection));
                pixelColor += rayColor(r, world, maxDepth);
            }
            writeColorToBuffer(buffer, i, j, width, pixelColor, samplesPerPixel);
        }
    }

    // FIXME: should be able to set data without updating width, height and only set size in onResize()
    image->setData(buffer);
    image->init(width, height);

}

void Renderer::exportRaytracedPPM(const Hittable& world, uint32_t width, uint32_t height, Camera* camera) {
    std::ofstream outFile;
    outFile.open("image.ppm");
    outFile << "P3\n" << (int)width << ' ' << (int)height << "\n255\n";

    glm::vec3 rayOrigin = camera->getPosition();
    for (int j = height-1; j >= 0; --j) {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < width; ++i) {
            glm::vec3 rayDirection = camera->getRayDirections()[i + j * width];
            Ray r = Ray(toVec3(rayOrigin), toVec3(rayDirection));
            Color pixelColor = rayColor(r, world, maxDepth);
            // TODO: multisample anti-aliasing
            //Color pixelColor(0,0,0);
            //for (int s = 0; s < samplesPerPixel; ++s) {
                //auto u = (i + randomDouble()) / (width-1);
                //auto v = (j + randomDouble()) / (height-1);
                //Ray r = camera->getRay(u, v);
                //pixelColor += Renderer::rayColor(r, world, maxDepth);
            //}
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
    if (!image) { // || image->
        image = make_shared<Texture>(GL_TEXTURE_2D);
    } else {
        // NOTE: resize image here?
    }

    // FIXME: segfault
    //delete[] buffer;

    // Assume RGBA
    buffer = new unsigned char[height * width * 4];
}

