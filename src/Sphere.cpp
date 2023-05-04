#include "Sphere.hpp"


// Ray - sphere intersection
bool Sphere::hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const {
    Vec3 oc = r.origin() - center;
    // Calculate quadratic formula coefficients for sphere equation
    auto a = r.direction().lengthSquared(); // same as dot product of r.direction * r.direction
    auto halfB = dot(oc, r.direction());
    auto c = oc.lengthSquared() - radius*radius;

    auto discriminant = halfB*halfB - a*c;
    if (discriminant < 0) return false;
    auto sqrtd = sqrt(discriminant);

    // Find the nearest root that lies in the aceptable range
    auto root = (-halfB - sqrtd) / a; // 1st root t1
    if (root < tMin || tMax < root) {
        root = (-halfB + sqrtd) / a; // 2nd root t0
        if (root < tMin || tMax < root) {
            return false;
        }
    }

    rec.t = root;
    // Use line equation to get hit point
    rec.p = r.at(rec.t); // could calculate for both roots for entry/exit ray intersections
    //Vec3 outwardNormal = (rec.p - center) / radius;
    // Determine if ray hit front or back face
    //rec.setFaceNormal(r, outwardNormal);

    return true;
}
