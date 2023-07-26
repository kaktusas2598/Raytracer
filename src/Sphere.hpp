#pragma once

#include "Hittable.hpp"

class Sphere : public Hittable {
    public:
        Sphere() {}
        Sphere(Point3 cen, double r) : center(cen), radius(r) {};

        virtual bool hit(
            const Ray& r, double tMin, double tMax, HitRecord& rec) const override;

        Point3* getCenter() { return &center; }
        float* getRadius() { return (float*)&radius; }

    private:
        Point3 center{0.0, 0.0, 0.0};
        double radius = 0.5f;
};
