#include "Scene.hpp"

Scene::Scene() {
    std::ifstream file("/home/Maxim/Documents/Labs_5_sem/Graphics/Lab6/config.json");

    // Read the file into a stringstream
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    // Convert stringstream to string
    std::string json_str = buffer.str();

    // Parse the JSON string
    json j;
    j = json::parse(json_str);

    // Access the "scene" object
    const json& scene = j["scene"];

    // Parse the "camera" object
    const json& camera = scene["camera"];
    std::string camera_type = camera["type"];
    std::vector<float> camera_position = camera["position"];
    std::vector<float> camera_direction = camera["direction"];

    camera = Camera(1920, 1080, )

    std::cout << "Camera Type: " << camera_type << std::endl;
    std::cout << "Camera Position: [" << camera_position[0] << ", " << camera_position[1] << ", " << camera_position[2] << "]" << std::endl;
    std::cout << "Camera Direction: [" << camera_direction[0] << ", " << camera_direction[1] << ", " << camera_direction[2] << "]" << std::endl;

    // Parse the "light" object
    const json& light = scene["light"];
    std::string light_type = light["type"];
    std::vector<float> light_position = light["position"];
    std::vector<float> light_direction = light["direction"];
    std::vector<float> light_fade = light["fade"];
    std::vector<float> light_color = light["color"];
    int light_rays = light["rays"];

    std::cout << "Light Type: " << light_type << std::endl;
    std::cout << "Light Position: [" << light_position[0] << ", " << light_position[1] << ", " << light_position[2] << "]" << std::endl;
    std::cout << "Light Direction: [" << light_direction[0] << ", " << light_direction[1] << ", " << light_direction[2] << "]" << std::endl;
    std::cout << "Light Fade: [" << light_fade[0] << ", " << light_fade[1] << "]" << std::endl;
    std::cout << "Light Color: [" << light_color[0] << ", " << light_color[1] << ", " << light_color[2] << "]" << std::endl;
    std::cout << "Light Rays: " << light_rays << std::endl;

    // Parse the "objects" array
    const json& objects = scene["objects"];
    for (const auto& obj : objects) {
        std::string obj_type = obj["type"];
        std::vector<float> position = obj["position"];
        std::vector<float> angles = obj["angles"];
        std::vector<float> color = obj["color"];
        float scale = obj["scale"];

        std::cout << "Object Type: " << obj_type << std::endl;
        std::cout << "Object Position: [" << position[0] << ", " << position[1] << ", " << position[2] << "]" << std::endl;
        std::cout << "Object Angles: [" << angles[0] << ", " << angles[1] << ", " << angles[2] << "]" << std::endl;
        std::cout << "Object Color: [" << color[0] << ", " << color[1] << ", " << color[2] << "]" << std::endl;
        std::cout << "Object Scale: " << scale << std::endl;
    }
}

bool Scene::read_config(const char* filename) {

}