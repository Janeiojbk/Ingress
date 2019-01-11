#ifndef PARTICLE_H
#define PARTICLE_H
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>

#include "shader_m.h"
#include "texture.h"

// Refer to https://learnopengl-cn.github.io/06%20In%20Practice/2D-Game/06%20Particles/

struct Particle {
	glm::vec3 Position, Velocity;
	glm::vec4 Color;
	GLfloat Life;

	Particle(glm::vec3 Position) :Position(Position), Velocity(0.0f), Color(1.0f), Life(0.0f) {

	}
};

class ParticleGenerator
{
public:
	ParticleGenerator(glm::vec3 startPosition, glm::vec3 endPosition, GLuint amount);
	ParticleGenerator()
	{

	}
	void Update(GLfloat dt, GLuint newParticles);
	void Draw(Shader shader, Texture2D texture);
private:
	std::vector<Particle> particles;
	GLuint amount;
	GLuint VAO;
	glm::vec3 startPosition;
	glm::vec3 endPosition;
	void init();
	GLuint firstUnusedParticle();
	void respawnParticle(Particle &particle, glm::vec3 offset = glm::vec3(0.0f));

};
#endif // !PARTICLE_H
