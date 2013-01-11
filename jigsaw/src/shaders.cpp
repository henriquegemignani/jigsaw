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

#include "ShaderProgram.h"
#include "Shader.h"

#define NEW_LINE "\n"

const char* strVertexShader =
"#version 120" NEW_LINE

"varying vec2 texture_coordinate;" NEW_LINE
"varying vec4 colors;" NEW_LINE

"void main(void) { " NEW_LINE
	"gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;" NEW_LINE
    "texture_coordinate = vec2(gl_MultiTexCoord0);" NEW_LINE
    "colors = gl_Color;" NEW_LINE
"}";

const char* strFragmentShader =
"#version 120" NEW_LINE

"varying vec2 texture_coordinate;" NEW_LINE
"varying vec4 colors;" NEW_LINE
"uniform sampler2D texture_id;" NEW_LINE

"void main(void) {" NEW_LINE
"   gl_FragColor = texture2D(texture_id, texture_coordinate) * colors;" NEW_LINE
"}";


void InitializeProgram() {
    Shader s1(GL_VERTEX_SHADER), s2(GL_FRAGMENT_SHADER);
    s1.CompileSource(strVertexShader);
    s2.CompileSource(strFragmentShader);
    ShaderProgram program;
    program.AttachShader(s1);
    program.AttachShader(s2);
    program.SetupProgram();
    glUseProgram(program.id());
}
