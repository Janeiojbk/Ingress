#include "particle.h"

ParticleGenerator::ParticleGenerator(glm::vec3 startPosition, glm::vec3 endPosition, GLuint amount)
	: amount(amount), startPosition(startPosition), endPosition(endPosition)
{
	this->init();
}

void ParticleGenerator::Update(GLfloat dt, GLuint newParticles)
{
	glm::vec3 offset = (startPosition - endPosition) / 2.0f;
	// Add new particles 
	for (GLuint i = 0; i < newParticles; ++i)
	{
		int unusedParticle = this->firstUnusedParticle();
		//std::cout << unusedParticle << std::endl;
		this->respawnParticle(this->particles[unusedParticle], offset);
	}
	// Update all particles
	for (GLuint i = 0; i < this->amount; ++i)
	{
		Particle &p = this->particles[i];
		p.Life -= dt; // reduce life
		if (p.Life > 0.0f)
		{	// particle is alive, thus update
			p.Position -= p.Velocity * 0.1f;
			//p.Color.a -= dt * 2.5;
			//std::cout << p.Position.x << " " << p.Position.y << " " << p.Position.z << std::endl;
		}
	}
}

// Render all particles
void ParticleGenerator::Draw(Shader shader, Texture2D texture)
{
	// Use additive blending to give it a 'glow' effect
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	shader.use();
	for (Particle particle : this->particles)
	{
		if (particle.Life > 0.0f)
		{
			shader.setVec3("offset", particle.Position);
			shader.setVec4("color", particle.Color);
			texture.Bind();
			glBindVertexArray(this->VAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
		}
	}
	// Don't forget to reset to default blending mode
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleGenerator::init()
{
	// Set up mesh and attribute properties
	GLuint VBO;
	GLfloat particle_quad[] = {
		0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f, 0.0f
	};
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(this->VAO);
	// Fill mesh buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), &particle_quad, GL_STATIC_DRAW);
	// Set mesh attributes
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glBindVertexArray(0);

	// Create this->amount default particle instances
	for (GLuint i = 0; i < this->amount; ++i)
		this->particles.push_back(Particle((startPosition + endPosition) / 2.0f));
}

// Stores the index of the last particle used (for quick access to next dead particle)
GLuint lastUsedParticle = 0;
GLuint ParticleGenerator::firstUnusedParticle()
{
	// First search from last used particle, this will usually return almost instantly
	for (GLuint i = lastUsedParticle; i < this->amount; ++i) {
		if (this->particles[i].Life <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}
	// Otherwise, do a linear search
	for (GLuint i = 0; i < lastUsedParticle; ++i) {
		if (this->particles[i].Life <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}
	// All particles are taken, override the first one (note that if it repeatedly hits this case, more particles should be reserved)
	lastUsedParticle = 0;
	return 0;
}

void ParticleGenerator::respawnParticle(Particle &particle, glm::vec3 offset)
{
	GLfloat particleLength = glm::sqrt(glm::pow(endPosition.x - startPosition.x, 2.0f)
		+ glm::pow(endPosition.y - startPosition.y, 2.0f)
		+ glm::pow(endPosition.z - startPosition.z, 2.0f));
	//GLfloat randomx = (rand() % 100 - 50) / 100.0f;
	GLfloat randomy = (rand() % 100 - 50) / (50.0f);
	GLfloat rColor = 0.5 + ((rand() % 100) / 100.0f);
	//std::cout << particle.Position.x <<" "<< particle.Position.y <<" "<< particle.Position.z << std::endl;
	//std::cout << randomx << randomy << std::endl;
	particle.Velocity = glm::vec3(offset.x * randomy,
		offset.y * randomy,
		offset.z * randomy);
	particle.Color = glm::vec4(rColor, rColor, rColor, 1.0f);
	particle.Life = 1.0f;
	particle.Position = (startPosition + endPosition) / 2.0f;
}
