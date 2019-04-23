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
		camera = new Camera(glm::vec3(0.0f, 0.0f, 20.0f));

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
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glm::mat4 proj = glm::perspective(glm::radians(camera->zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 600.f);
		glm::mat4 view = camera->getViewMatrix();
		
		forwardRender->use();
		forwardRender->setMat4("projection", proj);
		forwardRender->setMat4("view", view);

		forwardRender->setVec3("lightPos", glm::vec3(0.f, 1.f, 0.f));
		forwardRender->setVec3("lightColor", glm::vec3(1.f, 1.f, 1.f));

		for (int i = 0; i < numOfTeapots; i++)
		{
			glm::mat4 model = glm::mat4(1.f);
			model = glm::translate(model, objectPositions[i]);
			model = glm::scale(model, glm::vec3(0.05f));
			forwardRender->setMat4("model", model);

			teapot->Draw(*forwardRender);
		}

	}

	void processInput(GLFWwindow *window)
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera->processKeyboardInput(FORWARD, deltaTime);
		if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera->processKeyboardInput(LEFT, deltaTime);
		if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera->processKeyboardInput(RIGHT, deltaTime);
		if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera->processKeyboardInput(BACKWARD, deltaTime);
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

		camera->processMouseMovement(xoffset, yoffset, false);
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

	int numOfTeapots = 1000;

	float deltaTime, lastFrame;

	std::vector<glm::vec3> objectPositions;

	void PrepPositions()
	{
		for (int i = 0; i < numOfTeapots / 5; i++)
		{
			for (int j = 0; j < numOfTeapots / 100; j++)
			{
				objectPositions.push_back(glm::vec3(-5.f + 1.f * i, 0.f, 2.f * j));
			}
		}
	}

};
