#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <System.h>
#include <Shader.hpp>
#include <Camera.hpp>
#include <Model.hpp>

#include <iostream>

// Callbacks  
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow *window);

int main(int argc, char **argv)
{
	glfwInit();

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "CS5610 Interactive Computer Graphics - Deferred Shader", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);

	// == END SETUP ==

	//Shader geomPass("Source/Shaders/DeferredShading/Buffer/buffer.vert",
	//	"Source/Shaders/DeferredShading/Buffer/buffer.frag");
	//Shader lightPass("Source/Shaders/DeferredShading/BlinnPhong/deferred_shader.vert",
	//	"Source/Shaders/DeferredShading/BlinnPhong/deferred_shader.vert");

	//Set a background color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	//Main Loop
	while (!glfwWindowShouldClose(window))
	{
		//Clear color buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		processInput(window);

		// Window events and swapping buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//Close OpenGL window and terminate GLFW  
	glfwDestroyWindow(window);

	//Finalize and clean up GLFW  
	glfwTerminate();

	return 0;
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}