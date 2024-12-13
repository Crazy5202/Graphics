#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "vector3.hpp"

class Sphere
{
public:
    Vector3 center;
    float radius;
    Vector3 color;
    float reflectivity;
    float transmissivity;
    float refractiveIndex;

    Sphere(const Vector3& c, float r, const Vector3& col, float refl, float trans, float refrIdx);
    bool intersect(const Vector3& origin, const Vector3& direction, float& t) const;
};

#endif // SPHERE_HPP