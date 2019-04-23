#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 texCoords;

struct Light {
    vec3 Position;
    vec3 Color;
    
    float Linear;
    float Quadratic;
    float Radius;
};

uniform vec3 viewPos;

const int NR_LIGHTS = 32;
uniform Light lights[NR_LIGHTS];

uniform sampler2D texture_diffuse1;

void main()
{
    vec3 Diffuse = texture(texture_diffuse1, texCoords).rgb;
    float Specular = 0.5;

    vec3 lighting = Diffuse * 0.1; // hard-coded ambient component
    vec3 viewDir = normalize(viewPos - FragPos);
	for(int i = 0; i < NR_LIGHTS; ++i)
    {
        // calculate distance between light source and current fragment
        float distance = length(lights[i].Position - FragPos);
        if(distance < lights[i].Radius)
        {
            // diffuse
            vec3 lightDir = normalize(lights[i].Position - FragPos);
            vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * lights[i].Color;

            // specular
            vec3 halfwayDir = normalize(lightDir + viewDir);  
            float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
            vec3 specular = lights[i].Color * spec * Specular;

            // attenuation
            float attenuation = 1.0 / (1.0 + lights[i].Linear * distance + lights[i].Quadratic * distance * distance);
            diffuse *= attenuation;
            specular *= attenuation;
            lighting += diffuse + specular;
        }
    }

    FragColor = vec4(lighting, 1.0);
}
