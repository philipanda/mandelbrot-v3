#include "mandelbrot_camera.h"
#include <stdio.h>
#include <stdlib.h>

struct camera_t new_camera(SDL_Renderer *renderer){
    struct camera_t camera;
    camera.renderer = renderer;
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
    camera.mandelbrot_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB32, SDL_TEXTUREACCESS_STREAMING, camera.display_width, camera.display_height);
    SDL_SetTextureBlendMode(camera.mandelbrot_texture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureScaleMode(camera.mandelbrot_texture, SDL_SCALEMODE_NEAREST);
    camera.mandelbrot_texture_array = (unsigned int*)malloc(camera.display_width * camera.display_height * sizeof(unsigned int));

    return camera;
}

void free_mandelbrot_camera(struct camera_t camera){
   free(camera.mandelbrot_texture_array);
}

int get_camera_view_range(const struct camera_t camera, struct complex_t *start, struct complex_t *end){
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

struct complex_t px_to_pos(const struct camera_t camera, int x, int y) {
    double px = (x/(double)camera.display_width) - 0.5; // [-0.5, 0.5]
    double py = (y/(double)camera.display_height) - 0.5; // [-0.5, 0.5]
    return (struct complex_t){
        camera.pos.r + (px * camera.viewport_width * camera.zoom),
        camera.pos.i + (py * camera.viewport_height * camera.zoom)
    };
}

void print_camera(const struct camera_t const* camera, char* buf, unsigned int buf_len) {
    sprintf(buf, "camera: %f+%fi, [%f:%f], z:%f", camera->pos.r, camera->pos.i, camera->viewport_width, camera->viewport_height, camera->zoom);
}

int find_mandelbrot_depth(const struct complex_t c, unsigned int max_iter) {
  struct complex_t z = {0,0};
  unsigned int n = 0;

  while (complex_modulus_squared(z) <= 4 && n < max_iter) {
    z = complex_square(z);
    complex_add(&z, c);
    ++n;
  }
  return n + 1;
}

void render_mandelbrot(const struct camera_t *camera) {
    SDL_UpdateTexture(camera->mandelbrot_texture, NULL, camera->mandelbrot_texture_array, 4 * camera->display_width);
    SDL_RenderTexture(camera->renderer, camera->mandelbrot_texture, NULL, NULL);
}

void compute_mandelbrot(struct camera_t *camera) {
    const int max_iter = camera->max_iter;
    for (int i = 0; i < camera->display_height; ++i) {
        for (int r = 0; r < camera->display_width; ++r) {
            struct complex_t c = px_to_pos(*camera, r, i);
            unsigned int depth = find_mandelbrot_depth(c, max_iter);
            camera->mandelbrot_texture_array[i*camera->display_width + r] = depth * 255 / max_iter; 
        }
    }
}

void move_camera(struct camera_t *camera, const bool *keyboard_states, unsigned long dt_ns) {
    double zoom = camera->zoom * (1 + camera->zoom_speed * dt_ns * (keyboard_states[SDL_SCANCODE_S]-keyboard_states[SDL_SCANCODE_W]));
    camera->zoom = zoom > 0.0 ? zoom : 0.0;
    double max_iter = camera->max_iter * (1 + camera->max_iter_change_speed * dt_ns * (keyboard_states[SDL_SCANCODE_Q]-keyboard_states[SDL_SCANCODE_A]));
    camera->max_iter = max_iter > 0.0 ? max_iter : 0.0;
    camera->pos.r -= camera->move_speed * camera->zoom * dt_ns * keyboard_states[SDL_SCANCODE_LEFT];
    camera->pos.r += camera->move_speed * camera->zoom * dt_ns * keyboard_states[SDL_SCANCODE_RIGHT];
    camera->pos.i -= camera->move_speed * camera->zoom * dt_ns * keyboard_states[SDL_SCANCODE_UP];
    camera->pos.i += camera->move_speed * camera->zoom * dt_ns * keyboard_states[SDL_SCANCODE_DOWN];

    camera->dirty +=
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