#include <Windows.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "cyCore.h"
#include "cyTriMesh.h"
#include "cyMatrix.h"
#include "cyGL.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include "lodepng.h"

#include "Camera.hpp"
#include "Light.hpp"
#include "GMath.h"

// Callbacks  
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void mouse_callback(GLFWwindow* window, int button, int action, int mods);
static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

// Plane
GLuint vao_plane, vbo_plane;
cyMatrix4f planeModelMatrix(0.f);
unsigned img_width = 512, img_height = 512, img_width2 = 512, img_height2 = 512;
std::vector<unsigned char> pixels, pixels2;

GLuint normal_map, displ_map;

// Plane shader
cyGLSLProgram plane_shader;
cyGLSLShader plane_vert, plane_frag;

// Displacement Shader
cyGLSLProgram displ_shader;
cyGLSLShader displ_vert, displ_frag, displ_geom, displ_tcs, displ_tes;

// Wire frame
GLuint vao_wire, vbo_wire;
cyGLSLProgram wireframe;
cyGLSLShader wire_geom, wire_frag, wire_vert, wire_tcs, wire_tes;
bool activateWires = false;

// tesellation level
float currTessel = 1;

// Projection/Screen
GLuint width = 1200, height = 800;

float zNear = 0.1f, zFar = 100.f, zRange = zFar - zNear, FOV = TO_RADIANS(45.f);
float aspectRatio = (float)width / (float)height;

// Right
float lastX = 0.f, lastY = 0.f;
bool leftDown = false, rightDown = false;

Light light1(cyPoint3f(-0.5f, 0.f, 2.f), cyPoint3f(0.f, 0.f, 0.f), cyPoint3f(0.f, 1.f, 0.f));

Camera camera(cyPoint3f(0.f, 0.f, 3.f), cyPoint3f(0.f, 1.f, 0.f), cyPoint3f(0.f, 0.f, -1.f));

void loadNormals()
{
	unsigned width, height;

	std::string file("Source/objects/displacement/teapot_normal.png");

	//decode
	unsigned error = lodepng::decode(pixels, img_width, img_height, file);

	std::cout << "Image width and height: " << img_width << ", " << img_height << " loading from " << file << std::endl;
	glGenTextures(1, &normal_map);

	//if there's an error, display it
	if (error)
	{
		std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
		return;
	}

	glBindTexture(GL_TEXTURE_2D, normal_map);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_width, img_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &pixels[0]);
	glGenerateMipmap(GL_TEXTURE_2D);

	// texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glGenerateMipmap(GL_TEXTURE_2D);

	std::cout << "Decoded successfully, generated mip maps, set wrap mode, and set up texture filtering." << std::endl;
}

