#include "Mesh.hpp"

Mesh::Mesh(std::vector <float>& vertices, glm::mat4 _matrix, glm::vec3 _color): pos_matrix(_matrix), color(_color)
{
	Mesh::vertices = vertices;

	vao.Bind();
	// Generates Vertex Buffer Object and links it to vertices
	VBO vbo(vertices);
	// Links vbo attributes such as coordinates and colors to vao
	vao.LinkAttrib(vbo, 0, 3, GL_FLOAT, sizeof(float), (void*)0);
	vao.LinkAttrib(vbo, 1, 3, GL_FLOAT, sizeof(float), (void*)(3 * sizeof(float)));
	// Unbind all to prevent accidentally modifying them
	vao.Unbind();
	vbo.Unbind();
}

void Mesh::Draw
(
	Shader& shader, 
	Camera& camera
)
{
	shader.Activate();
	vao.Bind();

	// pushing info to vertex shader
	camera.Matrix(shader, "camMatrix");
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(pos_matrix));

	// pushing info to fragment shader
	glUniform3fv(glGetUniformLocation(shader.ID, "camPos"), 1, glm::value_ptr(camera.Position));

	glUniform3fv(glGetUniformLocation(shader.ID, "objectColor"), 1, glm::value_ptr(color));

	// Draw the actual mesh
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
}