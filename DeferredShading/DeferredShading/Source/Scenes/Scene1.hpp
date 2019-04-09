#ifndef _SCENE_1_H_
#define _SCENE_1_H_

#include <Windows.h>
#include <GL/glew.h>
#include <string>

#include "cyCore.h"
#include "cyTriMesh.h"
#include "cyMatrix.h"
#include "cyPoint.h"
#include "cyGL.h"

#include "Scene.h"
#include "Camera.hpp"
#include "Light.hpp"
#include "System.h"
#include "GBuffer.h"

#include "Model.hpp"

class Scene1 : public Scene
{
public:
	Scene1()
	{
		gbuffer.resize_gbuffer(3);
		gbuffer.resize_attachments(3);

		std::cout << "GBuffer Size: " << gbuffer.get_gbuffer_size() << "; GBuffer Attachment Size: " << gbuffer.get_attach_size()
			<< std::endl;

		gbuffer.add_buffer("position\0");
		gbuffer.add_buffer("normals\0");
		gbuffer.add_buffer("albedo\0");

		std::cout << "position texture: " << gbuffer.get_buffer("position\0") << "; normals texture: " << gbuffer.get_buffer("normals\0")
			<< "; albedo: " << gbuffer.get_buffer("albedo\0") << std::endl;

		gbuffer.generate_attachments();
		gbuffer.generate_depthbuffer();

		camera.set_position(cyPoint3f(0.f, 0.f, 5.f));

		Load();
	}

	void Load()
	{
		LoadLightShader();
		LoadGeometryPass();
		BlinnPhongPass();

		teapot.LoadModel("Source/objects/teapot.obj");
		quad.LoadQuad();

		LoadLights();
	}

	void LoadLights()
	{
		srand(13);
		for (int i = 0; i < 32; i++)
		{
			float xPos = (((rand()) % 100) / 100.f) * 6.f - 3.f;
			float yPos = (((rand()) % 100) / 100.f) * 6.f - 4.f;
			float zPos = (((rand()) % 100) / 100.f) * 6.f - 3.f;
			this->lightPositions.push_back(cyPoint3f(xPos, yPos, zPos));
			float rColor = ((rand() % 100) / 200.0f) + 0.5;
			float gColor = ((rand() % 100) / 200.0f) + 0.5;
			float bColor = ((rand() % 100) / 200.0f) + 0.5;
			lightColor.push_back(cyPoint3f(rColor, gColor, bColor));
		}
	}

	void LoadLightShader()
	{
		std::cout << "Compiling Light Shader" << std::endl;

		light_shader.CreateProgram();

		if (!light_vert.CompileFile("Source/Shaders/LightShader/lightbox_vert.vert", GL_VERTEX_SHADER))
		{
			std::cout << "Failed to compile light vertex shader." << std::endl;
			return;
		}

		if (!light_frag.CompileFile("Source/Shaders/LightShader/lightbox_fragment.frag", GL_FRAGMENT_SHADER))
		{
			std::cout << "Failed to compile light fragment shader." << std::endl;
			return;
		}

		light_shader.AttachShader(light_vert);
		light_shader.AttachShader(light_frag);

		light_shader.Link();

		std::cout << "Finished compiling succesfully. Beginning registration..." << std::endl;

		light_shader.RegisterUniform(0, "proj");
		light_shader.RegisterUniform(1, "view");
		light_shader.RegisterUniform(2, "model");
		light_shader.RegisterUniform(3, "lightColor");

		std::cout << "Registered all variables successfully!" << std::endl;
	}

	void LoadGeometryPass()
	{
		std::cout << "Compiling Geometry Pass Shader" << std::endl;

		geom_pass.CreateProgram();

		if (!geom_vert.CompileFile("Source/Shaders/GeometryPass/gbuffer_vertex.vert", GL_VERTEX_SHADER))
		{
			std::cout << "Failed to compile geometry pass vertex shader." << std::endl;
			return;
		}

		if (!geom_frag.CompileFile("Source/Shaders/GeometryPass/gbuffer_fragment.frag", GL_FRAGMENT_SHADER))
		{
			std::cout << "Failed to compile geometry pass fragment shader." << std::endl;
			return;
		}

		geom_pass.AttachShader(geom_vert);
		geom_pass.AttachShader(geom_frag);

		geom_pass.Link();

		std::cout << "Finished compiling succesfully. Beginning registration..." << std::endl;

		geom_pass.RegisterUniform(0, "proj");
		geom_pass.RegisterUniform(1, "view");
		geom_pass.RegisterUniform(2, "model");
		geom_pass.RegisterUniform(3, "texture_diffuse");
		geom_pass.RegisterUniform(4, "texture_specular");

		std::cout << "Registered all variables successfully!" << std::endl;
	}

