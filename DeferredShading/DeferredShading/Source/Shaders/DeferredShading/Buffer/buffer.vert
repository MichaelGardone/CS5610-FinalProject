#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 texCoords;

out vec3 fragPos;
out vec2 uvCoords;
out vec3 normal;

uniform mat4 PVM;
uniform mat4 model;

void main()
{
	vec4 worldPos = model * vec4(position, 1.0);
	fragPos = worldPos.xyz;
	uvCoords = texCoords;

	mat3 normMat = transpose(inverse(mat3(model)));
	normal = normMat * norm;

	gl_Position = PVM * worldPos;
}