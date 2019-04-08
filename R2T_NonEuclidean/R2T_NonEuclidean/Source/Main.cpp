#include <Windows.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Input.h"
#include "SceneManager.h"

#include <iostream>
#include <fstream>
#include <sstream>

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
	window = glfwCreateWindow(width, height, "Non-Euclidean Rendering", NULL, NULL);

	if (!window)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		return 1;
	}

	glfwMakeContextCurrent(window);

	// Take the input callbacks
	Input input;

	// Sets the key callback  
	glfwSetKeyCallback(window, input.key_callback);

	// Set the mouse button callback
	glfwSetMouseButtonCallback(window, input.mouse_callback);

	// Set the mouse move callback
	glfwSetCursorPosCallback(window, input.cursor_position_callback);

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

	SceneManager sm;
	
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
