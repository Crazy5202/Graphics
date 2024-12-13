#include "cube.hpp"
#include <algorithm>

Cube::Cube(const Vector3& min, const Vector3& max, const Vector3& col, float refl, float trans, float refrIdx)
    : min(min), max(max), color(col), reflectivity(refl), transmissivity(trans), refractiveIndex(refrIdx) {}

bool Cube::intersect(const Vector3& origin, const Vector3& direction, float& t) const
{
    float tMin = (min.x - origin.x) / direction.x;
    float tMax = (max.x - origin.x) / direction.x;
    if (tMin > tMax) std::swap(tMin, tMax);

    float tyMin = (min.y - origin.y) / direction.y;
    float tyMax = (max.y - origin.y) / direction.y;
    if (tyMin > tyMax) std::swap(tyMin, tyMax);

    if ((tMin > tyMax) || (tyMin > tMax)) return false;
    if (tyMin > tMin) tMin = tyMin;
    if (tyMax < tMax) tMax = tyMax;

    float tzMin = (min.z - origin.z) / direction.z;
    float tzMax = (max.z - origin.z) / direction.z;
    if (tzMin > tzMax) std::swap(tzMin, tzMax);

    if ((tMin > tzMax) || (tzMin > tMax)) return false;
    if (tzMin > tMin) tMin = tzMin;
    if (tzMax < tMax) tMax = tzMax;

    t = tMin >= 0 ? tMin : tMax;
    return t >= 0;
}