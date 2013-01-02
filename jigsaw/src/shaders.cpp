// Code wrote by Henrique Gemignani Passos Lima

#include <vector>
#include <cmath>
#include <iostream>
#include <algorithm>
#include "GL/glew.h"
#define NO_SDL_GLEXT
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_opengl.h"

#define NEW_LINE "\n"

GLuint CreateShader(GLenum eShaderType, const std::string &strShaderFile) {
	GLuint shader = glCreateShader(eShaderType);
	const char *strFileData = strShaderFile.c_str();
	glShaderSource(shader, 1, &strFileData, NULL);

	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);

		const char *strShaderType = NULL;
		switch(eShaderType) {
		case GL_VERTEX_SHADER: strShaderType = "vertex"; break;
		case GL_GEOMETRY_SHADER: strShaderType = "geometry"; break;
		case GL_FRAGMENT_SHADER: strShaderType = "fragment"; break;
		}

		fprintf(stderr, "Compile failure in %s shader:\n%s\n", strShaderType, strInfoLog);
		delete[] strInfoLog;
	}

	return shader;
}

GLuint CreateProgram(const std::vector<GLuint> &shaderList) {
	GLuint program = glCreateProgram();

	for(size_t iLoop = 0; iLoop < shaderList.size(); iLoop++)
		glAttachShader(program, shaderList[iLoop]);

	glLinkProgram(program);

	GLint status;
	glGetProgramiv (program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) 	{
		GLint infoLogLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
		fprintf(stderr, "Linker failure: %s\n", strInfoLog);
		delete[] strInfoLog;
	}

	for(size_t iLoop = 0; iLoop < shaderList.size(); iLoop++)
		glDetachShader(program, shaderList[iLoop]);

	return program;
}

GLuint theProgram;

const std::string strVertexShader(
"#version 120" NEW_LINE

"varying vec2 texture_coordinate;" NEW_LINE
"varying vec4 colors;" NEW_LINE

"void main(void) { " NEW_LINE
	"gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;" NEW_LINE
    "texture_coordinate = vec2(gl_MultiTexCoord0);" NEW_LINE
    "colors = gl_Color;" NEW_LINE
"}"
);

const std::string strFragmentShader(
"#version 120" NEW_LINE

"in vec2 texture_coordinate;" NEW_LINE
"in vec4 colors;" NEW_LINE
"uniform sampler2D texture_id;" NEW_LINE

"void main(void) {" NEW_LINE
"   gl_FragColor = texture2D(texture_id, texture_coordinate) * colors;" NEW_LINE
"}"
);

void InitializeProgram()
{
	std::vector<GLuint> shaderList;

	shaderList.push_back(CreateShader(GL_VERTEX_SHADER, strVertexShader));
	shaderList.push_back(CreateShader(GL_FRAGMENT_SHADER, strFragmentShader));

	theProgram = CreateProgram(shaderList);
    glUseProgram(theProgram);

	std::for_each(shaderList.begin(), shaderList.end(), glDeleteShader);
}
