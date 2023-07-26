#pragma once

#include "Sphere.hpp"

// NOTE: maybe could just rename HittableList to Scene? or World
struct Scene {
    std::vector<Sphere> Spheres;
};
