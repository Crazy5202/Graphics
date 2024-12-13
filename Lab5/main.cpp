#include "raytracer.hpp"
#include "ppm.hpp"

int traceDepth = 3;

int main() {
    int width = 1920;
    int height = 1080;
    PPM image(width, height);

    std::vector<Sphere> spheres =
    {
        Sphere(Vector3(0, -1, 5), 1, Vector3(1, 0, 0), 0.0, 0.0, 1.5),
        Sphere(Vector3(2, 0, 4), 1, Vector3(0, 1, 0), 0.0, 0.0, 1.5),
    };

    std::vector<Plane> planes =
    {
        Plane(Vector3(0, -2, 0), Vector3(0, 1, 0), Vector3(1.0, 0, 0), 0.0),
    };

    std::vector<Cube> cubes =
    {
        Cube(Vector3(-2.5, 0, 3), Vector3(-1.5, 1, 4), Vector3(0, 0, 1), 0.4, 0.6, 1.33),
        Cube(Vector3(0.5, 0.5, 6), Vector3(1.5, 1.5, 7), Vector3(1, 0, 1), 0.4, 0.6, 1.33),
    };

    std::vector<Light> lights =
    {
        Light(Vector3(0, 5, 0), Vector3(1, 1, 1)),
        Light(Vector3(5, 7, 5), Vector3(0.1, 0.1, 0.1)),
    };

    Camera camera(Vector3(0, 2, -0.5), Vector3(-1, 0, 3), Vector3(0, 1, 0));

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            Vector3 direction = camera.getRayDirection(x, y, width, height);
            Vector3 color = traceRay(camera.position, direction, spheres, planes, cubes, lights, traceDepth);
            image.set_pixel(x, y, 
                std::min(255, static_cast<int>(color.x * 255)), 
                std::min(255, static_cast<int>(color.y * 255)), 
                std::min(255, static_cast<int>(color.z * 255)));
        }
    }

    image.write_file("output.ppm");
    std::cout << "Готово!" << std::endl;
    return 0;
}