void loadShaders()
{
	// PLANE SHADER
	std::cout << "Starting plane_shader" << std::endl;
	plane_shader.CreateProgram();
	plane_vert.CompileFile("Source/Shaders/Plane Shaders/PlaneVS.txt", GL_VERTEX_SHADER);
	plane_frag.CompileFile("Source/Shaders/Plane Shaders/PlaneFS.txt", GL_FRAGMENT_SHADER);

	plane_shader.AttachShader(plane_vert);
	plane_shader.AttachShader(plane_frag);

	plane_shader.Link();

	// Register here
	plane_shader.RegisterUniform(0, "proj");
	plane_shader.RegisterUniform(1, "model");
	plane_shader.RegisterUniform(2, "view");
	plane_shader.RegisterUniform(3, "lightPos");
	plane_shader.RegisterUniform(4, "viewPos");
	plane_shader.RegisterUniform(5, "normalMap");
	std::cout << "Finished with plane_shader" << std::endl;

	// DISPLACEMENT MAPPING SHADER
	std::cout << "Creating displacement shader" << std::endl;
	displ_shader.CreateProgram();

	displ_vert.CompileFile("Source/Shaders/PlaneTessel/VS.txt", GL_VERTEX_SHADER);
	displ_frag.CompileFile("Source/Shaders/PlaneTessel/FS.txt", GL_FRAGMENT_SHADER);
	displ_geom.CompileFile("Source/Shaders/PlaneTessel/GS.txt", GL_GEOMETRY_SHADER);
	displ_tcs.CompileFile("Source/Shaders/PlaneTessel/TCS.txt", GL_TESS_CONTROL_SHADER);
	displ_tes.CompileFile("Source/Shaders/PlaneTessel/TES.txt", GL_TESS_EVALUATION_SHADER);

	displ_shader.AttachShader(displ_vert);
	displ_shader.AttachShader(displ_frag);
	displ_shader.AttachShader(displ_geom);
	displ_shader.AttachShader(displ_tcs);
	displ_shader.AttachShader(displ_tes);

	displ_shader.Link();

	displ_shader.RegisterUniform(0, "proj");
	displ_shader.RegisterUniform(1, "view");
	displ_shader.RegisterUniform(2, "model");
	// 3 is free
	displ_shader.RegisterUniform(4, "lightPos");
	displ_shader.RegisterUniform(5, "viewPos");
	displ_shader.RegisterUniform(6, "displMap");
	// 7 is free
	displ_shader.RegisterUniform(8, "normalMap");
	displ_shader.RegisterUniform(9, "normalMat");
	displ_shader.RegisterUniform(10, "subdivides");

	std::cout << "Finished with displacement shader" << std::endl;

	// WIREFRAME SHADER
	std::cout << "Starting wireframe shader" << std::endl;
	wireframe.CreateProgram();
	wire_vert.CompileFile("Source/Shaders/Wireframe/VertexShader.txt", GL_VERTEX_SHADER);
	wire_geom.CompileFile("Source/Shaders/Wireframe/GeometryShader.txt", GL_GEOMETRY_SHADER);
	wire_frag.CompileFile("Source/Shaders/Wireframe/FragmentShader.txt", GL_FRAGMENT_SHADER);
	wire_tcs.CompileFile("Source/Shaders/Wireframe/TCS.txt", GL_TESS_CONTROL_SHADER);
	wire_tes.CompileFile("Source/Shaders/Wireframe/TES.txt", GL_TESS_EVALUATION_SHADER);

	wireframe.AttachShader(wire_geom);
	wireframe.AttachShader(wire_frag);
	wireframe.AttachShader(wire_vert);
	wireframe.AttachShader(wire_tcs);
	wireframe.AttachShader(wire_tes);

	wireframe.Link();

	wireframe.RegisterUniform(0, "proj");
	wireframe.RegisterUniform(1, "model");
	wireframe.RegisterUniform(2, "view");
	wireframe.RegisterUniform(4, "subdivides");
	wireframe.RegisterUniform(5, "displMap");
	std::cout << "Finished wireframe shader" << std::endl;
}

