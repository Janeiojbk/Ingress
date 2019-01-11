#version 330 core

// Refer to https://learnopengl-cn.github.io/04%20Advanced%20OpenGL/06%20Cubemaps/

out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{    
    FragColor = texture(skybox, TexCoords);
}