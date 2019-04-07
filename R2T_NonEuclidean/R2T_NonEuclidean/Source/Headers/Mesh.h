#ifndef _MESH_H_
#define _MESH_H_

#include <GLFW/glfw3.h>
#include <GL/glew.h>
#include <vector>

#include "System.h"
#include "cyTriMesh.h"

typedef struct {
	cyPoint3f verts;
	cyPoint3f normals;
} vertex_info;

class Mesh {

public:
	Mesh(const char*);
	~Mesh();

	void Draw();
	
private:
	GLuint vao, vbo;
	cyTriMesh mesh;

	std::vector<vertex_info> vertices;

};

#endif