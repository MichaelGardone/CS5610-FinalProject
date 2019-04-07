#ifndef _OBJECT_H_
#define _OBJECT_H_

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "cyMatrix.h"
#include "cyPoint.h"
#include "cyGL.h"

#include "System.h"
#include "Camera.h"
#include "Mesh.h"

class Mesh;

class Object
{
public:
	Object();
	
	// Logic functions for the object
	virtual void Reset();
	virtual void Draw(Camera& camera);
	virtual void Update();

	// .vert = verex, .frag = fragment, .tcs = tessellation control, .tes = tessellation evaluation, .geom = geometry, .cmpte = compute
	virtual void LoadShaders(char**, int);
	// Register uniforms given by a list
	virtual void RegisterUniforms(char **, int);

	// Return a pointer to the Matrix4f to speed up return
	virtual cyMatrix4f* model_matrix() const;

private:
	// Position information
	cyPoint3f position;
	cyPoint3f scale;
	cyPoint3f rotation;

	cyPoint3f color;

	// Matrix/world information
	cyMatrix4f model_matrix;

	// Shader information
	cyGLSLProgram shader;
	cyGLSLShader vert, frag, tcs, tes, geom, cmpte;
	
	// Object's name for debugging purposes
	char* name;

	std::shared_ptr<Mesh> mesh;

};

#endif