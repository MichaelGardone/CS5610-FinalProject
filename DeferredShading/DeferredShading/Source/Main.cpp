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
#include "System.h"
#include "GBuffer.hpp"

typedef struct VertexInfo
{
	cyPoint3f pos;
	cyPoint3f normal;
	cyPoint2f uvCoords;
};

// Callbacks  
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void mouse_callback(GLFWwindow* window, int button, int action, int mods);
static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

// Teapot
cyTriMesh teapot;
std::vector<VertexInfo> vertex_info;
GLuint vao_teapot, vbo_teapot, teapot_texture;
unsigned img_width = 512, img_height = 512;
std::vector<unsigned char> pixels;
cyMatrix4f teapotModel(1.f);

// Plane
GLuint vao_plane, vbo_plane;
cyMatrix4f planeModelMatrix(0.f);
std::vector<VertexInfo> plane_verts(6);

// Cube
GLuint vao_cube, vbo_cube;
cyMatrix4f cubeModel;

// GBuffer
GBuffer gbuffer;

// For model info in a texture
cyGLSLProgram gbuffer_shader;
cyGLSLShader gbuf_vert, gbuf_frag;

// For the Blinn-Phong lighting
cyGLSLProgram defferdlight_shader;
cyGLSLShader dl_vert, dl_frag;

// Right
float lastX = 0.f, lastY = 0.f;
bool leftDown = false, rightDown = false;

Camera camera(cyPoint3f(0.f, 0.f, 3.f), cyPoint3f(0.f, 1.f, 0.f), cyPoint3f(0.f, 0.f, -1.f));
Light light(cyPoint3f(-2.f, 4.f, -1.f), cyPoint3f(), cyPoint3f());

void loadTeapot(char *filename)
{
	// Load from the command line
	bool load_result = teapot.LoadFromFileObj(filename, true, &std::cout);
	
	teapot.ComputeNormals();

	vertex_info.resize(teapot.NF() * 3);

	for (int i = 0; i < teapot.NF(); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			vertex_info[3 * i + j].pos = teapot.V(teapot.F(i).v[j]); // Position
			vertex_info[3 * i + j].normal = teapot.VN(teapot.F(i).v[j]); // Vertex Normal
			vertex_info[3 * i + j].uvCoords = cyPoint2f(teapot.VT(teapot.FT(i).v[j]).x, teapot.VT(teapot.FT(i).v[j]).y); // Vertex textures
		}
	}

	glGenVertexArrays(1, &vao_teapot);
	glBindVertexArray(vao_teapot);

	glGenBuffers(1, &vbo_teapot);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_teapot);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexInfo) * vertex_info.size(), &vertex_info[0], GL_STATIC_DRAW);

	// Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexInfo), (void*)0);

	// Normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexInfo), (void*)offsetof(VertexInfo, normal));

	// Texture UV coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexInfo), (void*)offsetof(VertexInfo, uvCoords));

	glBindVertexArray(0);

	teapotModel = cy::Matrix4f::MatrixTrans(cyPoint3f(0.f, -0.25f, 1.75f)) * cy::Matrix4f::MatrixScale(0.05f) * cy::Matrix4f::MatrixRotationX(TO_RADIANS(-90.f));
	//teapotModel = cy::Matrix4f::MatrixRotationX(TO_RADIANS(-90.f));
	//teapotModel = cy::Matrix4f::MatrixScale(0.05f);
}

void loadCube()
{
	float vertices[] = {
		// back face
		-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
		 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
		 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
		 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
		-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
		-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
		// front face
		-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
		 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
		 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
		 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
		-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
		-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
		// left face
		-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
		-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
		-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
		-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
		-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
		-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
		// right face
		 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
		 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
		 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
		 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
		 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
		 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
		// bottom face
		-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
		 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
		 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
		 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
		-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
		-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
		// top face
		-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
		 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
		 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
		 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
		-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
		-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
	};
	glGenVertexArrays(1, &vao_cube);
	glGenBuffers(1, &vao_cube);
	// fill buffer
	glBindBuffer(GL_ARRAY_BUFFER, vbo_cube);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// link vertex attributes
	glBindVertexArray(vao_cube);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	cubeModel = cy::Matrix4f::MatrixTrans(cyPoint3f(0.f, 0.f, 1.f)) * cy::Matrix4f::MatrixScale(0.5f);
}

