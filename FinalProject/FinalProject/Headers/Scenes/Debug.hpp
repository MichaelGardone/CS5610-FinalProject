#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <System.h>
#include <Shader.hpp>
#include <Camera.hpp>
#include <Model.hpp>
#include <GBuffer.hpp>

class Debug
{
public:
	Debug()
	{
		debug = new Shader("Shaders/Debug/debug.vert", "Shaders/Debug/debug.frag");
		debug->use();
		debug->setInt("tex", 0);

		geomPass = new Shader("Shaders/DeferredShading/Buffer/buffer.vert",
			"Shaders/DeferredShading/Buffer/buffer.frag");

		teapot = new Model("Objects/teapot.obj");

		camera = new Camera(glm::vec3(0.0f, 0.0f, 5.0f));
	}

	~Debug()
	{
		delete debug;
		delete teapot;
		delete geomPass;
		delete camera;
	}

	void Draw()
	{
		gBuffer.Bind();

		glm::mat4 proj = glm::perspective(glm::radians(camera->zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.f);
		glm::mat4 view = camera->getViewMatrix();
		glm::mat4 model = glm::mat4(1.f);
		model = glm::translate(model, glm::vec3(0.0, -1.f, 0.0));
		model = glm::rotate(model, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
		model = glm::scale(model, glm::vec3(0.1f));

		geomPass->use();
		geomPass->setMat4("projection", proj);
		geomPass->setMat4("view", view);
		geomPass->setMat4("model", model);

		teapot->Draw(*geomPass);

		gBuffer.Unbind();

		debug->use();
		glActiveTexture(GL_TEXTURE0);
		if (shouldRender == 0)
			glBindTexture(GL_TEXTURE_2D, gBuffer.getPosition());
		else if (shouldRender == 1)
			glBindTexture(GL_TEXTURE_2D, gBuffer.getNormal());
		else if (shouldRender == 2)
			glBindTexture(GL_TEXTURE_2D, gBuffer.getAlbedo());
		RenderQuad();
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

	void processInput(GLFWwindow *window)
	{
		if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && shouldRender != 0)
			shouldRender = 0;
		if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS && shouldRender != 1)
			shouldRender = 1;
		if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS && shouldRender != 2)
			shouldRender = 2;
	}

private:
	Shader* geomPass;
	Shader* debug;
	Model* teapot;
	Camera* camera;
	GBuffer gBuffer;

	unsigned int quadVAO;
	unsigned int quadVBO;

	unsigned char shouldRender = 0;

	float lastX = (float)WIDTH / 2.0;
	float lastY = (float)HEIGHT / 2.0;
	bool firstMouse = true;

	void RenderQuad()
	{
		if (quadVAO == 0)
		{
			float quadVertices[] = {
				// positions        // texture Coords
				-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
				-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
				 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
				 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
			};
			// setup plane VAO
			glGenVertexArrays(1, &quadVAO);
			glGenBuffers(1, &quadVBO);
			glBindVertexArray(quadVAO);
			glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		}
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	}

};
