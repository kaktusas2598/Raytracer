#pragma once

#include <cmath>
#include <limits>
#include <memory>

using std::shared_ptr;
using std::make_shared;
using std::sqrt;

// Constants
const double INF = std::numeric_limits<double>::infinity();
const double PI = 3.1415926535897932385;

// Utility Functions
inline double degToRad(double degrees) {
    return degrees * PI / 180.0;
}

// Common Headers
#include "Ray.hpp"
#include "Vec3.hpp"
