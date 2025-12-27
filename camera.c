#include "camera.h"
#include <stdio.h>

struct camera_t new_camera(){
    struct camera_t camera;
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
    
    return camera;
}

struct complex_t px_to_pos(const struct camera_t camera, int x, int y) {
    double px = (x/(double)camera.display_width) - 0.5; // [-0.5, 0.5]
    double py = (y/(double)camera.display_height) - 0.5; // [-0.5, 0.5]
    return (struct complex_t){
        camera.pos.r + (px * camera.viewport_width * camera.zoom),
        camera.pos.i + (py * camera.viewport_height * camera.zoom)
    };
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

void print_camera(const struct camera_t const* camera, char* buf, unsigned int buf_len) {
    sprintf(buf, "camera: %f+%fi, [%f:%f], z:%f", camera->pos.r, camera->pos.i, camera->viewport_width, camera->viewport_height, camera->zoom);
}