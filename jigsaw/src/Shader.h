#ifndef SHADER_H_
#define SHADER_H_

#include <cstdio>
#include "GL/glew.h"
#define NO_SDL_GLEXT
#include "SDL_opengl.h"

class Shader {
  public:
    Shader(GLenum shaderType) : id_(0), type_(shaderType) {
        id_ = glCreateShader(shaderType);
    }
    ~Shader() {
        glDeleteShader(id_);
    }

    GLuint id() const { return id_; }

    bool CompileSource(const char* source) {
        glShaderSource(id_, 1, &source, NULL);
        glCompileShader(id_);

        GLint status;
        glGetShaderiv(id_, GL_COMPILE_STATUS, &status);
        if (status != GL_TRUE) {
            GLint infoLogLength;
            glGetShaderiv(id_, GL_INFO_LOG_LENGTH, &infoLogLength);

            GLchar *strInfoLog = new GLchar[infoLogLength + 1];
            glGetShaderInfoLog(id_, infoLogLength, NULL, strInfoLog);

            const char *strShaderType = NULL;
            switch(type_) {
            case GL_VERTEX_SHADER: strShaderType = "vertex"; break;
            case GL_GEOMETRY_SHADER: strShaderType = "geometry"; break;
            case GL_FRAGMENT_SHADER: strShaderType = "fragment"; break;
            }

            fprintf(stderr, "Compile failure in %s shader:\n%s\n", strShaderType, strInfoLog);
            delete[] strInfoLog;
        }
        return status == GL_TRUE;
    }

  protected:
    GLuint id_;
    GLenum type_;
};

#endif // SHADERPROGRAM_H_