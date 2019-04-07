#version 410 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

uniform mat4 MVP;
uniform model;

out vec3 norm;
out vec3 fragPos;

void main()
{
	gl_Position = MVP * vec4(position, 1.0);

	norm = normal;
	fragPos = vec3(model * vec4(position, 1.0));
}