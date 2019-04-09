#ifndef _SCENE_1_H_
#define _SCENE_1_H_

#include <Windows.h>
#include <GL/glew.h>

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

		gbuffer.add_buffer("position");
		gbuffer.add_buffer("normals");
		gbuffer.add_buffer("albedo");

		gbuffer.generate_attachments();
		gbuffer.generate_depthbuffer();

		Load();
	}

	void Load()
	{
		LoadLightShader();
		LoadGeometryPass();
		PhongBlinnPass();
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
		geom_pass.RegisterUniform(3, "model");
		geom_pass.RegisterUniform(4, "model");
		geom_pass.RegisterUniform(2, "model");

		std::cout << "Registered all variables successfully!" << std::endl;
	}

	void PhongBlinnPass()
	{
		std::cout << "Compiling Phong-Blinn Pass Shader" << std::endl;

		phongblinn_pass.CreateProgram();

		if (!pb_vert.CompileFile("Source/Shaders/PhongBlinnPass/lightingpass_vertex.vert", GL_VERTEX_SHADER))
		{
			std::cout << "Failed to compile Phong-Blinn pass vertex shader." << std::endl;
			return;
		}

		if (!pb_frag.CompileFile("Source/Shaders/PhongBlinnPass/lightingpass_fragment.frag", GL_FRAGMENT_SHADER))
		{
			std::cout << "Failed to compile Phong-Blinn pass fragment shader." << std::endl;
			return;
		}

		phongblinn_pass.AttachShader(geom_vert);
		phongblinn_pass.AttachShader(geom_frag);

		phongblinn_pass.Link();

		std::cout << "Finished compiling succesfully. Beginning registration..." << std::endl;

		phongblinn_pass.RegisterUniform(0, "viewPos");
		phongblinn_pass.RegisterUniform(1, "lights");

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
		geom_pass.SetUniform("model", cyMatrix4f(1.f));

		teapot.draw();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// 2 - Lighting pass
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 3 - Apply texture


		// 4 - Rendering the light cubes

	}

private:
	GBuffer gbuffer;

	cyGLSLProgram light_shader;
	cyGLSLShader light_vert, light_frag;

	cyGLSLProgram geom_pass;
	cyGLSLShader geom_vert, geom_frag;

	cyGLSLProgram phongblinn_pass;
	cyGLSLShader pb_vert, pb_frag;
	
	//(cyPoint3f(-0.5f, 0.f, 2.f), cyPoint3f(0.f, 0.f, 0.f), cyPoint3f(0.f, 1.f, 0.f))
	Light light1;

	//(cyPoint3f(0.f, 0.f, 3.f), cyPoint3f(0.f, 1.f, 0.f), cyPoint3f(0.f, 0.f, -1.f))
	Camera camera;
	
	Model teapot("teapot", "Source/objects/teapot.obj");
};

#endif