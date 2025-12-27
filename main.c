#define SDL_MAIN_USE_CALLBACKS 1
#define DEBUG_MODE
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_vulkan.h>
#include "mandelbrot.h"
#include "camera.h"
#include <stdlib.h>
#include "debug.h"
// Rendering states
static unsigned int W = 640;
static unsigned int H = 480;

const unsigned int FPS_CAP = 60;
const unsigned int TARGET_FRAMETIME_NS = 1e9 / FPS_CAP;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_GPUDevice *gpu = NULL;
SDL_Texture *mandelbrot_texture = NULL;
unsigned int* mandelbrot_texture_array = NULL;

// Logic states
struct camera_t camera;
const bool *keyboard_states = NULL;


SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "gpu");
    SDL_SetHint(SDL_HINT_GPU_DRIVER, "vulkan");
    SDL_SetAppMetadata("Mandelbrot", "3.0", "dev.philipanda.mandelbrot");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("examples/renderer/primitives", W, H, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    gpu = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, NULL, "GPU");
    if (!gpu) {
        SDL_Log("Couldn't create a GPU: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    mandelbrot_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB32, SDL_TEXTUREACCESS_STREAMING, W, H);
    if (!mandelbrot_texture) {
        SDL_Log("Couldn't create texture: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    mandelbrot_texture_array = (unsigned int*)malloc(W * H * sizeof(unsigned int));

    SDL_SetRenderLogicalPresentation(renderer, W, H, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    keyboard_states = SDL_GetKeyboardState(NULL);
    if (!keyboard_states){
        SDL_Log("Couldn't get keyboard state array: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    camera = new_camera();
    camera.display_height=H;
    camera.display_width=W;
    camera.pos = (struct complex_t){0.0, 0.0};
    return SDL_APP_CONTINUE;
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

/* TICK */
SDL_AppResult SDL_AppIterate(void *appstate)
{
    static Uint64 past = 0; // will keep the past frame time across iterations, initialized only at the start
    Uint64 start = SDL_GetTicksNS();
    Uint64 dt_ns = start - past;

    // camera movement
    double zoom = camera.zoom * (1 + camera.zoom_speed * dt_ns * (keyboard_states[SDL_SCANCODE_DOWN]-keyboard_states[SDL_SCANCODE_UP]));
    camera.zoom = zoom > 0.0 ? zoom : 0.0;
    camera.pos.r -= camera.move_speed * camera.zoom * dt_ns * keyboard_states[SDL_SCANCODE_A];
    camera.pos.r += camera.move_speed * camera.zoom * dt_ns * keyboard_states[SDL_SCANCODE_D];
    camera.pos.i -= camera.move_speed * camera.zoom * dt_ns * keyboard_states[SDL_SCANCODE_W];
    camera.pos.i += camera.move_speed * camera.zoom * dt_ns * keyboard_states[SDL_SCANCODE_S];

    camera.dirty +=
        keyboard_states[SDL_SCANCODE_A] +
        keyboard_states[SDL_SCANCODE_D] +
        keyboard_states[SDL_SCANCODE_W] +
        keyboard_states[SDL_SCANCODE_S] +
        keyboard_states[SDL_SCANCODE_DOWN] +
        keyboard_states[SDL_SCANCODE_UP]
        ;


    DEBUG {
        char* c = malloc(256);
        print_camera(&camera, c, 256);
        SDL_Log(c);
        free(c);
    }

    // logic
    if(camera.dirty){
        const int max_iter = 50;
        for (int i = 0; i < H; ++i) {
            for (int r = 0; r < W; ++r) {
                struct complex_t c = px_to_pos(camera, r, i);
                //SDL_Log("start: %f+%fi", c.r, c.i);
                unsigned int depth = find_mandelbrot_depth(c, max_iter);
                mandelbrot_texture_array[i*W + r] = depth * 255 / max_iter; 
            }
        }
    }

    // rendering
    if(camera.dirty) {
        SDL_SetRenderDrawColor(renderer, 33, 33, 33, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);
        SDL_UpdateTexture(mandelbrot_texture, NULL, mandelbrot_texture_array, 4 * W);
        SDL_RenderTexture(renderer, mandelbrot_texture, NULL, NULL);
        SDL_RenderPresent(renderer);
        camera.dirty = false;
    }
    Uint64 dt = SDL_GetTicksNS() - past;
    DEBUG{
        SDL_Log("TICK TIME: %.2fms",dt*1e-6);
    }
    if (dt < TARGET_FRAMETIME_NS) {
        SDL_DelayNS(TARGET_FRAMETIME_NS - dt);
    }
    DEBUG {
        SDL_Log("REAL FPS: %.2f", 1e9/(SDL_GetTicksNS()-past));
    }

    past = start;
    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    /* SDL will clean up the window/renderer for us. */
}
