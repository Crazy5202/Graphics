#pragma once

#include "json.hpp"
using json = nlohmann::json;

#include "Mesh.hpp"
#include "Camera.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <glm/glm.hpp>

struct LightSource {
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 color;
    std::vector<float> fade;
};

class Scene
{
public:
    std::vector<Mesh> meshes;
    Camera cam;

    Shader shader;
    LightSource lightsrc;

    Scene(const std::string& filePath);
    std::vector<float> parseOBJ(const std::string& filePath, std::vector<float>& minmax);
    void render();
};