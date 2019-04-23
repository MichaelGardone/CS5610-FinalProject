#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <System.h>
#include <Shader.hpp>
#include <Camera.hpp>
#include <Model.hpp>

class ForwardRenderHeavy
{
public:
	ForwardRenderHeavy()
	{
		forwardRender = new Shader("Shaders/ForwardShading/VertexShader.txt", "Shaders/ForwardShading/FragmentShader.txt");
		teapot = new Model("Objects/teapot.obj");
		camera = new Camera(glm::vec3(0.0f, 0.0f, 5.0f));

		PrepPositions();
	}

	~ForwardRenderHeavy()
	{
		delete forwardRender;
		delete teapot;
		delete camera;
	}

	void Draw()
	{
		forwardRender->use();

		for (int i = 0; i < numOfTeapots; i++)
		{

			teapot->Draw(*forwardRender);
		}

	}

	void processInput(GLFWwindow *window)
	{

	}

	void mouse_callback(GLFWwindow* window, double xpos, double ypos)
	{
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos;

		lastX = xpos;
		lastY = ypos;

		camera->processMouseMovement(xoffset, yoffset);
	}

	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
		camera->processMouseScroll(yoffset);
	}

private:
	Shader* forwardRender;
	Camera* camera;
	Model* teapot;

	float lastX = (float)WIDTH / 2.0;
	float lastY = (float)HEIGHT / 2.0;
	bool firstMouse = true;

	int numOfTeapots = 500; // Total = 1000 (500 in front of, 500 behind

	std::vector<glm::vec3> objectPositions;

	void PrepPositions()
	{
		for (int i = 0; i < numOfTeapots / 5; i++)
		{
			for (int j = 0; j < numOfTeapots / 100; j++)
			{
				objectPositions.push_back(glm::vec3(5.f * i, 0.f, 5.f * j));
			}
		}
	}

};
