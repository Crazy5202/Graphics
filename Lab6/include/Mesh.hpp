#ifndef MESH_CLASS_H
#define MESH_CLASS_H

#include<string>

#include"VAO.hpp"
#include"Camera.hpp"

class Mesh
{
public:
	std::vector <float> vertices;
	// Store VAO in public so it can be used in the Draw function
	VAO vao;
	glm::mat4 pos_matrix;
	// Initializes the mesh
	Mesh(std::vector <float>& vertices, glm::mat4 _pos_matrix);

	// Draws the mesh
	void Draw
	(
		Shader& shader, 
		Camera& camera
	);
};
#endif