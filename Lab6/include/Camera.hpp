#pragma once

#include <GL/glew.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>

#include "shaderClass.hpp"

class Camera
{
public:
	// Stores the main vectors of the camera
	glm::vec3 Position = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 Orientation = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 Up = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::mat4 cameraMatrix = glm::mat4(1.0f);

	// Prevents the camera from jumping around when first clicking left click
	bool firstClick = true;

	// Stores the width and height of the window
	int width;
	int height;

	// Adjust the speed of the camera and it's sensitivity when looking around
	float speed = 0.1f;
	float sensitivity = 100.0f;
	
	float FOV = 75.0f;

	int projType;

	// Camera constructor to set up initial values
	Camera(int width, int height, int type);

	Camera(): Camera(1920, 1080, 1) {};

	void updateMat(float FOVdeg, int type);
	// Exports the camera matrix to a shader
	void Matrix(Shader& shader, const char* uniform);
	// Handles camera inputs
	void Inputs(GLFWwindow* window);
};