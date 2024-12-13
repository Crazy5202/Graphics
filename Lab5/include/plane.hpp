#ifndef PLANE_HPP
#define PLANE_HPP

#include "vector3.hpp"

struct Plane
{
    Vector3 point;
    Vector3 normal;
    Vector3 color;
    float reflectivity;

    Plane(const Vector3& p, const Vector3& n, const Vector3& col, float refl);
    bool intersect(const Vector3& origin, const Vector3& direction, float& t) const;
};

#endif // PLANE_HPP