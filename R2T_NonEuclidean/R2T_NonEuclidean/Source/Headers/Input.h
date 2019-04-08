#ifndef _INPUT_H_
#define _INPUT_H_

class Input {
public:
	Input();

	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouse_callback(GLFWwindow* window, int button, int action, int mods);
	static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
	
	float mouse_x;
	float mouse_y;
	float mouse_dx;
	float mouse_dy;

private:

};

#endif