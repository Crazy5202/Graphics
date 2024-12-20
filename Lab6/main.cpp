#include "Scene.hpp"
#include <filesystem>

int main() {

    int width = 1920, height = 1080;

	glfwInit();

	GLFWwindow* window = glfwCreateWindow(width, height, "OpenGL", NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	glewInit();

    glViewport(0, 0, width, height);

	glEnable(GL_DEPTH_TEST);    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    auto lab_path = std::filesystem::current_path().parent_path().string();
    Scene scene(lab_path);
    scene.shader.Activate();
    
    int type = 1;
	glUniform3fv(glGetUniformLocation(scene.shader.ID, "lightColor"), 1, glm::value_ptr(scene.lightsrc.color));
	glUniform3fv(glGetUniformLocation(scene.shader.ID, "lightPos"), 1, glm::value_ptr(scene.lightsrc.position));
    glUniform1d(glGetUniformLocation(scene.shader.ID, "lightType"), type);

	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		// Clean the back buffer and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Handles camera inputs
		scene.cam.Inputs(window);
        // Render all objects
        scene.render();

		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}

	// Delete all the objects we've created
	scene.shader.Delete();
	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}