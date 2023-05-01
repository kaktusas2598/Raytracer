#include "Sphere.hpp"


bool Sphere::hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const {
    Vec3 oc = r.origin() - center;
    // Calculate quadratic formula coefficients
    auto a = r.direction().lengthSquared(); // same as dot product of r.direction * r.direction
    auto halfB = dot(oc, r.direction());
    auto c = oc.lengthSquared() - radius*radius;

    auto discriminant = halfB*halfB - a*c;
    if (discriminant < 0) return false;
    auto sqrtd = sqrt(discriminant);

    // Find the nearest root that lies in the aceptable range
    auto root = (-halfB - sqrtd) / a;
    if (root < tMin || tMax < root) {
        root = (-halfB + sqrtd) / a;
        if (root < tMin || tMax < root) {
            return false;
        }
    }

    rec.t = root;
    rec.p = r.at(rec.t);
    Vec3 outwardNormal = (rec.p - center) / radius;
    rec.setFaceNormal(r, outwardNormal);

    return true;
}
