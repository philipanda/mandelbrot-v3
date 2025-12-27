#include "complex.h"
#include <stdbool.h>
struct camera_t {
    unsigned int display_width;
    unsigned int display_height;
    double viewport_width;
    double viewport_height;
    struct complex_t pos;
    double zoom;
    double zoom_speed;
    double max_iter;
    double move_speed;
    bool dirty;
};

struct camera_t new_camera();

struct complex_t px_to_pos(const struct camera_t camera, int x, int y);

int get_camera_view_range(const struct camera_t camera, struct complex_t *start, struct complex_t *end);

void print_camera(const struct camera_t const* camera, char* buf, unsigned int  buf_len);