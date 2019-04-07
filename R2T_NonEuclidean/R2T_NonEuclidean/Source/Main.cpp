#include <Windows.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "cyCore.h"
#include "cyTriMesh.h"
#include "cyMatrix.h"
#include "cyGL.h"

#include <iostream>
#include <fstream>
#include <sstream>

// Callbacks  
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void mouse_callback(GLFWwindow* window, int button, int action, int mods);
static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

// Projection/Screen
GLuint width = 1200, height = 800;

int main(int argc, char **argv)
{
	//Initialize GLFW  
	if (!glfwInit())
	{
		return 1;
	}

	//Declare a window object  
	GLFWwindow* window;

	// Create a window and create its OpenGL context  
	window = glfwCreateWindow(width, height, "Project 8", NULL, NULL);

	if (!window)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		return 1;
	}

	glfwMakeContextCurrent(window);

	// Sets the key callback  
	glfwSetKeyCallback(window, key_callback);

	// Set the mouse button callback
	glfwSetMouseButtonCallback(window, mouse_callback);

	// Set the mouse move callback
	glfwSetCursorPosCallback(window, cursor_position_callback);

	//Initialize GLEW
	GLenum err = glewInit();

	//If GLEW hasn't initialized  
	if (err != GLEW_OK)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		return -1;
	}

	glEnable(GL_DEPTH_TEST);

	std::cout << glGetString(GL_VERSION) << std::endl;

	// == END SETUP ==


	//Main Loop
	while (!glfwWindowShouldClose(window))
	{
		//Set a background color
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

		//Clear color buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Render scene here


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

// CALL BACKS
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

static void mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
	
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{

	
	
}
