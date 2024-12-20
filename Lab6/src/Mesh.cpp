#include "Mesh.hpp"

Mesh::Mesh(std::vector <float>& vertices, glm::mat4 _matrix): pos_matrix(_matrix)
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
	// Bind shader to be able to access uniforms
	shader.Activate();
	vao.Bind();

	// Take care of the camera Matrix
	glUniform3f(glGetUniformLocation(shader.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
	camera.Matrix(shader, "camMatrix");

	// Push the matrices to the vertex shader
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(pos_matrix));

	// Draw the actual mesh
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
}