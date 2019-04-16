#version 330 core
layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texCoords;

out vec2 uvCoords;

void main()
{
	uvCoords = texCoords;
	gl_Position = vec4(pos, 1.0);
}