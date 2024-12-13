#include "raytracer.hpp"
#include <vector>
#include <limits>
#include <cmath>

Vector3 refract(const Vector3& I, const Vector3& N, float eta)
{
    float cosI = -I.dot(N);
    float sinT2 = eta * eta * (1 - cosI * cosI);
    if (sinT2 > 1) return Vector3(0, 0, 0);
    float cosT = std::sqrt(1 - sinT2);
    return I * eta + N * (eta * cosI - cosT);
}

Vector3 traceRay(const Vector3& origin, const Vector3& direction,
    const std::vector<Sphere>& spheres, const std::vector<Plane>& planes,
    const std::vector<Cube>& cubes, const std::vector<Light>& lights, int depth)
{
    if (depth <= 0) return Vector3(0, 0, 0);

    float tMin = std::numeric_limits<float>::infinity();
    Vector3 hitPoint, normal, baseColor;
    float reflectivity = 0;
    float transmissivity = 0;
    float refractiveIndex = 1;

    for (const auto& sphere : spheres)
    {
        float t;
        if (sphere.intersect(origin, direction, t) && t < tMin)
        {
            tMin = t;
            hitPoint = origin + direction * t;
            normal = (hitPoint - sphere.center).normalize();
            baseColor = sphere.color;
            reflectivity = sphere.reflectivity;
            transmissivity = sphere.transmissivity;
            refractiveIndex = sphere.refractiveIndex;
        }
    }

    for (const auto& plane : planes)
    {
        float t;
        if (plane.intersect(origin, direction, t) && t < tMin)
        {
            tMin = t;
            hitPoint = origin + direction * t;
            normal = plane.normal;
            baseColor = plane.color;
            reflectivity = plane.reflectivity;
        }
    }

    for (const auto& cube : cubes)
    {
        float t;
        if (cube.intersect(origin, direction, t) && t < tMin)
        {
            tMin = t;
            hitPoint = origin + direction * t;

            if (std::abs(hitPoint.x - cube.min.x) < 1e-3) normal = Vector3(-1, 0, 0);
            else if (std::abs(hitPoint.x - cube.max.x) < 1e-3) normal = Vector3(1, 0, 0);
            else if (std::abs(hitPoint.y - cube.min.y) < 1e-3) normal = Vector3(0, -1, 0);
            else if (std::abs(hitPoint.y - cube.max.y) < 1e-3) normal = Vector3(0, 1, 0);
            else if (std::abs(hitPoint.z - cube.min.z) < 1e-3) normal = Vector3(0, 0, -1);
            else if (std::abs(hitPoint.z - cube.max.z) < 1e-3) normal = Vector3(0, 0, 1);

            baseColor = cube.color;
            reflectivity = cube.reflectivity;
            transmissivity = cube.transmissivity;
            refractiveIndex = cube.refractiveIndex;
        }
    }

    if (tMin == std::numeric_limits<float>::infinity()) return Vector3(0, 0, 0);

    Vector3 color(0, 0, 0);
    for (const auto& light : lights)
    {
        Vector3 lightDir = (light.position - hitPoint).normalize();
        Vector3 lambert = baseColor * std::max(0.f, normal.dot(lightDir));
        color = color + lambert * light.intensity;
    }

    if (reflectivity > 0)
    {
        Vector3 reflectDir = direction - normal * 2 * direction.dot(normal);
        color = color + traceRay(hitPoint + normal * 1e-4, reflectDir, spheres, planes, cubes, lights, depth - 1) * reflectivity;
    }

    if (transmissivity > 0)
    {
        float eta = direction.dot(normal) < 0 ? 1 / refractiveIndex : refractiveIndex;
        Vector3 refractDir = refract(direction, normal, eta);
        color = color + traceRay(hitPoint - normal * 1e-4, refractDir, spheres, planes, cubes, lights, depth - 1) * transmissivity;
    }

    return color;
}