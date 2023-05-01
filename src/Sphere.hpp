#pragma once

#include "Hittable.hpp"

class Sphere : public Hittable {
    public:
        Sphere() {}
        Sphere(Point3 cen, double r) : center(cen), radius(r) {};

        virtual bool hit(
            const Ray& r, double tMin, double tMax, HitRecord& rec) const override;

    private:
        Point3 center;
        double radius;
};
