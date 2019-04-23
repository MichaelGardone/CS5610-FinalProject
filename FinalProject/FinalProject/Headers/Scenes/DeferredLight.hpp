#pragma once

#include <vector>

#include <System.h>
#include <Shader.hpp>
#include <Camera.hpp>
#include <Model.hpp>
#include <GBuffer.hpp>
#include <Stencil.hpp>

class DeferredLight
{
public:

	DeferredLight()
	{
		geomPass = new Shader("Shaders/DeferredShading/Buffer/buffer.vert",
			"Shaders/DeferredShading/Buffer/buffer.frag");
		lightPass = new Shader("Shaders/DeferredShading/BlinnPhong/deferred_shader.vert",
			"Shaders/DeferredShading/BlinnPhong/deferred_shader.frag");
		//outlinePass = new Shader("Shaders/PostProcessing/StencilBuffer/Stencil.vert",
		//	"Shaders/PostProcessing/StencilBuffer/Stencil.frag");

		teapot = new Model("Objects/teapot.obj");

		lightPass->use();
		lightPass->setInt("gPosition", 0);
		lightPass->setInt("gNormal", 1);
		lightPass->setInt("gAlbedoSpec", 2);

		camera = new Camera(glm::vec3(0.0f, 0.0f, 15.0f));

		PrepLights();
		PrepPositions();
	}

	~DeferredLight()
	{
		delete geomPass;
		delete lightPass;
		delete teapot;
		delete camera;
		delete outlinePass;
	}

	void Draw()
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Write to a texture since we are using deferred
		/*sbuffer.Bind();
		// Write
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);
		glEnable(GL_STENCIL_TEST);

		glm::mat4 proj = glm::perspective(glm::radians(camera->zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.f);
		glm::mat4 view = camera->getViewMatrix();
		glm::mat4 model = glm::mat4(1.f);

		outlinePass->use();
		outlinePass->setMat4("projection", proj);
		outlinePass->setMat4("view", view);
		for (int i = 0; i < numOfTeapots; i++)
		{
			glm::mat4 model = glm::mat4(1.f);
			model = glm::translate(model, objectPositions[i]);
			model = glm::scale(model, glm::vec3(0.07f));

			outlinePass->setMat4("model", model);

			teapot->Draw(*outlinePass);
		}

		sbuffer.Unbind();*/

		gbuffer.Bind();
		// Render as normal
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
		glm::mat4 proj = glm::perspective(glm::radians(camera->zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.f);
		glm::mat4 view = camera->getViewMatrix();
		glm::mat4 model = glm::mat4(1.f);

		geomPass->use();
		geomPass->setMat4("projection", proj);
		geomPass->setMat4("view", view);
		for (int i = 0; i < numOfTeapots; i++)
		{
			model = glm::mat4(1.f);
			model = glm::translate(model, objectPositions[i]);
			model = glm::scale(model, glm::vec3(0.05f));

			geomPass->setMat4("model", model);

			teapot->Draw(*geomPass);
		}
		gbuffer.Unbind();

		lightPass->use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gbuffer.getPosition());
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gbuffer.getNormal());
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gbuffer.getAlbedo());
		for (unsigned int i = 0; i < lightPositions.size(); i++)
		{
			lightPass->setVec3("lights[" + std::to_string(i) + "].Position", lightPositions[i]);
			lightPass->setVec3("lights[" + std::to_string(i) + "].Color", lightColors[i]);

			const float constant = 1.0;
			const float linear = 0.7;
			const float quadratic = 1.8;
			lightPass->setFloat("lights[" + std::to_string(i) + "].Linear", linear);
			lightPass->setFloat("lights[" + std::to_string(i) + "].Quadratic", quadratic);

			const float maxBrightness = std::fmaxf(std::fmaxf(lightColors[i].r, lightColors[i].g), lightColors[i].b);
			float radius = (-linear + std::sqrt(linear * linear - 4 * quadratic * (constant - (256.0f / 5.0f) * maxBrightness))) / (2.0f * quadratic);
			lightPass->setFloat("lights[" + std::to_string(i) + "].Radius", radius);
		}
		lightPass->setVec3("viewPos", camera->position);
		renderQuad();

		sbuffer.Blit();
		gbuffer.Blit();

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

	void StencilOn()
	{
		//glEnable(GL_STENCIL_TEST);
		//glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		//glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	}

	void StencilOff()
	{
		//glDisable(GL_STENCIL_TEST);
		//glEnable(GL_DEPTH_TEST);
	}

private:
	Shader* geomPass;
	Shader* lightPass;
	Shader* outlinePass;
	Camera* camera;
	Model* teapot;

	GBuffer gbuffer;
	SBuffer sbuffer;

	unsigned int quadVAO;
	unsigned int quadVBO;

	float lastX = (float)WIDTH / 2.0;
	float lastY = (float)HEIGHT / 2.0;
	bool firstMouse = true;

	int numOfTeapots = 100;

	float deltaTime, lastFrame;

	std::vector<glm::vec3> objectPositions;

	void PrepPositions()
	{
		for (int i = 0; i < numOfTeapots / 4; i++)
		{
			for (int j = 0; j < numOfTeapots / 25; j++)
			{
				objectPositions.push_back(glm::vec3(-5.f + 1.f * i, 0.f, 2.f * j));
			}
		}
	}

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

	void renderQuad()
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