void loadPlane()
{
	cyPoint3f v1(-0.75f, 0.75f, 0.f);
	cyPoint3f v2(-0.75f, -0.75f, 0.f);
	cyPoint3f v3(0.75f, -0.75f, 0.f);
	cyPoint3f v4(0.75f, 0.75f, 0.f);

	// Tex coords
	cyPoint2f uv1(0.f, 1.f);
	cyPoint2f uv2(0.f, 0.f);
	cyPoint2f uv3(1.f, 0.f);
	cyPoint2f uv4(1.f, 1.f);

	// Normals
	cyPoint3f normal(0.f, 0.f, 1.f);

	// Set up storage for tangents and bitangents
	cyPoint3f t1, b1, t2, b2;

	// T1
	cyPoint3f e1 = v2 - v1;
	cyPoint3f e2 = v3 - v1;
	cyPoint2f dUV1 = uv2 - uv1;
	cyPoint2f dUV2 = uv3 - uv1;

	float f = 1.f / (dUV1.x * dUV2.y - dUV1.y * dUV2.x);
	t1.x = f * (dUV2.y * e1.x - dUV1.y * e2.x);
	t1.y = f * (dUV2.y * e1.y - dUV1.y * e2.y);
	t1.z = f * (dUV2.y * e1.z - dUV1.y * e2.z);
	t1.Normalize();

	b1.x = f * (-dUV2.x * e1.x + dUV1.x * e2.x);
	b1.y = f * (-dUV2.x * e1.y + dUV1.x * e2.y);
	b1.z = f * (-dUV2.x * e1.z + dUV1.x * e2.z);
	b1.Normalize();

	// T2
	e1 = v3 - v1;
	e2 = v4 - v1;
	dUV1 = uv3 - uv1;
	dUV2 = uv4 - uv1;

	f = 1.f / (dUV1.x * dUV2.y - dUV1.y * dUV2.x);
	t2.x = f * (dUV2.y * e1.x - dUV1.y * e2.x);
	t2.y = f * (dUV2.y * e1.y - dUV1.y * e2.y);
	t2.z = f * (dUV2.y * e1.z - dUV1.y * e2.z);
	t2.Normalize();

	b2.x = f * (-dUV2.x * e1.x + dUV1.x * e2.x);
	b2.y = f * (-dUV2.x * e1.y + dUV1.x * e2.y);
	b2.z = f * (-dUV2.x * e1.z + dUV1.x * e2.z);
	b2.Normalize();

	float plane[] = {
		// positions       // Normal				       // uv coords   // tangent		// bitangent
		v1.x, v1.y, v1.z,  normal.z, normal.y, normal.z,  uv1.x, uv1.y,  t1.x, t1.y, t1.z, b1.x, b1.y, b1.z,
		v2.x, v2.y, v2.z,  normal.z, normal.y, normal.z,  uv2.x, uv2.y,  t1.x, t1.y, t1.z, b1.x, b1.y, b1.z,
		v3.x, v3.y, v3.z,  normal.z, normal.y, normal.z,  uv3.x, uv3.y,  t1.x, t1.y, t1.z, b1.x, b1.y, b1.z,

		v1.x, v1.y, v1.z,  normal.z, normal.y, normal.z,  uv1.x, uv1.y,  t2.x, t2.y, t2.z, b2.x, b2.y, b2.z,
		v3.x, v3.y, v3.z,  normal.z, normal.y, normal.z,  uv3.x, uv3.y,  t2.x, t2.y, t2.z, b2.x, b2.y, b2.z,
		v4.x, v4.y, v4.z,  normal.z, normal.y, normal.z,  uv4.x, uv4.y,  t2.x, t2.y, t2.z, b2.x, b2.y, b2.z,
		// positions       // Normal				       // uv coords   // tangent		// bitangent
	};

	glGenVertexArrays(1, &vao_plane);
	glBindVertexArray(vao_plane);

	glGenBuffers(1, &vbo_plane);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_plane);
	glBufferData(GL_ARRAY_BUFFER, sizeof(plane), &plane, GL_STATIC_DRAW);
	
	// Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 14, (void*)0);

	// Normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 14, (void*)(sizeof(float) * 3));

	// UV coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 14, (void*)(sizeof(float) * 6));

	// Tangent
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 14, (void*)(sizeof(float) * 8));

	// Bitangent
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 14, (void*)(sizeof(float) * 11));

	glBindVertexArray(0);

	planeModelMatrix.SetScale(1.f, 1.f, 1.f);

}

void loadWireFrame()
{
	cyPoint3f v1(-0.75f, 0.75f, 0.f);
	cyPoint3f v2(-0.75f, -0.75f, 0.f);
	cyPoint3f v3(0.75f, -0.75f, 0.f);
	cyPoint3f v4(0.75f, 0.75f, 0.f);

	// Tex coords
	cyPoint2f uv1(0.f, 1.f);
	cyPoint2f uv2(0.f, 0.f);
	cyPoint2f uv3(1.f, 0.f);
	cyPoint2f uv4(1.f, 1.f);

	float plane[] = {
		// positions      // uv coords 
		v1.x, v1.y, v1.z, uv1.x, uv1.y,
		v2.x, v2.y, v2.z, uv2.x, uv2.y,
		v3.x, v3.y, v3.z, uv3.x, uv3.y,

		v1.x, v1.y, v1.z, uv1.x, uv1.y,
		v3.x, v3.y, v3.z, uv3.x, uv3.y,
		v4.x, v4.y, v4.z, uv4.x, uv4.y,
		// positions      // uv coords 
	};

	glGenVertexArrays(1, &vao_wire);
	glBindVertexArray(vao_wire);

	glGenBuffers(1, &vbo_wire);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_wire);
	glBufferData(GL_ARRAY_BUFFER, sizeof(plane), &plane, GL_STATIC_DRAW);

	// Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0);

	// UVs
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(sizeof(float) * 5));

	glBindVertexArray(0);
}

void loadDisplacementMap()
{
	unsigned width, height;

	std::string file("Source/objects/displacement/teapot_disp.png");

	//decode
	unsigned error = lodepng::decode(pixels2, img_width2, img_height2, file);

	std::cout << "Image width and height: " << img_width2 << ", " << img_height2 << " loading from " << file << std::endl;
	glGenTextures(1, &normal_map);

	//if there's an error, display it
	if (error)
	{
		std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
		return;
	}

	glBindTexture(GL_TEXTURE_2D, displ_map);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_width2, img_height2, 0, GL_RGBA, GL_UNSIGNED_BYTE, &pixels2[0]);
	glGenerateMipmap(GL_TEXTURE_2D);

	// texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glGenerateMipmap(GL_TEXTURE_2D);

	std::cout << "Decoded successfully, generated mip maps, set wrap mode, and set up texture filtering." << std::endl;
}

