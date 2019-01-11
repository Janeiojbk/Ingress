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
	Object(string const path, glm::vec3 scale, float theta);
	// Draw的驱动函数，如果是多边形就自己画。如果是导入模型使用model的Draw方法
	void draw(Shader ourShader);
	// 两套体系，自己画的需要颜色和shape， 模型需要缩放程度
	// position在子类中初始化
	glm::vec3 position, color, scale;
	glm::vec3 shape;
	bool isScene;
	float theta;
private:
	// 属于自己画的物体的属性
	unsigned int VBO, VAO;
	float vertices[180];
};

