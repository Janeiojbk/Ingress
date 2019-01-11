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
	// Draw����������������Ƕ���ξ��Լ���������ǵ���ģ��ʹ��model��Draw����
	void draw(Shader ourShader);
	// ������ϵ���Լ�������Ҫ��ɫ��shape�� ģ����Ҫ���ų̶�
	// position�������г�ʼ��
	glm::vec3 position, color, scale;
	glm::vec3 shape;
	bool isScene;
	float theta;
private:
	// �����Լ��������������
	unsigned int VBO, VAO;
	float vertices[180];
};

