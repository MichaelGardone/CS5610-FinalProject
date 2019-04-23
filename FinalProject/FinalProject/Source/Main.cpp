#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <System.h>

#include <Scenes/ForwardRenderingScene.hpp>
#include <Scenes/DeferredShading.hpp>
#include <Scenes/Debug.hpp>
#include <Scenes/ForwardRenderHeavy.hpp>
#include <Scenes/DeferredShadingHeavy.hpp>
#include <Scenes/DeferredLight.hpp>

#include <iostream>

// Callbacks
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// Scenes
ForwardRender* fr;
Debug* debug;
DeferredShader* ds;
ForwardRenderHeavy* frh;
DeferredHeavy* dh;
DeferredLight* dp;

unsigned char currScene = 1;

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
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);

	// == END SETUP ==

	fr = new ForwardRender();
	ds = new DeferredShader();
	debug = new Debug();
	frh = new ForwardRenderHeavy();
	dh = new DeferredHeavy();
	dp = new DeferredLight();

	std::cout << "Loaded forward shading scene" << std::endl;
	
	//Set a background color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	//Main Loop
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		//Clear color buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (currScene == 1)
			fr->Draw();
		else if (currScene == 2)
			ds->Draw();
		else if (currScene == 3)
			debug->Draw();
		else if (currScene == 4)
			frh->Draw();
		else if (currScene == 5)
			dh->Draw();
		else if (currScene == 6)
			dp->Draw();

		// Window events and swapping buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Clean up
	delete fr;
	delete ds;
	delete debug;
	delete dh;
	delete dp;

	//Close OpenGL window and terminate GLFW  
	glfwDestroyWindow(window);

	//Finalize and clean up GLFW  
	glfwTerminate();

	return 0;
}

// ==== CALLBACKS ==== //
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && currScene != 1)
	{
		currScene = 1; // Forward shading scene
		dp->StencilOff();
		std::cout << "Loaded forward shading scene" << std::endl;
	}
	else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && currScene != 2)
	{
		currScene = 2; // Deferred Shading scene
		dp->StencilOff();
		std::cout << "Loaded deferred shading scene" << std::endl;
	}
	else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && currScene != 3)
	{
		currScene = 3; // Debug shading scene
		dp->StencilOff();
		std::cout << "Loaded debug deferred shading scene" << std::endl;
	}
	else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS && currScene != 4)
	{
		currScene = 4; // Forward shading w/ lots of teapots
		dp->StencilOff();
		std::cout << "Loaded forward shading scene with lots of teapots" << std::endl;
	}
	else if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS && currScene != 5)
	{
		currScene = 5; // Deferred Shading w/ lots of teapots and text
		dp->StencilOff();
		std::cout << "Loaded deferred shading scene with lots of teapots" << std::endl;
	}
	else if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS && currScene != 6)
	{
		currScene = 6; // Deferred Shading w/ Post Processing
		dp->StencilOn();
		std::cout << "Loaded deferred shading scene with post processing" << std::endl;
	}

	if (currScene == 1)
		fr->processInput(window);
	else if (currScene == 2)
		ds->processInput(window);
	else if (currScene == 3)
		debug->processInput(window);
	else if (currScene == 4)
		frh->processInput(window);
	else if (currScene == 5)
		dh->processInput(window);
	else if (currScene == 6)
		dp->processInput(window);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (currScene == 1)
		fr->mouse_callback(window, xpos, ypos);
	else if (currScene == 2)
		ds->mouse_callback(window, xpos, ypos);
	else if (currScene == 3)
		debug->mouse_callback(window, xpos, ypos);
	else if (currScene == 4)
		frh->mouse_callback(window, xpos, ypos);
	else if (currScene == 5)
		dh->mouse_callback(window, xpos, ypos);
	else if (currScene == 6)
		dp->mouse_callback(window, xpos, ypos);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (currScene == 1)
		fr->scroll_callback(window, xoffset, yoffset);
	else if (currScene == 2)
		ds->scroll_callback(window, xoffset, yoffset);
	else if (currScene == 3)
		debug->scroll_callback(window, xoffset, yoffset);
	else if (currScene == 4)
		frh->scroll_callback(window, xoffset, yoffset);
	else if (currScene == 5)
		dh->scroll_callback(window, xoffset, yoffset);
	else if (currScene == 6)
		dp->scroll_callback(window, xoffset, yoffset);
}