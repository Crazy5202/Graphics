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
    glDisable(GL_CULL_FACE);

    auto lab_path = std::filesystem::current_path().parent_path().string();
    Scene scene(lab_path);
    //std::cout << "HERE!" << std::endl;
    auto val = glGetError();
    if (val) std::cout << "scene_creation " << val << std::endl;
    scene.shader.Activate();
    
    int type = 1;
	glUniform3fv(glGetUniformLocation(scene.shader.ID, "lightColor"), 1, glm::value_ptr(scene.lightsrc.color));
	glUniform3fv(glGetUniformLocation(scene.shader.ID, "lightPos"), 1, glm::value_ptr(scene.lightsrc.position));
    glUniform1i(glGetUniformLocation(scene.shader.ID, "lightType"), type);
    
    val = glGetError();
    if (val) std::cout << "passing_to_shader_main " << val << std::endl;

	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		// Clean the back buffer and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Handles camera inputs
		scene.cam.Inputs(window);
        
        val = glGetError();
        if (val) std::cout << "camera " << val << std::endl;

        // Render all objects
        scene.render();

        val = glGetError();
        if (val) std::cout << "scene "  << val << std::endl;

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