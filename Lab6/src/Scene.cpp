#include "Scene.hpp"

struct OBJECT {
    std::string type;
    glm::vec3 position;
    glm::vec3 color;
    std::vector<float> angles;
    float scale;
};

Scene::Scene(const std::string& filePath) {
    std::ifstream file(filePath+"/config.json");

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
        temp_object.scale = obj["scale"];
        objects.push_back(temp_object);
    }
    for (auto& obj: objects) {
        auto res = parseOBJ(filePath+"/assets/objects/"+obj.type+".obj");
        glm::mat4 transformationMatrix = glm::mat4(1.0f);

        // 1. Масштабирование
        transformationMatrix = glm::scale(transformationMatrix, glm::vec3(obj.scale));

        // 2. Поворот по оси X
        transformationMatrix = glm::rotate(transformationMatrix, glm::radians(obj.angles[0]), glm::vec3(1.0f, 0.0f, 0.0f));

        // 3. Поворот по оси Y
        transformationMatrix = glm::rotate(transformationMatrix, glm::radians(obj.angles[1]), glm::vec3(0.0f, 1.0f, 0.0f));

        // 4. Поворот по оси Z
        transformationMatrix = glm::rotate(transformationMatrix, glm::radians(obj.angles[2]), glm::vec3(0.0f, 0.0f, 1.0f));

        // 5. Перемещение
        transformationMatrix = glm::translate(transformationMatrix, obj.position);
        
        if (!res.empty()) {
            meshes.emplace_back(res, transformationMatrix);
        }
    }
    std::cout << "HOORAY!" << std::endl;
}

std::vector<float> Scene::parseOBJ(const std::string& filePath) {
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<float> data;

    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return data;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream lineStream(line);
        std::string prefix;
        lineStream >> prefix;

        if (prefix == "v") {
            float x, y, z;
            lineStream >> x >> y >> z;
            positions.emplace_back(x, y, z);
        } else if (prefix == "vn") {
            float x, y, z;
            lineStream >> x >> y >> z;
            normals.emplace_back(x, y, z);
        } else if (prefix == "f") {
            std::string vertexData;

            while (lineStream >> vertexData) {
                std::istringstream vertexStream(vertexData);
                std::string posIndex, normalIndex;

                std::getline(vertexStream, posIndex, '/');
                std::getline(vertexStream, normalIndex, '/');
                std::getline(vertexStream, normalIndex, '/');

                unsigned int posIdx = std::stoi(posIndex) - 1;
                unsigned int normalIdx;
                if (!normalIndex.empty()) {
                    normalIdx = std::stoi(normalIndex) - 1;
                } else {
                    normalIdx = 0;
                }

                data.push_back(positions[posIdx].x);
                data.push_back(positions[posIdx].y);
                data.push_back(positions[posIdx].z);

                data.push_back(normals[normalIdx].x);
                data.push_back(normals[normalIdx].y);
                data.push_back(normals[normalIdx].z);
            }
        }
    }

    file.close();
    return data;
}