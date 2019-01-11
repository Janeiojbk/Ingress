#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

void main()
{
	gl_Position = vec4(aPos-vec3(1.0f, 0.0f, 0.0f), 1.0);
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}