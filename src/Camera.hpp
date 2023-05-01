#pragma once

#include "Common.hpp"

class Camera {
    public:
        Camera();

        Ray getRay(double u, double v) const {
            return Ray(origin, lowerLeftCorner + u*horizontal + v*vertical - origin);
        }
    private:
        Point3 origin;
        Point3 lowerLeftCorner;
        Vec3 horizontal;
        Vec3 vertical;
};
