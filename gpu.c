#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_vulkan.h>
#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>

#define SDL_CHECK_INIT(value) \
do{ \
	if ((void*)value == NULL){ \
		SDL_Log("Error initializing %s: %s\n", #value, SDL_GetError()); \
		return -1; \
	} \
}while(0);

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
	SDL_CHECK_INIT(vShaderCode);
	SDL_CHECK_INIT(fShaderCode);
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

static SDL_Window* window;
static SDL_GLContext glcontext;
static GLuint shaderProgram;
    
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]){
	SDL_CHECK_INIT(SDL_Init(SDL_INIT_VIDEO));
    SDL_SetAppMetadata("Mandelbrot", "3.0", "dev.philipanda.mandelbrot");

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    
    window = SDL_CreateWindow("name", 640, 480, SDL_WINDOW_OPENGL);
    SDL_CHECK_INIT(window);

	glcontext = SDL_GL_CreateContext(window);
	SDL_GL_MakeCurrent(window, glcontext);

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		SDL_Log("OpenGL failed to init %s\n", glewGetErrorString(err));
		SDL_GL_DestroyContext(glcontext);
		SDL_DestroyWindow(window);
		return SDL_APP_FAILURE;
	}
	if (!SDL_GL_GetCurrentContext()) {
		SDL_Log("No GL context");
		exit(1);
	}
	SDL_Log("Renderer: %s, OpenGL Version: %s\n", glGetString(GL_RENDERER), glGetString(GL_VERSION));
    
  	glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GLuint dummyVAO;
	glGenVertexArrays(1, &dummyVAO);
	glBindVertexArray(dummyVAO);

	shaderProgram = LoadShader("mandelbrot.vert.glsl", "mandelbrot.frag.glsl");
	if (!glIsProgram(shaderProgram)) {
		SDL_Log("Invalid program");
	}
	SDL_Log("Shaders loaded!\n");
	return 0;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    switch (event->type) {
        case SDL_EVENT_QUIT: {
            return SDL_APP_SUCCESS;
        }
    }
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }
    return SDL_APP_CONTINUE;
}

static size_t frame_counter = 0;
SDL_AppResult SDL_AppIterate(void *appstate)
{
	glClear(GL_COLOR_BUFFER_BIT);
	
	glUseProgram(shaderProgram);
	CheckGLError("glUseProgram");

	glUniform2f(glGetUniformLocation(shaderProgram, "ScreenSize"), 640.0f, 480.0f);
	CheckGLError("glUniform1i");

	glUniform2f(glGetUniformLocation(shaderProgram, "PlaneMin"), -1.5f, -2.0f);
	CheckGLError("glUniform2f");

	glUniform2f(glGetUniformLocation(shaderProgram, "PlaneMax"), 1.5f, 2.0f);
	CheckGLError("glUniform1f");

	glUniform1ui(glGetUniformLocation(shaderProgram, "MaxIter"), 50u);
	CheckGLError("glUniform1ui");

	glDrawArrays(GL_TRIANGLES, 0, 6);
	CheckGLError("glDrawElements");

	// Ensure the buffer swap happens AFTER drawing
	SDL_GL_SwapWindow(window);

	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    SDL_GL_DestroyContext(glcontext);
    SDL_DestroyWindow(window);
    SDL_Quit();

	SDL_DestroyWindow(window);
}