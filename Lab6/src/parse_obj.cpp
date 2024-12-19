#include "parse_obj.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <glm/glm.hpp>

std::vector<float> parseOBJ(const std::string& filePath) {
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
/*
// Usage example
int main() {
    auto parsedData = parseOBJ("cube.obj");

    std::cout << "Vertices count: " << parsedData.size() / 6 << std::endl;

    // Data can now be uploaded to OpenGL VBO/VAO
    return 0;
}
*/