#version 330 core

layout(location = 1) in vec3 pos;
layout(location = 2) in vec2 texCoords;

out vec2 uvCoords;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

void main()
{
	uvCoords = texCoords;
	gl_Position = proj * view * model * vec4(pos, 1.0);
}
