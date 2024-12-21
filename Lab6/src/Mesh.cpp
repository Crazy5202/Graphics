#include "Mesh.hpp"

Mesh::Mesh(std::vector <float>& vertices, glm::mat4 _matrix, glm::vec3 _color, std::vector<float> minmax): pos_matrix(_matrix), color(_color)
{
	Mesh::vertices = vertices;

	vao.Bind();
	// Generates Vertex Buffer Object and links it to vertices
	VBO vbo(vertices);
	// Links vbo attributes such as coordinates and colors to vao
	vao.LinkAttrib(vbo, 0, 3, GL_FLOAT, 6*sizeof(float), (void*)0);
	vao.LinkAttrib(vbo, 1, 3, GL_FLOAT, 6*sizeof(float), (void*)(3 * sizeof(float)));
	// Unbind all to prevent accidentally modifying them
	vao.Unbind();
	vbo.Unbind();

	glm::vec4 temp_vec1 = pos_matrix*glm::vec4(minmax[0], minmax[2], minmax[4], 1.0);
	glm::vec4 temp_vec2 = pos_matrix*glm::vec4(minmax[1], minmax[3], minmax[5], 1.0);
	min_pos = glm::vec3(std::min(temp_vec1.x, temp_vec2.x), std::min(temp_vec1.y, temp_vec2.y), std::min(temp_vec1.z, temp_vec2.z)) - glm::vec3(0.1f);
	max_pos = glm::vec3(std::max(temp_vec1.x, temp_vec2.x), std::max(temp_vec1.y, temp_vec2.y), std::max(temp_vec1.z, temp_vec2.z)) + glm::vec3(0.1f);
}

void Mesh::Draw(Shader& shader, Camera& camera)
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

bool Mesh::check_intersect(glm::vec3& position) {
	if (position.x >= min_pos.x and position.x <= max_pos.x and position.y >= min_pos.y and position.y<= max_pos.y and position.z >= min_pos.z and position.z <= max_pos.z) {
		return true;
	}
	return false;
}