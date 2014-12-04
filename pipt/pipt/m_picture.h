#include <stdio.h>
#include <stdlib.h>

#include "glew.h"
static float P_WIDTH = 3.0f;

class MPicture{

public:
	void paint();
	friend GLubyte* Gaussian_Blur(GLubyte* pixels, int height, int width);
	friend GLuint load_texture(const char* file_name, bool useGaussianBlur);
	friend int readShaderSource(char *fileName, GLchar **vertexShader, GLchar **fragmentShader);
	friend int installShaders(const GLchar *Vertex, const GLchar *Fragment);
};