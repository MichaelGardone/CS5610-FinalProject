#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>

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
		forwardRender2 = new Shader("Shaders/ForwardShading/Lights/VS.vert", "Shaders/ForwardShading/Lights/FS.frag");
		teapot = new Model("Objects/teapot.obj");
		camera = new Camera(glm::vec3(0.0f, 0.0f, 5.0f));

		PrepLights();
	}

	~ForwardRender()
	{
		delete teapot;
		delete camera;
		delete forwardRender;
		delete forwardRender2;
	}

	virtual void Draw()
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glm::mat4 proj = glm::perspective(glm::radians(camera->zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.f);
		glm::mat4 view = camera->getViewMatrix();
		glm::mat4 model = glm::mat4(1.f);
		model = glm::translate(model, glm::vec3(0.0, -1.f, 0.0));
		model = glm::rotate(model, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
		model = glm::scale(model, glm::vec3(0.1f));

		if (!lotsOfLights)
		{
			forwardRender->use();
			forwardRender->setMat4("projection", proj);
			forwardRender->setMat4("view", view);
			forwardRender->setMat4("model", model);
			forwardRender->setVec3("viewPos", camera->position);

			forwardRender->setVec3("lightPos", glm::vec3(0.f, 0.f, 2.f));
			forwardRender->setVec3("lightColor", glm::vec3(1.f, 1.f, 1.f));

			teapot->Draw(*forwardRender);
		}
		else {
			forwardRender2->use();
			forwardRender2->setMat4("projection", proj);
			forwardRender2->setMat4("view", view);
			forwardRender2->setMat4("model", model);
			forwardRender2->setVec3("viewPos", camera->position);

			for (unsigned int i = 0; i < lightPositions.size(); i++)
			{
				forwardRender2->setVec3("lights[" + std::to_string(i) + "].Position", lightPositions[i]);
				forwardRender2->setVec3("lights[" + std::to_string(i) + "].Color", lightColors[i]);

				const float constant = 1.0;
				const float linear = 0.7;
				const float quadratic = 1.8;
				forwardRender2->setFloat("lights[" + std::to_string(i) + "].Linear", linear);
				forwardRender2->setFloat("lights[" + std::to_string(i) + "].Quadratic", quadratic);

				const float maxBrightness = std::fmaxf(std::fmaxf(lightColors[i].r, lightColors[i].g), lightColors[i].b);
				float radius = (-linear + std::sqrt(linear * linear - 4 * quadratic * (constant - (256.0f / 5.0f) * maxBrightness))) / (2.0f * quadratic);
				forwardRender2->setFloat("lights[" + std::to_string(i) + "].Radius", radius);
			}

			teapot->Draw(*forwardRender2);
		}

	}

	void processInput(GLFWwindow *window)
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera->processKeyboardInput(FORWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera->processKeyboardInput(LEFT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera->processKeyboardInput(RIGHT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera->processKeyboardInput(BACKWARD, deltaTime);

		if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && !lotsOfLights)
			lotsOfLights = true;
		else if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && lotsOfLights)
			lotsOfLights = false;
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
	Shader* forwardRender2;
	Camera* camera;
	Model* teapot;

	float deltaTime, lastFrame;

	bool lotsOfLights = false;

	float lastX = (float)WIDTH / 2.0;
	float lastY = (float)HEIGHT / 2.0;
	bool firstMouse = true;

	std::vector<glm::vec3> lightPositions;
	std::vector<glm::vec3> lightColors;

	void PrepLights()
	{
		const unsigned int NR_LIGHTS = 32;
		srand(13);
		for (unsigned int i = 0; i < NR_LIGHTS; i++)
		{
			// calculate slightly random offsets
			float xPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
			float yPos = ((rand() % 100) / 100.0) * 6.0 - 4.0;
			float zPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
			lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
			// also calculate random color
			float rColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
			float gColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
			float bColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
			lightColors.push_back(glm::vec3(rColor, gColor, bColor));
		}
	}
};
