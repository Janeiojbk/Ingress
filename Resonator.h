#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "Resonator.h"
#include "shader_m.h"

class Resonator
{
public:
	Resonator(float distance);
	~Resonator();
	void draw(Shader ourShader, glm::vec3 pos);

	long health;
	float distance;
private:
	unsigned int VBO, VAO;
	float vertices[180];
};

