#include "plane.hpp"
#include <cmath>

Plane::Plane(const Vector3& p, const Vector3& n, const Vector3& col, float refl)
    : point(p), normal(n.normalize()), color(col), reflectivity(refl) {}

bool Plane::intersect(const Vector3& origin, const Vector3& direction, float& t) const
{
    float denom = normal.dot(direction);
    if (std::abs(denom) > 1e-6)
    {
        t = (point - origin).dot(normal) / denom;
        return t >= 0;
    }
    return false;
}