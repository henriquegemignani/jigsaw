#ifndef SHADERPROGRAM_H_
#define SHADERPROGRAM_H_

#include <cstdio>
#include "GL/glew.h"
#define NO_SDL_GLEXT
#include "SDL_opengl.h"

#include "Shader.h"

class ShaderProgram {
public:
    ShaderProgram() : id_(0) {
        id_ = glCreateProgram();
    }
    ~ShaderProgram() {
        glDeleteProgram(id_);
    }

    GLuint id() const { return id_; }

    bool IsValid() const {
        return glIsProgram(id_) == GL_TRUE;
    }
    
    void AttachShader(const Shader& shader) {
        glAttachShader(id_, shader.id());
    }

    bool SetupProgram() {
        glLinkProgram(id_);

        GLint status;
        glGetProgramiv (id_, GL_LINK_STATUS, &status);
        if (status != GL_TRUE) 	{
            GLint infoLogLength;
            glGetProgramiv(id_, GL_INFO_LOG_LENGTH, &infoLogLength);

            GLchar *strInfoLog = new GLchar[infoLogLength + 1];
            glGetProgramInfoLog(id_, infoLogLength, NULL, strInfoLog);
            fprintf(stderr, "Linker failure: %s\n", strInfoLog);
            delete[] strInfoLog;
        }
        return status == GL_TRUE;
    }

private:
    GLuint id_;
};

#endif // SHADERPROGRAM_H_