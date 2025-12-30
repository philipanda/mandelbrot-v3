#include "complex.h"
#include <stdbool.h>
#include <SDL3/SDL.h>
#include <GL/glew.h>

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
    bool highp;
    GLuint shader;
    GLuint highpshader;
    unsigned int* mandelbrot_texture_array;
};

struct camera_t *init_mandelbrot();
struct complex_t px_to_pos(int x, int y);
int get_camera_view_range(struct complex_t *start, struct complex_t *end);
void print_camera(char* buf, unsigned int  buf_len);

int find_mandelbrot_depth(const struct complex_t c, unsigned int max_iter);
void free_mandelbrot_camera();
void render_mandelbrot();
void move_camera(const bool *keyboard_states, unsigned long dt_ns);