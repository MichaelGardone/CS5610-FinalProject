#version 330 core
out vec4 fragColor;

in vec2 uvCoords;

uniform sampler2D gPos;
uniform sampler2D gNorm;
uniform sampler2D gAlbedo;

struct Light {
	vec3 position;
	vec3 color;

	float linear;
	float quadratic;
};

const int NR_LIGHTS = 32;
uniform Light lights[NR_LIGHTS];
uniform vec3 viewPos;

void main()
{
	vec3 fragPos = texture(gPos, uvCoords).rgb;
	vec3 normal = texture(gNorm, uvCoords).rgb;
	vec3 diffuse = texture(gAlbedo, uvCoords).rgb;
	float specular_comp = texture(gAlbedo, uvCoords).a;

	vec3 lighting = diffuse * 0.1; // Todo: Not make the ambient component hard-coded
	vec3 viewDir = normalize(viewPos - fragPos);
	for(int i = 0; i < NR_LIGHTS; i++)
	{
		// diffuse component
		vec3 lightDir = normalize(lights[i].position - fragPos);
		vec3 diffuse = max(dot(normal, lightDir), 0.0) * diffuse * lights[i].color;

		// specular component
		vec3 halfDir = normalize(lightDir + viewDir);
		float spec = pow(max(dot(normal, halfDir), 0.0), 16.0);
		vec3 specular = lights[i].color * spec * specular_comp;

		// attenutation component
		float distance = length(lights[i].position - fragPos);
		float atten = 1.0/ (1.0 + lights[i].linear * distance + lights[i].quadratic * distance * distance);
		diffuse *= atten;
		specular *= atten;
		lighting += diffuse + specular;
	}

	fragColor = vec4(lighting, 1.0);
}