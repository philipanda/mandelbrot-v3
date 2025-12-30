#include "mandelbrot_camera.h"
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include "gpu.h"

static struct camera_t camera;
static GLint shader_screensize;
static GLint shader_planemin;
static GLint shader_planemax;
static GLint shader_maxiter;

struct camera_t *init_mandelbrot() {
    camera.dirty = true;
    camera.display_height = 480;
    camera.display_width = 640;
    camera.viewport_height = 3;
    camera.viewport_width = 4;
    camera.move_speed = 1e-9;
    camera.pos = (struct complex_t){0,0};
    camera.max_iter = 50;
    camera.zoom = 1.0;
    camera.zoom_speed = 1e-9;
    camera.max_iter_change_speed = 1e-10;
    camera.mandelbrot_texture_array = (unsigned int*)malloc(camera.display_width * camera.display_height * sizeof(unsigned int));
    camera.shader = LoadShader("mandelbrot.vert.glsl", "mandelbrot.frag.glsl");
    camera.highpshader = LoadShader("mandelbrot.vert.glsl", "mandelbrot_highp.frag.glsl");
    camera.highp = false;
    
    shader_screensize = glGetUniformLocation(camera.shader, "ScreenSize");
    shader_planemin = glGetUniformLocation(camera.shader, "PlaneMin");
    shader_planemax = glGetUniformLocation(camera.shader, "PlaneMax");
    shader_maxiter = glGetUniformLocation(camera.shader, "MaxIter");
    
    return &camera;
}

void free_mandelbrot_camera(){
   free(camera.mandelbrot_texture_array);
}

int get_camera_view_range(struct complex_t *start, struct complex_t *end){
    struct complex_t a;
    struct complex_t b;
    a.r = camera.pos.r - (camera.display_width/2 * camera.zoom);
    a.i = camera.pos.i - (camera.display_height/2 * camera.zoom);
    b.r = camera.pos.r + (camera.display_width/2 * camera.zoom);
    b.i = camera.pos.i + (camera.display_height/2 * camera.zoom);
    *start = a;
    *end = b;
    return 0;
}

struct complex_t px_to_pos(int x, int y) {
    double px = (x/(double)camera.display_width) - 0.5; // [-0.5, 0.5]
    double py = (y/(double)camera.display_height) - 0.5; // [-0.5, 0.5]
    return (struct complex_t){
        camera.pos.r + (px * camera.viewport_width * camera.zoom),
        camera.pos.i + (py * camera.viewport_height * camera.zoom)
    };
}

void print_camera(char* buf, unsigned int buf_len) {
    sprintf(buf, "camera: %f+%fi, [%f:%f], z:%f", camera.pos.r, camera.pos.i, camera.viewport_width, camera.viewport_height, camera.zoom);
}

void render_mandelbrot() {
    struct complex_t p0 = px_to_pos(0, 0);
    struct complex_t p1 = px_to_pos(camera.display_width, camera.display_height);
    SDL_Log("iter: %lf\n", camera.max_iter);
    if (camera.highp){
        glUseProgram(camera.highpshader);
        glUniform2d(shader_screensize, (double)camera.display_width, (double)camera.display_height);
        glUniform2d(shader_planemin, p0.r, p0.i);
        glUniform2d(shader_planemax, p1.r, p1.i);
    } else {
        glUseProgram(camera.shader);
        glUniform2f(shader_screensize, (float)camera.display_width, (float)camera.display_height);
        glUniform2f(shader_planemin, p0.r, p0.i);
        glUniform2f(shader_planemax, p1.r, p1.i);
    }
    glUniform1ui(shader_maxiter, (unsigned int)camera.max_iter); 
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void move_camera(const bool *keyboard_states, unsigned long dt_ns) {
    double zoom = camera.zoom * (1 + camera.zoom_speed * dt_ns * (keyboard_states[SDL_SCANCODE_S]-keyboard_states[SDL_SCANCODE_W]));
    camera.zoom = zoom > 0.0 ? zoom : 0.0;
    double max_iter = camera.max_iter * (1 + camera.max_iter_change_speed * dt_ns * (keyboard_states[SDL_SCANCODE_Q]-keyboard_states[SDL_SCANCODE_A]));
    camera.max_iter = max_iter > 0.0 ? max_iter : 0.0;
    camera.pos.r -= camera.move_speed * camera.zoom * dt_ns * keyboard_states[SDL_SCANCODE_LEFT];
    camera.pos.r += camera.move_speed * camera.zoom * dt_ns * keyboard_states[SDL_SCANCODE_RIGHT];
    camera.pos.i += camera.move_speed * camera.zoom * dt_ns * keyboard_states[SDL_SCANCODE_UP];
    camera.pos.i -= camera.move_speed * camera.zoom * dt_ns * keyboard_states[SDL_SCANCODE_DOWN];
    if(keyboard_states[SDL_SCANCODE_1] && camera.highp || keyboard_states[SDL_SCANCODE_2] && !camera.highp)
        camera.highp = !camera.highp;
    camera.dirty +=
        keyboard_states[SDL_SCANCODE_Q] +
        keyboard_states[SDL_SCANCODE_A] +
        keyboard_states[SDL_SCANCODE_W] +
        keyboard_states[SDL_SCANCODE_S] +
        keyboard_states[SDL_SCANCODE_E] +
        keyboard_states[SDL_SCANCODE_D] +
        keyboard_states[SDL_SCANCODE_DOWN] +
        keyboard_states[SDL_SCANCODE_UP] + 
        keyboard_states[SDL_SCANCODE_LEFT] +
        keyboard_states[SDL_SCANCODE_RIGHT] 
        ;
}