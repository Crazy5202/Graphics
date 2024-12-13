#ifndef RAYTRACER_HPP
#define RAYTRACER_HPP

#include "vector3.hpp"
#include "sphere.hpp"
#include "plane.hpp"
#include "cube.hpp"
#include "light.hpp"
#include "camera.hpp"
#include <vector>

Vector3 refract(const Vector3& I, const Vector3& N, float eta);
Vector3 traceRay(const Vector3& origin, const Vector3& direction,
    const std::vector<Sphere>& spheres, const std::vector<Plane>& planes,
    const std::vector<Cube>& cubes, const std::vector<Light>& lights, int depth);

#endif // RAYTRACER_HPP