int main(int argc, char **argv)
{
	//Initialize GLFW  
	if (!glfwInit())
	{
		return 1;
	}

	//Declare a window object  
	GLFWwindow* window;

	// Create a window and create its OpenGL context  
	window = glfwCreateWindow(width, height, "Project 8", NULL, NULL);

	if (!window)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		return 1;
	}

	glfwMakeContextCurrent(window);

	// Sets the key callback  
	glfwSetKeyCallback(window, key_callback);

	// Set the mouse button callback
	glfwSetMouseButtonCallback(window, mouse_callback);

	// Set the mouse move callback
	glfwSetCursorPosCallback(window, cursor_position_callback);

	//Initialize GLEW
	GLenum err = glewInit();

	//If GLEW hasn't initialized  
	if (err != GLEW_OK)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		return -1;
	}

	glEnable(GL_DEPTH_TEST);

	std::cout << glGetString(GL_VERSION) << std::endl;

	// == END SETUP ==

	GLint patchVerts = 0;
	glGetIntegerv(GL_MAX_PATCH_VERTICES, &patchVerts);
	glPatchParameteri(GL_PATCH_VERTICES, 3);
	std::cout << "Max number of patches per batch supported: " << patchVerts << std::endl;

	loadShaders();
	loadPlane();
	loadWireFrame();
	loadDisplacementMap();
	loadNormals();

	std::cout << std::endl;
	std::cout << "Press SPACE to toggle the wireframe mode" << std::endl;
	std::cout << "Use left to decrease tesellation and right to increase" << std::endl;

	//Main Loop
	while (!glfwWindowShouldClose(window))
	{
		//Set a background color
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

		//Clear color buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// RENDER PLANE
		displ_shader.Bind();

		displ_shader.SetUniform(0, camera.get_perspective_matrix());
		displ_shader.SetUniform(1, planeModelMatrix);
		displ_shader.SetUniform(2, camera.get_view_matrix());

		displ_shader.SetUniform(4, light1.get_position());
		displ_shader.SetUniform(5, camera.get_position());
		displ_shader.SetUniform(9, cyMatrix3f(planeModelMatrix));
		displ_shader.SetUniform(10, currTessel);

		displ_shader.SetUniform(3, cyPoint2f(width, height));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, normal_map);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, displ_map);

		// Rendering using patches
		glBindVertexArray(vao_plane);
		glDrawArrays(GL_PATCHES, 0, 6);
		glBindVertexArray(0);

		// WIREFRAME RENDERING
		if (activateWires)
		{
			wireframe.Bind();
			wireframe.SetUniform(0, camera.get_perspective_matrix());
			wireframe.SetUniform(1, planeModelMatrix);
			wireframe.SetUniform(2, camera.get_view_matrix());

			wireframe.SetUniform(4, currTessel);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, displ_map);

			glBindVertexArray(vao_wire);
			glDrawArrays(GL_PATCHES, 0, 6);
			glBindVertexArray(0);
		}

		// Window events and swapping buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//Close OpenGL window and terminate GLFW  
	glfwDestroyWindow(window);

	//Finalize and clean up GLFW  
	glfwTerminate();

	return 0;
}

// CALL BACKS
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		if (activateWires)
			activateWires = false;
		else
			activateWires = true;

	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
		if(currTessel > 1)
			std::cout << "Current Tessel Level: " << (--currTessel) << std::endl;
		else
			std::cout << "Current Tessel Level: " << (currTessel) << std::endl;

	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
		std::cout << "Current Tessel Level: " << (++currTessel) << std::endl;
}

static void mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS)
	{
		leftDown = true;
	}

	if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_RELEASE)
	{
		leftDown = false;
	}

	if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_PRESS)
	{
		rightDown = true;
	}

	if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_RELEASE)
	{
		rightDown = false;
	}
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{

	float deltaX = (float)xpos - lastX;
	float deltaY = lastY - (float)ypos;

	if (leftDown)
	{
		camera.rotate(deltaX, deltaY);
		lastX = (float)xpos;
		lastY = (float)ypos;
	}

	if (rightDown)
	{
		camera.translate(deltaX, deltaY);
		lastX = (float)xpos;
		lastY = (float)ypos;
	}
	
}
