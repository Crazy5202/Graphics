#include "sphere.hpp"
#include <cmath>

Sphere::Sphere(const Vector3& c, float r, const Vector3& col, float refl, float trans, float refrIdx)
    : center(c), radius(r), color(col), reflectivity(refl), transmissivity(trans), refractiveIndex(refrIdx) {}

bool Sphere::intersect(const Vector3& origin, const Vector3& direction, float& t) const
{
    Vector3 oc = origin - center;
    float b = oc.dot(direction);
    float c = oc.dot(oc) - radius * radius;
    float discriminant = b * b - c;
    if (discriminant > 0)
    {
        float sqrtD = std::sqrt(discriminant);
        t = -b - sqrtD;
        if (t < 0) t = -b + sqrtD;
        return t >= 0;
    }
    return false;
}