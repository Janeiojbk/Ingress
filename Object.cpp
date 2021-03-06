#include "Object.h"


// 由于Model没有默认构造函数，这里随便给一个对象模型
// 这个是自己画的构造函数
Object::Object(glm::vec3 Color, glm::vec3 Shape)
	:Model("./model/resonator/resonator.obj")
{
	shape = Shape;
	color = Color;
	float Vertices[] = {
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};

	memcpy(vertices, Vertices, sizeof(vertices));
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
}

// 导入模型的构造函数
Object::Object(string const path, glm::vec3 scale, float theta)
	: Model(path)
{	
	this->theta = theta;
	this->scale = scale;
	VAO = 0;
	isScene = false;
}
// Draw驱动函数
void Object::draw(Shader ourShader)
{
	ourShader.use();
	// set up matrix
	glm::mat4 model = glm::mat4(1.0f);

	model = glm::translate(model, position);
	// VAO为0则表示是导入模型
	if (VAO != 0) {
		model = glm::scale(model, shape);
		glm::vec4 color = glm::vec4(Object::color, 1.0f);

		ourShader.setMat4("model", model);
		ourShader.setVec4("ourColor", color);

		// render container
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	else {
		if (position == glm::vec3(0.0f, -0.33f, -1.2f)) {
			model = glm::rotate(model, glm::radians(8.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			ourShader.setMat4("view", glm::mat4());
		}
		model = glm::rotate(model, theta, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, scale);
		
		if(isScene)
			model = glm::translate(model, glm::vec3(-26604.0f, -0.5f, 49336.5f));
		ourShader.setMat4("model", model);
		Draw(ourShader);
	}
}

