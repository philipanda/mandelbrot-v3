#include "camera.h"

struct camera_t new_camera(){
    return (struct camera_t){
        640, 480, // display 
        4, 3, // viewport
        (struct complex_t){0,0}, //pos
        1.0, //zoom
        50, //max_iter
        1.0, //speed
        true // dirty
    };
}

struct complex_t px_to_pos(const struct camera_t camera, int x, int y) {
    double px = (x/(double)camera.display_width) - 0.5; // [-0.5, 0.5]
    double py = (y/(double)camera.display_height) - 0.5; // [-0.5, 0.5]
    return (struct complex_t){
        camera.pos.r + px * camera.viewport_width,
        camera.pos.i + py * camera.viewport_height
    };
}

int get_camera_view_range(const struct camera_t camera, struct complex_t *start, struct complex_t *end){
    struct complex_t a;
    struct complex_t b;
    a.r = camera.pos.r - (camera.display_width/2 * camera.zoom_factor);
    a.i = camera.pos.i - (camera.display_height/2 * camera.zoom_factor);
    b.r = camera.pos.r + (camera.display_width/2 * camera.zoom_factor);
    b.i = camera.pos.i + (camera.display_height/2 * camera.zoom_factor);
    *start = a;
    *end = b;
    return 0;
}
