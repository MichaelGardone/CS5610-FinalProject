#pragma once

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glm/glm.hpp>

class Shader {
public:
	unsigned int id;

	Shader(const char* vertPath, const char* fragPath, const char* geomPath = nullptr)
	{
		std::string vert_code, frag_code, geom_code;
		std::ifstream vertSF, fragSF, geomSF;
		vertSF.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fragSF.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		geomSF.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try
		{
			vertSF.open(vertPath);
			fragSF.open(fragPath);
			std::stringstream vShader, fShader;
			vShader << vertSF.rdbuf();
			fShader << fragSF.rdbuf();

			vertSF.close();
			fragSF.close();

			vert_code = vShader.str();
			frag_code = fShader.str();

			if (geomPath != nullptr)
			{
				geomSF.open(geomPath);
				std::stringstream gShader;
				gShader << geomSF.rdbuf();
				geomSF.close();
				geom_code = gShader.str();
			}

		}
		catch (std::ifstream::failure e)
		{
			std::cout << "ERROR: Shader file not read successfully!" << std::endl;
		}

		const char* vShaderCode = vert_code.c_str();
		const char* fShaderCode = frag_code.c_str();
		unsigned int vertex, frag;

		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);
		checkCompileErrors(vertex, "VERTEX");

		frag = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(frag, 1, &fShaderCode, NULL);
		glCompileShader(frag);
		checkCompileErrors(frag, "FRAGMENT");

		unsigned int geom;
		if (geomPath != nullptr)
		{
			const char* geomShaderCode = geom_code.c_str();
			geom = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(geom, 1, &geomShaderCode, NULL);
			glCompileShader(geom);
			checkCompileErrors(geom, "GEOMETRY");
		}

		// Create the shader
		id = glCreateProgram();
		glAttachShader(id, vertex);
		glAttachShader(id, frag);
		if (geomPath != nullptr)
			glAttachShader(id, geom);

		// Link and error check
		glLinkProgram(id);
		checkCompileErrors(id, "PROGRAM");

		// Cleanup
		glDeleteShader(vertex);
		glDeleteShader(frag);
		if (geomPath != nullptr)
			glDeleteShader(geom);
	}

	void use()
	{
		glUseProgram(id);
	}

	void setBool(const std::string &name, bool value) const
	{
		glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
	}

	void setInt(const std::string &name, int value) const
	{
		glUniform1i(glGetUniformLocation(id, name.c_str()), value);
	}

	void setFloat(const std::string &name, float value) const
	{
		glUniform1f(glGetUniformLocation(id, name.c_str()), value);
	}

	void setVec2(const std::string &name, const glm::vec2 &value) const
	{
		glUniform2fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
	}

	void setVec2(const std::string &name, float x, float y) const
	{
		glUniform2f(glGetUniformLocation(id, name.c_str()), x, y);
	}

	void setVec3(const std::string &name, const glm::vec3 &value) const
	{
		glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
	}

	void setVec3(const std::string &name, float x, float y, float z) const
	{
		glUniform3f(glGetUniformLocation(id, name.c_str()), x, y, z);
	}

	void setVec4(const std::string &name, const glm::vec4 &value) const
	{
		glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
	}

	void setVec4(const std::string &name, float x, float y, float z, float w)
	{
		glUniform4f(glGetUniformLocation(id, name.c_str()), x, y, z, w);
	}

	void setMat2(const std::string &name, const glm::mat2 &mat) const
	{
		glUniformMatrix2fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

	void setMat3(const std::string &name, const glm::mat3 &mat) const
	{
		glUniformMatrix3fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

	void setMat4(const std::string &name, const glm::mat4 &mat) const
	{
		glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

private:
	void checkCompileErrors(GLuint shader, std::string type)
	{
		GLint success;
		GLchar infoLog[2048];
		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 2048, NULL, infoLog);
				std::cout << "### ERROR: Shader Compilation Error ###\n" <<
					"Type: " << type << "\n" <<
					infoLog << "\n" <<
					"################" << std::endl;
			}
		}
		else {
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shader, 2048, NULL, infoLog);
				std::cout << "### ERROR: Program Linker Error ###\n" <<
					"Type: " << type << "\n" <<
					infoLog << "\n" <<
					"################" << std::endl;
			}
		}
	}

};
