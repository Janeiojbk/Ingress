#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader_m.h"
#include "Model.h"

class Object:public Model
{
public:
	Object(glm::vec3 Color = glm::vec3(0.8f, 0.7f, 0.4f), glm::vec3 Shape = glm::vec3(1.0f, 1.0f, 1.0f));
	Object(string const path, glm::vec3 scale);
	~Object();
	void draw(Shader ourShader);
	glm::vec3 position, color, scale;
	glm::vec3 shape;
private:
	unsigned int VBO, VAO;
	float vertices[180];
};

