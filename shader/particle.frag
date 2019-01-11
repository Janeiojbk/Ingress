#version 330 core

// Refer to https://learnopengl-cn.github.io/06%20In%20Practice/2D-Game/06%20Particles/

in vec2 TexCoords;
in vec4 ParticleColor;
out vec4 color;

uniform sampler2D sprite;

void main()
{
	vec4 texColor = texture(sprite, TexCoords);
	if(texColor.a < 0.1)
		discard;
	color = texColor * ParticleColor;
}