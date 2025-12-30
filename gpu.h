#include <SDL3/SDL.h>
#include <GL/glew.h>

GLuint LoadShader(const char* vertexPath, const char* fragmentPath);
SDL_GLContext *InitGL(SDL_Window *window);