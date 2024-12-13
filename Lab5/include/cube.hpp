#ifndef CUBE_HPP
#define CUBE_HPP

#include "vector3.hpp"

class Cube
{
public:
    Vector3 min;
    Vector3 max;
    Vector3 color;
    float reflectivity;
    float transmissivity;
    float refractiveIndex;

    Cube(const Vector3& min, const Vector3& max, const Vector3& col, float refl, float trans, float refrIdx);
    bool intersect(const Vector3& origin, const Vector3& direction, float& t) const;
};

#endif // CUBE_HPP