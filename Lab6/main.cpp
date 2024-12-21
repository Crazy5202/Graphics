#include "Scene.hpp"
#include <filesystem>

#include"imgui.h"
#include"imgui_impl_glfw.h"
#include"imgui_impl_opengl3.h"

float FOV = 75.0f;

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if (yoffset > 0) {
        FOV = std::max(FOV - 5.0f, 30.0f);
    } else if (yoffset < 0) {
		FOV = std::min(FOV + 5.0f, 90.0f);
    }
}

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

	//glfwSwapInterval( 0 );
	
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
    
    int light_type = 3;
	int proj_type = 2;
	glUniform3fv(glGetUniformLocation(scene.shader.ID, "lightColor"), 1, glm::value_ptr(scene.lightsrc.color));
	glUniform3fv(glGetUniformLocation(scene.shader.ID, "lightPos"), 1, glm::value_ptr(scene.lightsrc.position));
	glUniform3fv(glGetUniformLocation(scene.shader.ID, "lightPointed"), 1, glm::value_ptr(scene.lightsrc.direction));
	glUniform2f(glGetUniformLocation(scene.shader.ID, "fade"), scene.lightsrc.fade[0], scene.lightsrc.fade[1]);
    
    val = glGetError();
    if (val) std::cout << "passing_to_shader_main " << val << std::endl;

	glfwSetScrollCallback(window, scroll_callback);

	// Initialize ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	// Variables to create periodic event for FPS displaying
	double prevTime = 0.0;
	double crntTime = 0.0;
	double timeDiff;
	// Keeps track of the amount of frames in timeDiff
	unsigned int counter = 0;
	float FPS = 0.0;

	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		crntTime = glfwGetTime();
		timeDiff = crntTime - prevTime;
		counter++;

		// Clean the back buffer and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Tell OpenGL a new frame is about to begin
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Handles camera inputs
		scene.cam.FOV = FOV;
		scene.cam.Inputs(window);

        val = glGetError();
        if (val) std::cout << "camera " << val << std::endl;

        // Render all objects
        scene.render();

        val = glGetError();
        if (val) std::cout << "scene "  << val << std::endl;

		// ImGUI window creation
		ImGui::SetNextWindowPos(ImVec2(10, 20), ImGuiCond_Always);
		ImGui::Begin("Graphics settings", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
		ImGui::SetWindowSize(ImVec2(350.0f, 100.0f));
		// Slider that appears in the window
		ImGui::SliderInt("Lighting type", &light_type, 1, 3);
		ImGui::SliderInt("Projection type", &proj_type, 1, 2);
		// Ends the window
		ImGui::End();

		if (timeDiff >= 1.0 / 30.0)
		{
			// Creates new title
			FPS = (1.0 / timeDiff) * counter;

			// Resets times and counter
			prevTime = crntTime;
			counter = 0;
		}

		ImGui::SetNextWindowPos(ImVec2(width - 100, 20), ImGuiCond_Always);
        ImGui::Begin("FPS Display", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
		ImGui::SetWindowSize(ImVec2(100.0f, 30.0f));
		ImGui::Text("FPS: %.2f", FPS);
        ImGui::End();

		ImGui::SetNextWindowPos(ImVec2(width - 100, 60), ImGuiCond_Always);
        ImGui::Begin("Objects on the scene", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
        ImGui::SetWindowSize(ImVec2(100.0f, 30.0f));
		ImGui::Text("Objects: %d", scene.meshes.size());
        ImGui::End();

		if (proj_type==1) scene.cam.updateMat(scene.cam.FOV, proj_type-1);
		else (scene.cam.updateMat(scene.cam.FOV, 1));

		glUniform1i(glGetUniformLocation(scene.shader.ID, "lightType"), light_type);

		// Renders the ImGUI elements
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}

	// Deletes all ImGUI instances
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	// Delete all the objects we've created
	scene.shader.Delete();
	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}