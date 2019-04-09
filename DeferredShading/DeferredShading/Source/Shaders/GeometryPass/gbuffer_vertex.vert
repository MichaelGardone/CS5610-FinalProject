#version 330 core

layout (location = 1) in vec3 pos;
layout (location = 2) in vec3 norm;
layout (location = 3) in vec2 texCoords;

out vec3 fragPos;
out vec2 uvCoords;
out vec3 normal;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

void main()
{
	vec4 worldPos = model * vec4(pos, 1.0);
	fragPos = worldPos.xyz;
	uvCoords = texCoords;

	mat3 normalMat = transpose(inverse(mat3(model)));
	normal = normalMat * norm;

	gl_Position = proj * view * worldPos;
}