#include "tools.h"

#include <iostream>
#include "SDL_opengl.h"

bool CheckGLError(char *file, int line) {
    bool ret = false;
    GLenum glErr;
    while ((glErr = glGetError()) != GL_NO_ERROR) {
        const GLubyte* sError = gluErrorString(glErr);
        
        std::cout << "GL Error #" << glErr;
        if (sError)
            std::cout << " (" << gluErrorString(glErr) << ")";
        else
            std::cout << " (no message available)";
        std::cout  << " in File " << file << " at line: " << line << std::endl;
        
        ret = true;
    }
    return ret;
}