#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

struct Light{
	vec3 direction;

	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
};

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform float shininess;
uniform Light light;
uniform vec3 viewPos;


void main()
{    
    // ambient
    vec4 ambient = light.ambient * texture(texture_diffuse1, TexCoords).rgba;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(norm, lightDir), 0.0);
    vec4 diffuse = light.diffuse * diff * texture(texture_diffuse1, TexCoords).rgba;  
    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
	
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec4 specular = light.specular * spec * texture(texture_specular1, TexCoords).rgba;  
        
    vec4 result = ambient + diffuse + specular;
	
	if(result.a < 0.1f)
		discard;

    FragColor = result;
}
