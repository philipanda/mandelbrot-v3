#include <SDL3/SDL.h>
#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>
#include "debug.h"

void CheckShaderCompilation(GLuint shader, const char* type) {
    GLint success;
    GLchar infoLog[1024];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
		SDL_Log("Shader Compilation Error (%s): %s\n", type, infoLog);
    }
}

void CheckShaderLinking(GLuint program) {
    GLint success;
    GLchar infoLog[1024];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 1024, NULL, infoLog);
        SDL_Log("Shader Linking Error: %s\n", infoLog);
    }
}

const char *read_shader_file(const char* path) {
	FILE* file = fopen(path, "r");
	fseek(file, 0, SEEK_END);
    long filesize = ftell(file);
    rewind(file);

	char *buffer = (char*)malloc(filesize+1);
	if(!buffer){
		fclose(file);
		SDL_Log("Failed to allocate memory");
		return NULL;
	}

	size_t read_size = fread(buffer, 1, filesize, file);
	if (read_size != filesize) {
		SDL_Log("Failed to read shader file");
		free(buffer);
		fclose(file);
		return NULL;
	}
	SDL_Log("Shader file %s loaded\n", path);
	fclose(file);
	buffer[filesize] = '\0';
	return (const char*)buffer;
}

GLuint LoadShader(const char* vertexPath, const char* fragmentPath) {
    const GLchar *vShaderCode = read_shader_file(vertexPath);
    const GLchar *fShaderCode = read_shader_file(fragmentPath);
	SDL_CHECK_ERROR(vShaderCode);
	SDL_CHECK_ERROR(fShaderCode);
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	SDL_Log("created shader\n");
    glShaderSource(vertexShader, 1, &vShaderCode, NULL);
	SDL_Log("shader source initialized\n");
    glCompileShader(vertexShader);
    CheckShaderCompilation(vertexShader, "VERTEX");
	SDL_Log("Compiled Vertex shader\n");
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
    glCompileShader(fragmentShader);
    CheckShaderCompilation(fragmentShader, "FRAGMENT");
	SDL_Log("Compiled Fragment shader\n");

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    CheckShaderLinking(shaderProgram);
	SDL_Log("Shader program created\n");

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

void CheckGLError(const char* function) {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        SDL_Log("OpenGL Error in %s: %s\n", function, err);
    }
}

static SDL_GLContext glcontext;
SDL_GLContext *InitGL(SDL_Window *window){
	DEBUG{SDL_Log("Initializing GL");}
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	
	glcontext = SDL_GL_CreateContext(window);
	if (!glcontext){
		SDL_Log("Creating GL Context failed: %s", SDL_GetError());
		return NULL;
	}
	SDL_GL_MakeCurrent(window, glcontext);
	DEBUG{SDL_Log("GL Context created & attached");}

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		SDL_Log("OpenGL failed to init %s\n", glewGetErrorString(err));
		SDL_GL_DestroyContext(glcontext);
		SDL_DestroyWindow(window);
		return NULL;
	}

	if (!SDL_GL_GetCurrentContext()) {
		SDL_Log("No GL context");
		exit(1);
	}
	SDL_Log("Renderer: %s, OpenGL Version: %s\n", glGetString(GL_RENDERER), glGetString(GL_VERSION));

	GLuint dummyVAO;
	glGenVertexArrays(1, &dummyVAO);
	glBindVertexArray(dummyVAO);

	return &glcontext;
}

void free_gpu() {
	SDL_GL_DestroyContext(glcontext);
}
