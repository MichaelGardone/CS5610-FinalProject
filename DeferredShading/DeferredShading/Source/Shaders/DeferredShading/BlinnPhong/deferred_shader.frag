#version 330 core

// Blinn-Phong lighting deferred shading

out vec4 color;

in vec2 uvCoords;

// GBUFFER TEXTURES
uniform sampler2D positions;
uniform sampler2D normals;
uniform sampler2D albedo;

struct Light {
	vec3 position;
	vec3 color;
	float linear;
	float quad;
};

const int NUM_LIGHTS = 1;
uniform Light lights[NUM_LIGHTS];
uniform vec3 viewPos;

void main()
{
	vec3 fragPos = texture(positions, uvCoords).rgb;
	vec3 normal = texture(normals, uvCoords).rgb;
	vec3 diffuse = texture(albedo, uvCoords).rgb;
	float specular = texture(albedo, uvCoords).a;

	vec3 lighting = diffuse * 0.3;
	vec3 viewDir = normalize(viewPos - fragPos);
	for(int i = 0; i < NUM_LIGHTS; i++)
	{
		// diffuse
		vec3 lightDir = normalize(lights[i].position - fragPos);
		vec3 diff = max(dot(normal, lightDir), 0.0) * diffuse * lights[i].color;
		// specular
		vec3 halfway = normalize(lightDir + viewDir);
		float fspec = pow(max(dot(normal, halfway), 0.0), 16.0);
		vec3 spec = lights[i].color * fspec * specular;
		// attenuation
		float distance = length(lights[i].position - fragPos);
		float attenuation = 1.0 / (1.0 + lights[i].Linear * distance + lights[i].Quadratic * distance * distance);
		diff *= attenuation;
		spec *= attenuation;
		lighting += diff + spec;
	}

	color = vec4(lighting, 1.0);
}