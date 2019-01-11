#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Texture2D
{
public:
	GLuint ID;
	int Width, Height, nrChannels;
	GLuint Internal_Format;
	GLuint Image_Format;
	GLuint Wrap_S;
	GLuint Wrap_T;
	GLuint Filter_Min;
	GLuint Filter_Max;

	Texture2D(const GLchar *file, GLboolean alpha);

	void Generate(GLuint width, GLuint height, unsigned char * data);

	void Bind() const;
};

#endif // !TEXTURE_H
