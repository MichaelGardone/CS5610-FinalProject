#version 330 core

out vec4 color;
in vec2 uvCoords;

uniform sampler2D tex;

void main()
{
	color = texture(tex, uvCoords);
	//color = vec4(0.3, 0.4, 0.7, 1.0);
}