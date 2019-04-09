#ifndef _MODEL_H_
#define _MODEL_H_

#include <Windows.h>
#include <GL/glew.h>
#include <vector>

#include "cyCore.h"
#include "cyTriMesh.h"
#include "cyMatrix.h"
#include "cyPoint.h"
#include "cyGL.h"

#include "lodepng.h"

typedef struct vertex_info {
	cyPoint3f position;
	cyPoint3f normal;
	cyPoint2f texCoords;
};

class Model
{
public:
	
	Model(const char *name, const char *file)
	{
		this->name = new char[strlen(name)];
		this->name = name;

		LoadModel(file);
	}
	
	~Model()
	{
		delete[] name;
	}
	
	void LoadModel(const char* file)
	{
		if (!mesh.LoadFromFileObj(file))
		{
			std::cout << "Unable to load teapot!" << std::endl;
			return;
		}

		mesh.ComputeNormals();
		info.resize(mesh.NF());

		for (int i = 0; i < mesh.NF(); i++)
		{
			for (int j = 0; j < 3; j++)
			{
				info[3 * i + j].position = mesh.V(mesh.F(i).v[j]);
				info[3 * i + j].normal = mesh.VN(mesh.F(i).v[j]);
				info[3 * i + j].texCoords = cyPoint2f(mesh.VT(mesh.FT(i).v[j]).x, mesh.VT(mesh.FT(i).v[j]).y);
			}
		}

		// Setup memory
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_info) * info.size(), &info[0], GL_STATIC_DRAW);

		// Positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_info), (void*)0);

		// Normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_info), (void*)(offsetof(vertex_info, normal)));

		// Textures
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_info), (void*)(offsetof(vertex_info, texCoords)));

		// Load texture
		unsigned width, height;
		unsigned error = lodepng::decode(pixels, width, height, "Source/objects/brick.png");

		// TEXTURE0 = DIFFUSE
		glActiveTexture(GL_TEXTURE0);

		if (error)
		{
			std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
			return;
		}
		else {
			// no issue, let's get our texture data!
			glBindTexture(GL_TEXTURE_2D, texture);

			unsigned char *data = &pixels[0];

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			// texture wrapping
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			// texture filtering
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		}

		unsigned width2, height2;
		error = lodepng::decode(pixels, width2, height2, "Source/objects/brick-specular.png");

		// TEXTURE1 = SPECULAR
		glActiveTexture(GL_TEXTURE1);
		if (error)
		{
			std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
			return;
		}
		else {
			// no issue, let's get our texture data!
			glBindTexture(GL_TEXTURE_2D, texture);

			unsigned char *data = &pixels[0];

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width2, height2, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			// texture wrapping
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			// texture filtering
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		}
	}

	void Draw()
	{
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, info.size());
		glBindVertexArray(0);
	}

private:
	cyTriMesh mesh;

	std::vector<vertex_info> info;
	std::vector<unsigned char> pixels, pixels2;

	GLuint texture;
	GLuint vao, vbo;

	const char *name;

};

#endif