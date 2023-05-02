#pragma once

#include "Camera.hpp"
#include "Color.hpp"
#include "Common.hpp"
#include "Hittable.hpp"
#include "Texture.hpp"

class Renderer {
    public:
        Renderer() = default;
        ~Renderer() { delete[] buffer; }

        // Raytrace to texture pixel buffer
        void raytraceWorld(const Hittable& world, uint32_t width, uint32_t height);
        // Raytrace to fstream and write PPM image
        void exportRaytracedPPM(const Hittable& world, uint32_t width, uint32_t height);

        void generateCheckerTexture(uint32_t width, uint32_t height);

        unsigned int getTextureID() const { return image->getID(); }

        void onResize(uint32_t width, uint32_t height);

        // Maybe move to color.hpp?
        static Color rayColor(const Ray& r, const Hittable& world) {
            HitRecord rec;
            if (world.hit(r, 0, INF, rec)) {
                // Visualise spehere's normals
                return 0.5 * (rec.normal + Color(1,1,1));
            }
            Vec3 normalizedDir = normalize(r.direction());
            // Background colour gradient
            auto t = 0.5*(normalizedDir.y() + 1.0);
            return (1.0-t)*Color(1.0, 1.0, 1.0) + t*Color(0.5, 0.7, 1.0);
        }

        // Higher sample count will solve aliasing issues, but decrease performance, static to make it easier with imgui
        static int SamplesPerPixel;
    private:
        shared_ptr<Texture> image = nullptr;
        unsigned char* buffer = nullptr;

        Camera camera;
};
