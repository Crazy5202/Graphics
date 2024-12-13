#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "vector3.hpp"

struct Camera
{
    Vector3 position;
    Vector3 forward;
    Vector3 up;
    Vector3 right;

    Camera(const Vector3& pos, const Vector3& lookAt, const Vector3& upVec);
    Vector3 getRayDirection(float x, float y, float imageWidth, float imageHeight) const;
};

#endif // CAMERA_HPP