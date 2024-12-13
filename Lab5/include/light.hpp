#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "vector3.hpp"

class Light
{
public:
    Vector3 position;
    Vector3 intensity;

    Light(const Vector3& pos, const Vector3& inten);
};

#endif // LIGHT_HPP