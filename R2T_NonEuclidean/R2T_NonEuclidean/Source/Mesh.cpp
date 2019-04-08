#include "Mesh.h"

/*
public:
	Mesh(const char*);
	~Mesh();

	void Draw();

private:
	GLuint vao, vbo;

	std::vector<float> verts;
	std::vector<float> normals;
*/

Mesh::Mesh(const char* file)
{
	// Load mesh into cyTriMesh backend
	this->mesh.LoadFromFileObj(file);
	mesh.ComputeNormals();

	this->vertices.resize(mesh.NF() * 3);

	for (unsigned int i = 0; i < mesh.NF(); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			this->vertices[3 * i + j].verts = mesh.V(mesh.F(i).v[j]);
			this->vertices[3 * i + j].normals = mesh.VN(mesh.F(i).v[j]);
		}
	}

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(0, sizeof(vertex_info) * this->vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// 0 = POSITIONS
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_info), (void*)0);

	// 1 = NORMALS
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_info), (void*)offsetof(vertex_info, normals));
}

Mesh::~Mesh()
{
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

void Mesh::Draw()
{
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 1);
}
