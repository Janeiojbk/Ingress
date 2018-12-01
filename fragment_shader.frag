#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;
in vec4 FragPosLightSpace;

struct Light{
	vec3 direction;

	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
};

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D shadowMap;
uniform float shininess;
uniform Light light;
uniform vec3 viewPos;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // 执行透视除法
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // 变换到[0,1]的范围
    projCoords = projCoords * 0.5 + 0.5;
    // 取得最近点的深度(使用[0,1]范围下的fragPosLight当坐标)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // 取得当前片元在光源视角下的深度
    float currentDepth = projCoords.z;
    // 检查当前片元是否在阴影中
    // Calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(Normal);
    float bias = 0.005;
    // Check whether current frag pos is in shadow
     //float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
	}    
    }
    shadow /= 9.0;
    // Keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

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
        
	    // 计算阴影
    float shadow = ShadowCalculation(FragPosLightSpace);       
    vec4 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * texture(texture_diffuse1, TexCoords).rgba;    
    vec4 result = ambient + diffuse + specular;
	
	if(result.a < 0.1f)
		discard;

    FragColor = lighting;
}