void loadTexture()
{
	// TEXTURE
	// We're gonna go ahead and load our textures now
	std::string file("Source/objects/brick.png");

	//decode
	unsigned error = lodepng::decode(pixels, img_width, img_height, file);

	std::cout << "Image width and height: " << img_width << ", " << img_height << " loading from " << file << std::endl;
	glGenTextures(1, &teapot_texture);
	glBindTexture(GL_TEXTURE_2D, teapot_texture);

	//if there's an error, display it
	if (error)
	{
		std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
		return;
	}

	// no issue, let's get our texture data!
	glBindTexture(GL_TEXTURE_2D, teapot_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_width, img_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &pixels[0]);
	glGenerateMipmap(GL_TEXTURE_2D);
	// texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	std::cout << "Decoded successfully, generated mip maps, set wrap mode, and set up texture filtering." << std::endl;
}

void loadShaders()
{
	// GBuffer shader
	std::cout << "Starting buffer shader" << std::endl;
	
	gbuffer_shader.CreateProgram();

	gbuf_vert.CompileFile("Source/Shaders/DeferredShading/Buffer/buffer.vert", GL_VERTEX_SHADER);
	gbuf_frag.CompileFile("Source/Shaders/DeferredShading/Buffer/buffer.frag", GL_FRAGMENT_SHADER);

	gbuffer_shader.AttachShader(gbuf_vert);
	gbuffer_shader.AttachShader(gbuf_frag);

	gbuffer_shader.Link();

	// Register here
	gbuffer_shader.RegisterUniform(0, "PVM");
	gbuffer_shader.RegisterUniform(1, "model");
	std::cout << "Finished with buffer shader" << std::endl;

	// Deferred Lighting Shader
	std::cout << "Starting deferred lighting shader" << std::endl;

	defferdlight_shader.CreateProgram();

	dl_vert.CompileFile("Source/Shaders/DeferredShading/BlinnPhong/deferred_shader.vert", GL_VERTEX_SHADER);
	dl_frag.CompileFile("Source/Shaders/DeferredShading/BlinnPhong/deferred_shader.frag", GL_FRAGMENT_SHADER);

	defferdlight_shader.AttachShader(dl_vert);
	defferdlight_shader.AttachShader(dl_frag);

	defferdlight_shader.Link();

	// Register here
	defferdlight_shader.RegisterUniform(0, "viewPos");
	defferdlight_shader.RegisterUniform(1, "lights");
	std::cout << "Finished with deferred lighting shader" << std::endl;
}

void loadPlane()
{
	float plane[] = {
		// positions            // normals         // texcoords
		25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
	   -25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
	   -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,

		25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
	   -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
		25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
		// positions		   // Normals		  // uv coords
	};

	glGenVertexArrays(1, &vao_plane);
	glBindVertexArray(vao_plane);

	glGenBuffers(1, &vbo_plane);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_plane);
	glBufferData(GL_ARRAY_BUFFER, sizeof(plane), plane, GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(3 * sizeof(float)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(6 * sizeof(float)));

	glBindVertexArray(0);

	planeModelMatrix.SetIdentity();
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
	window = glfwCreateWindow(width, height, "Project 7", NULL, NULL);

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

	loadShaders();
	loadTeapot(argv[1]);
	loadCube();
	loadPlane();
	loadTexture();

	//Main Loop
	while (!glfwWindowShouldClose(window))
	{
		//Set a background color
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		//Clear color buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 1 - Geometry pass
		gbuffer.Bind();

		gbuffer_shader.Bind();

		// TEAPOT
		cyMatrix4f PVM = camera.get_perspective_matrix() * camera.get_view_matrix() * planeModelMatrix;

		// Diffuse map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, teapot_texture);

		// PLANE
		glBindVertexArray(vao_plane);
		glDrawArrays(GL_TRIANGLES, 0, plane_verts.size());
		glBindVertexArray(0);

		PVM = camera.get_perspective_matrix() * camera.get_view_matrix() * teapotModel;

		glBindVertexArray(vao_teapot);
		glDrawArrays(GL_TRIANGLES, 0, vertex_info.size());
		glBindVertexArray(0);

		gbuffer.Unbind();
		
		// 2 - Lighting pass

		// 3 - Copy to default
		gbuffer.CopyToDefault();

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
