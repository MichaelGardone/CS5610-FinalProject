#version 330 core

layout(location = 1) out vec3 gPos;
layout(location = 2) out vec3 gNorm;
layout(location = 3) out vec4 gAlbedo;

in vec3 fragPos;
in vec2 uvCoords;
in vec3 normal;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;

void main ()
{
	gPos = fragPos;
	gNorm = normalize(normal);
	gAlbedo.rgb = texture(texture_diffuse, uvCoords).rgb;
	gAlbedo.a = texture(texture_specular, uvCoords).a;
}