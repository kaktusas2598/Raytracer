#pragma once

#include "Color.hpp"
#include "Common.hpp"
#include "Hittable.hpp"
#include "Texture.hpp"
#include "Camera.hpp"

class Renderer {
    public:
        Renderer() = default;
        ~Renderer() { delete[] buffer; }

        // Raytrace to texture pixel buffer
        void raytraceWorld(const Hittable& world, uint32_t width, uint32_t height, Camera* camera);
        // Raytrace to fstream and write PPM image
        void exportRaytracedPPM(const Hittable& world, uint32_t width, uint32_t height, Camera* camera);

        void generateCheckerTexture(uint32_t width, uint32_t height);

        unsigned int getTextureID() const { return image->getID(); }
        int* getSamplesPerPixel() { return &samplesPerPixel; }
        int* getMaxDepth() { return &maxDepth; }
        int* getRenderType() { return &renderType; }

        void onResize(uint32_t width, uint32_t height);

        // Maybe move to color.hpp?
        Color rayColor(const Ray& r, const Hittable& world, int depth) {
            HitRecord rec;

            // Exceeded ray bounce limit
            if (depth <= 0) {
                return Color(0,0,0);
            }
            // Adding small bias to tMin to fix any shadow acne
            if (world.hit(r, 0.001, INF, rec)) {
                if (renderType == 0) {
                    // Generate random diffuse bounce ray
                    Point3 target = rec.p + rec.normal + randomInUnitSphere();
                    return 0.5 * rayColor(Ray(rec.p, target - rec.p), world, depth-1);
                } else if (renderType == 1) {
                    // Directional light
                    Vec3 lightDir = normalize(Vec3(-1, -1, -1));
                    // Calculate diffuse factor, base on the angle between light's direction and normal of ray/world intersection
                    double intensity = std::max(dot(rec.normal,-lightDir), 0.0); // == cos(angle)
                    return intensity * rec.color;
                    // With normals
                    return intensity * 0.5 * (rec.normal + Color(1,1,1));
                } else if (renderType == 2) {
                    // Visualise spehere's normals only
                    return 0.5 * (rec.normal + Color(1,1,1));
                } else if (renderType == 3) {
                    // Visualise ray hit points
                    return (rec.p);
                }
            }
            Vec3 normalizedDir = normalize(r.direction());
            // Background colour gradient
            auto t = 0.5*(normalizedDir.y() + 1.0);
            return (1.0-t)*Color(1.0, 1.0, 1.0) + t*Color(0.5, 0.7, 1.0);
        }

    private:
        shared_ptr<Texture> image = nullptr;
        unsigned char* buffer = nullptr;
        int maxDepth = 5; //<<< Maximum number of light ray bounces
        int samplesPerPixel = 1; // Higher sample count will solve aliasing issues, but decrease performance
        int renderType = 0;
};
