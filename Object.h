#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader_m.h"

class Object
{
public:
	Object();
	~Object();
	void draw(Shader ourShader);
	glm::vec3 position, color;
	unsigned int VBO, VAO;
	float vertices[180];
};

