#version 330 core

// Refer to https://learnopengl-cn.github.io/04%20Advanced%20OpenGL/06%20Cubemaps/

layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}