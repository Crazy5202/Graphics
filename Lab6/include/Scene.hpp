#pragma once

#include "json.hpp"
using json = nlohmann::json;

#include "VAO.hpp"
#include "Camera.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>


class Scene
{
public:
    std::vector<VAO> objects;
    Camera cam;

    Scene();
    bool read_config(const char* filename);
    void render();
};