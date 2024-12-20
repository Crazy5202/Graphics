#include "Scene.hpp"
#include <filesystem>

int main() {

	glfwInit();

	GLFWwindow* window = glfwCreateWindow(1920, 1080, "OpenGL", NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	glewInit();

    Scene scene(std::filesystem::current_path().parent_path().string());
    
    std::cout << "END!";
}