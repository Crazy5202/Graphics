#include "camera.hpp"
#include <cmath>

Camera::Camera(const Vector3& pos, const Vector3& lookAt, const Vector3& upVec)
    : position(pos)
{
    forward = (lookAt - position).normalize();
    right = forward.cross(upVec).normalize();
    up = right.cross(forward);
}

Vector3 Camera::getRayDirection(float x, float y, float imageWidth, float imageHeight) const
{
    float fovScale = std::tan(M_PI / 4);
    float aspectRatio = imageWidth / imageHeight;
    float px = (2 * (x + 0.5) / imageWidth - 1) * aspectRatio * fovScale;
    float py = (1 - 2 * (y + 0.5) / imageHeight) * fovScale;
    return (forward + right * px + up * py).normalize();
}