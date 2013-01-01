// Code wrote by Henrique Gemignani Passos Lima

#include <cmath>
#include <iostream>
#include <algorithm>
#include "GL/glew.h"
#define NO_SDL_GLEXT
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_opengl.h"

char vsFile[1][256] = { "null.vert" };
char fsFile[1][256] = { "null.frag" };

GLhandleARB fShad[1];
GLhandleARB vShad[1];
GLhandleARB gl2Program[2];
GLboolean gl2ProgramStatus[1] = {GL_FALSE};

bool loadShader( GLhandleARB shader, char* fn) {
    FILE *fp;
    bool ret = true;

    if (!(fp = fopen(fn,"rb"))) {
        return false;
    }

    fseek(fp, 0, SEEK_END);
    int length = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    GLubyte *buf = new GLubyte[length+1];

    fread( buf, 1, length, fp);
    buf[length] = '\0'; // make it a regular C string so str* functions work

    glShaderSourceARB( shader, 1, (const char**)&buf, &length);

    if (glGetError() != 0) {
        ret = false;
    }

    fclose(fp);

    delete []buf;
    return ret;
}

GLboolean compileShader( GLhandleARB shader) {
    GLint      infoLogLength;
    GLint      compileStatus;   

    glCompileShaderARB(shader);

    glGetObjectParameterivARB(shader, GL_OBJECT_COMPILE_STATUS_ARB, &compileStatus);
    glGetObjectParameterivARB(shader, GL_OBJECT_INFO_LOG_LENGTH_ARB, &infoLogLength);

    printf("Compile status: %d\n", compileStatus);

    if (infoLogLength) {
        GLcharARB *infoLog = (GLcharARB *) malloc(infoLogLength);
        glGetInfoLogARB(shader, infoLogLength, NULL, infoLog);
        printf("Info Log: %s\n", infoLog);
        free(infoLog);
    }
    return compileStatus;
}

GLboolean linkProgram( GLhandleARB program) {
    GLint      infoLogLength;
    GLint      linkStatus;

    glLinkProgramARB(program);

    glGetObjectParameterivARB(program, GL_OBJECT_LINK_STATUS_ARB, &linkStatus);
    glGetObjectParameterivARB(program, GL_OBJECT_INFO_LOG_LENGTH_ARB, &infoLogLength);

    printf("Link status: %d\n", linkStatus);

    if (infoLogLength) {
        GLcharARB *infoLog = (GLcharARB *) malloc(infoLogLength);
        glGetInfoLogARB(program, infoLogLength, NULL, infoLog);
        printf("Info Log: %s\n", infoLog);
        free(infoLog);
    }
    return linkStatus;
}

void shaders_init() {
    int i = 0;
    vShad[i] = glCreateShaderObjectARB( GL_VERTEX_SHADER_ARB );
    fShad[i] = glCreateShaderObjectARB( GL_FRAGMENT_SHADER_ARB );
    gl2Program[i] = glCreateProgramObjectARB();

    glAttachObjectARB( gl2Program[i], vShad[i] );
    glAttachObjectARB( gl2Program[i], fShad[i] );

    if (!loadShader( vShad[i], vsFile[i])) {
        printf("Warning: unable to load vertex shader file \"%s\"\n", vsFile[i]);
    }
    if (!loadShader( fShad[i], fsFile[i])){
        printf("Warning: unable to load fragment shader file \"%s\"\n", fsFile[i]);
    }

    printf("Compiling vertex shader: '%s'\n", vsFile[i]);
    compileShader( vShad[i] );
    printf("Compiling fragment shader: '%s'\n", fsFile[i]);
    compileShader( fShad[i] );

    printf("Linking program\n");
    gl2ProgramStatus[i] = linkProgram( gl2Program[i] ); // wwlk
}