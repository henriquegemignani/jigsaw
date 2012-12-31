#ifndef TOOLS_H_
#define TOOLS_H_

bool CheckGLError(char *file, int line);
   
#define CHECK_GL_ERROR() CheckGLError(__FILE__, __LINE__)

#endif // TOOLS_H_