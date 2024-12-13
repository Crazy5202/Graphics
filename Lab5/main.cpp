#include <SFML/Graphics.hpp>
#include "raytracer.hpp"

int traceDepth = 5;

int main()
{
    sf::RenderWindow window(sf::VideoMode(1200, 1000), "Ray Tracing");
    sf::Image image;
    image.create(1200, 1000);

    std::vector<Sphere> spheres =
    {
        Sphere(Vector3(0, -1, 5), 1, Vector3(1, 0, 0), 0.5, 0.5, 1.5),
        Sphere(Vector3(2, 0, 4), 1, Vector3(0, 1, 0), 0.5, 0.5, 1.5),
    };

    std::vector<Plane> planes =
    {
        Plane(Vector3(0, -2, 0), Vector3(0, 1, 0), Vector3(1, 1, 1), 0.3),
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

    for (int y = 0; y < 1000; ++y)
    {
        for (int x = 0; x < 1200; ++x)
        {
            Vector3 direction = camera.getRayDirection(x, y, 1200, 1000);
            Vector3 color = traceRay(camera.position, direction, spheres, planes, cubes, lights, traceDepth);
            sf::Color pixelColor(
                std::min(255, static_cast<int>(color.x * 255)),
                std::min(255, static_cast<int>(color.y * 255)),
                std::min(255, static_cast<int>(color.z * 255))
            );
            image.setPixel(x, y, pixelColor);
        }
    }

    sf::Texture texture;
    texture.loadFromImage(image);
    sf::Sprite sprite(texture);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }
        window.clear();
        window.draw(sprite);
        window.display();
    }

    return 0;
}