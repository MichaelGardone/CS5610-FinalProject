#ifndef OBJECT_H_
#define OBJECT_H_

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "cyMatrix.h"
#include "cyPoint.h"
#include "cyGL.h"

class Object
{
public:
	Object();

	// Memory management
	virtual ~Object();

	// Logic functions for the object
	virtual void Reset();
	virtual void Draw();
	virtual void Update();

	// .vert = verex, .frag = fragment, .tcs = tessellation control, .tes = tessellation evaluation, .geom = geometry, .cmpte = compute
	virtual void LoadShaders(char**, int);

	// Return a pointer to the Matrix4f to speed up return
	virtual cyMatrix4f* model_matrix() const;

private:
	// Position information
	cyPoint3f position;
	cyPoint3f scale;
	cyPoint3f rotation;

	// Matrix/world information
	cyMatrix4f model_matrix;

	// Shader information
	cyGLSLProgram program;
	cyGLSLShader vert, frag, tcs, tes, geom, cmpte;

	// Object's name for debugging purposes
	char* name;

};

#endif