	void BlinnPhongPass()
	{
		std::cout << "Compiling Blinn-Phong Pass Shader" << std::endl;

		blinnphong_pass.CreateProgram();

		if (!bp_vert.CompileFile("Source/Shaders/PhongBlinnPass/lightingpass_vertex.vert", GL_VERTEX_SHADER))
		{
			std::cout << "Failed to compile Phong-Blinn pass vertex shader." << std::endl;
			return;
		}

		if (!bp_frag.CompileFile("Source/Shaders/PhongBlinnPass/lightingpass_fragment.frag", GL_FRAGMENT_SHADER))
		{
			std::cout << "Failed to compile Phong-Blinn pass fragment shader." << std::endl;
			return;
		}

		blinnphong_pass.AttachShader(bp_vert);
		blinnphong_pass.AttachShader(bp_frag);

		blinnphong_pass.Link();

		std::cout << "Finished compiling succesfully. Beginning registration..." << std::endl;

		blinnphong_pass.RegisterUniform(0, "viewPos");

		std::cout << "Registered all variables successfully!" << std::endl;
	}

	void Draw()
	{
		// 1 - Geometry pass
		glBindFramebuffer(GL_FRAMEBUFFER, gbuffer.get_gbuffer());

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		geom_pass.Bind();
		geom_pass.SetUniform("proj", camera.get_perspective_matrix());
		geom_pass.SetUniform("view", camera.get_view_matrix());
		geom_pass.SetUniform("model", teapot.get_model_matrix());
		
		teapot.Draw();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// 2 - Lighting pass
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		blinnphong_pass.Bind();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gbuffer.get_buffer("position"));
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gbuffer.get_buffer("normals"));
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gbuffer.get_buffer("albedo"));
		
		for (int i = 0; i < 32; i++)
		{
			std::string pos = "lights[" + std::to_string(i) + "].position";
			blinnphong_pass.SetUniform(pos.c_str(), lightPositions[i]);
			std::string col = "lights[" + std::to_string(i) + "].color";
			blinnphong_pass.SetUniform(col.c_str(), lightColor[i]);
			const float constant = 1.0;
			const float linear = 0.7;
			const float quadratic = 1.8;
			std::string line = "lights[" + std::to_string(i) + "].linear";
			blinnphong_pass.SetUniform(line.c_str(), linear);
			std::string quad = "lights[" + std::to_string(i) + "].quadratic";
			blinnphong_pass.SetUniform(quad.c_str(), quadratic);
		}
		blinnphong_pass.SetUniform(0, camera.get_position());

		quad.Draw();

		// 3 - Apply texture
		glBindFramebuffer(GL_READ_FRAMEBUFFER, gbuffer.get_gbuffer());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	}

private:
	GBuffer gbuffer;

	cyGLSLProgram light_shader;
	cyGLSLShader light_vert, light_frag;

	cyGLSLProgram geom_pass;
	cyGLSLShader geom_vert, geom_frag;

	cyGLSLProgram blinnphong_pass;
	cyGLSLShader bp_vert, bp_frag;
	
	//(cyPoint3f(-0.5f, 0.f, 2.f), cyPoint3f(0.f, 0.f, 0.f), cyPoint3f(0.f, 1.f, 0.f))
	Light light1;

	//(cyPoint3f(0.f, 0.f, 3.f), cyPoint3f(0.f, 1.f, 0.f), cyPoint3f(0.f, 0.f, -1.f))
	Camera camera;
	
	Model teapot;
	Model quad;

	std::vector<cyPoint3f> lightPositions;
	std::vector<cyPoint3f> lightColor;
};

#endif