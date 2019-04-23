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

class ForwardRender {
public:
	ForwardRender()
	{
		forwardRender = new Shader("Shaders/ForwardShading/VertexShader.txt", "Shaders/ForwardShading/FragmentShader.txt");
		teapot = new Model("Objects/teapot.obj");
		camera = new Camera(glm::vec3(0.0f, 0.0f, 5.0f));
	}

	~ForwardRender()
	{
		delete teapot;
		delete camera;
		delete forwardRender;
	}

	virtual void Draw()
	{
		glm::mat4 proj = glm::perspective(glm::radians(camera->zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.f);
		glm::mat4 view = camera->getViewMatrix();
		glm::mat4 model = glm::mat4(1.f);
		model = glm::translate(model, glm::vec3(0.0, 5.0, 0.0));
		model = glm::scale(model, glm::vec3(0.05f));

		forwardRender->use();
		forwardRender->setMat4("projection", proj);
		forwardRender->setMat4("view", view);
		forwardRender->setMat4("model", model);

		forwardRender->setVec3("lightPos", glm::vec3(0.f, 1.f, 0.f));
		forwardRender->setVec3("lightColor", glm::vec3(1.f, 1.f, 1.f));

		teapot->Draw(*forwardRender);
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
};
