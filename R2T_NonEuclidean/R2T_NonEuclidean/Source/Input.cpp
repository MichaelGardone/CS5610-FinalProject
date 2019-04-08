#include "Input.h"

Input::Input() {}

void Input::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	//if((key == GLFW_KEY_W || key == GLFW_KEY_UP) && action == GLFW_REPEAT)
		// go forward
	
	//if((key == GLFW_KEY_S || key == GLFW_KEY_DOWN) && action == GLFW_REPEAT)
		// go back

	//if((key == GLFW_KEY_A || key == GLFW_KEY_LEFT) && action == GLFW_REPEAT)
		// go left

	//if((key == GLFW_KEY_D || key == GLFW_KEY_RIGHT) && action == GLFW_REPEAT)
		// go right
	
}

void Input::mouse_callback(GLFWwindow* window, int button, int action, int mods)
{

}

void Input::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{

}
