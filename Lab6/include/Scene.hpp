#pragma once

#include "json.hpp"
using json = nlohmann::json;

#include "VAO.hpp"
#include "Camera.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

struct LightSource {
    std::string type;
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 color;
    std::vector<float> fade;
    int rays;
};

class Scene
{
public:
    std::vector<VAO> objects;
    Camera cam;

    LightSource lightsrc;

    Scene();
    bool read_config(const char* filename);
    void render();
};