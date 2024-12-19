#include "Scene.hpp"

struct OBJECT {
    std::string type;
    glm::vec3 position;
    glm::vec3 color;
    std::vector<float> angles;
    float scale;
};

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

    if (camera_type == "orto") {
        cam.updateMat(cam.FOV, 0);
    } else {
        float newFOV = camera["FOV"];
        cam.updateMat(newFOV, 1);
    }

    // Parse the "light" object
    const json& light = scene["light"];
    lightsrc.type = light["type"];
    lightsrc.fade = std::vector<float>(light["fade"]);
    lightsrc.position = glm::vec3(light["position"][0], light["position"][1], light["position"][2]);
    lightsrc.direction = glm::vec3(light["direction"][0], light["direction"][1], light["direction"][2]);
    lightsrc.color = glm::vec3(light["color"][0], light["color"][1], light["color"][2]);
    lightsrc.rays = light["rays"];

    std::vector<OBJECT> objects;

    // Parse the "obj_json" array
    const json& obj_json = scene["objects"];
    for (const auto& obj : obj_json) {
        OBJECT temp_object;
        temp_object.type = obj["type"];
        temp_object.position = glm::vec3(obj["position"][0], obj["position"][1], obj["position"][2]);
        temp_object.color = glm::vec3(obj["color"][0], obj["color"][1], obj["color"][2]);
        temp_object.angles = std::vector<float>(obj["angles"]);
        float scale = obj["scale"];
        objects.push_back(temp_object);
    }
    std::cout << "HOORAY!" << std::endl;
}

bool Scene::read_config(const char* filename) {

}