#include <iostream>
#include "json.hpp"

using json = nlohmann::json;

int main() {
    // JSON data as a string
    std::string json_str = R"(
    {
      "scene": {
        "light": {
          "name": "MainLight",
          "position": [0.0, 1.0, 2.0],
          "direction": [3.0, 4.0, 5.0]
        },
        "objects": [
          {
            "position": [6.0, 7.0, 8.0],
            "scale": 1.5
          },
          {
            "position": [9.0, 10.0, 11.0],
            "scale": 2.0
          }
        ],
        "camera": {
          "position": [12.0, 13.0, 14.0],
          "direction": [15.0, 16.0, 17.0],
          "projectionType": "perspective"
        }
      }
    }
    )";

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
    std::string light_name = light["name"];
    std::vector<double> light_position = light["position"];
    std::vector<double> light_direction = light["direction"];

    std::cout << "Light Name: " << light_name << std::endl;
    std::cout << "Light Position: [" << light_position[0] << ", " << light_position[1] << ", " << light_position[2] << "]" << std::endl;
    std::cout << "Light Direction: [" << light_direction[0] << ", " << light_direction[1] << ", " << light_direction[2] << "]" << std::endl;

    // Parse the "objects" array
    const json& objects = scene["objects"];
    for (const auto& obj : objects) {
        std::vector<double> position = obj["position"];
        double scale = obj["scale"];
        std::cout << "Object Position: [" << position[0] << ", " << position[1] << ", " << position[2] << "]" << std::endl;
        std::cout << "Object Scale: " << scale << std::endl;
    }

    // Parse the "camera" object
    const json& camera = scene["camera"];
    std::vector<double> camera_position = camera["position"];
    std::vector<double> camera_direction = camera["direction"];
    std::string projection_type = camera["projectionType"];

    std::cout << "Camera Position: [" << camera_position[0] << ", " << camera_position[1] << ", " << camera_position[2] << "]" << std::endl;
    std::cout << "Camera Direction: [" << camera_direction[0] << ", " << camera_direction[1] << ", " << camera_direction[2] << "]" << std::endl;
    std::cout << "Projection Type: " << projection_type << std::endl;

    return 0;
}