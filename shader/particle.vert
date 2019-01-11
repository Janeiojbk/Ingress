#version 330 core

// Refer to https://learnopengl-cn.github.io/06%20In%20Practice/2D-Game/06%20Particles/

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec2 TexCoord;

out vec2 TexCoords;
out vec4 ParticleColor;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec3 offset;
uniform vec4 color;

void main()
{
    float scale = 0.3f;
    TexCoords = TexCoord;
    ParticleColor = color;
	gl_Position = projection * view * model * vec4((vertex.xyz * scale) + offset, 1.0);
}