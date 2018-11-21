#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "Resonator.h"
#include "shader_m.h"

class Portal
{
public:
	Portal(glm::vec3 Position);
	~Portal();
	void draw(Shader ourShader);

	glm::vec3 position, color;
	float lastHacked;
	float radius;
	std::vector<Resonator> resonator;
private:
	unsigned int VBO, VAO;
	float vertices[180];
};

