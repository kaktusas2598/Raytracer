#pragma once


#include "Ray.hpp"

struct HitRecord {
    Point3 p;
    Vec3 normal;
    double t;
    bool frontFace;

    // HACK ?
    Color color; //<<< hit object albedo color

    inline void setFaceNormal(const Ray& r, const Vec3& outwardNormal) {
        frontFace = dot(r.direction(), outwardNormal) < 0;
        normal = frontFace ? outwardNormal :-outwardNormal;
    }
};

class Hittable {
    public:
        virtual bool hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const = 0;
        Color Albedo;
};
