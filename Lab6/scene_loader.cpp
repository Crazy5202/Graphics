#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "json.hpp"

using json = nlohmann::json;

int main() {
    // Open the JSON file
    std::ifstream file("/home/Maxim/Documents/Labs_5_sem/Graphics/Lab6/config.json");
    if (!file.is_open()) {
        std::cerr << "Failed to open config.json" << std::endl;
        return 1;
    }

    // Read the file into a stringstream
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    // Convert stringstream to string
    std::string json_str = buffer.str();

    // Parse the JSON string
    json j;
    try {
        j = json::parse(json_str);
    } catch (json::parse_error& e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        return 1;
    }

    // Access the "scene" object
    const json& scene = j["scene"];

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

    // Parse the "camera" object
    const json& camera = scene["camera"];
    std::string camera_type = camera["type"];
    std::vector<float> camera_position = camera["position"];
    std::vector<float> camera_direction = camera["direction"];

    std::cout << "Camera Type: " << camera_type << std::endl;
    std::cout << "Camera Position: [" << camera_position[0] << ", " << camera_position[1] << ", " << camera_position[2] << "]" << std::endl;
    std::cout << "Camera Direction: [" << camera_direction[0] << ", " << camera_direction[1] << ", " << camera_direction[2] << "]" << std::endl;

    return 0;
}