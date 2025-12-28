#include "complex.h"
#include <stdbool.h>
#include <SDL3/SDL.h>


struct camera_t {
    unsigned int display_width;
    unsigned int display_height;
    double viewport_width;
    double viewport_height;
    struct complex_t pos;
    double zoom;
    double zoom_speed;
    double max_iter;
    double max_iter_change_speed;
    double move_speed;
    bool dirty;
    SDL_Renderer* renderer;
    SDL_Texture *mandelbrot_texture;
    unsigned int* mandelbrot_texture_array;
};

struct camera_t new_camera(SDL_Renderer *renderer);
struct complex_t px_to_pos(const struct camera_t camera, int x, int y);
int get_camera_view_range(const struct camera_t camera, struct complex_t *start, struct complex_t *end);
void print_camera(const struct camera_t const* camera, char* buf, unsigned int  buf_len);

int find_mandelbrot_depth(const struct complex_t c, unsigned int max_iter);
bool init_mandelbrot_camera(SDL_Renderer *renderer, unsigned int w, unsigned int h);
void free_mandelbrot_camera(struct camera_t camera);
void compute_mandelbrot(struct camera_t *camera);
void render_mandelbrot(const struct camera_t *camera);
void move_camera(struct camera_t *camera, const bool *keyboard_states, unsigned long dt_ns);