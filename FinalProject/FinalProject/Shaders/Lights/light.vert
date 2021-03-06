#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;

uniform PVM;

void main()
{
	gl_Position = PVM * vec4(pos, 1.0);
}