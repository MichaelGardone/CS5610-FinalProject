#version 330 core
layout(location = 0) out vec3 g_position;
layout(location = 1) out vec3 g_normal;
layout(location = 2) out vec4 g_albedo;

out vec3 fragPos;
out vec2 uvCoords;
out vec3 normal;

uniform sampler2D tex_diffuse;
uniform sampler2D tex_specular;

void main()
{
	g_position = fragPos;
	g_normal = normalize(normal);

	g_albedo.rgb = texture(tex_diffuse, uvCoords).rgb;
	g_albedo.a = texture(tex_specular, uvCoords).a;
}