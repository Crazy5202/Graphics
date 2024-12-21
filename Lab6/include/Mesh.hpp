#pragma once

#include<string>

#include"VAO.hpp"
#include"Camera.hpp"

class Mesh
{
public:
	std::vector <float> vertices;
	// Store VAO in public so it can be used in the Draw function
	VAO vao;
	// matrix and color
	glm::mat4 pos_matrix;
	glm::vec3 color;
	glm::vec3 min_pos;
	glm::vec3 max_pos;
	// Initializes the mesh
	Mesh(std::vector <float>& vertices, glm::mat4 _pos_matrix, glm::vec3 color, std::vector<float> _minmax);

	bool check_intersect(glm::vec3& position);
	// Draws the mesh
	void Draw (Shader& shader, Camera& camera);
};