#define SDL_MAIN_USE_CALLBACKS 1
//#define DEBUG_MODE
//#define DEBUG_CAMERA
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdlib.h>
#include <stdio.h>
#include "mandelbrot_camera.h"
#include "debug.h"
#include "text.h"
#include "gpu.h"

// Rendering states
static unsigned int W = 640;
static unsigned int H = 480;
static double FONT_SCALE = 0.5;
const unsigned int FPS_CAP = 60;
double TARGET_FRAMETIME_NS = 1.0 / FPS_CAP * 1e9;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_GPUDevice *gpu = NULL;

// Logic states
struct camera_t *camera;
const bool *keyboard_states = NULL;


SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    SDL_SetAppMetadata("Mandelbrot", "3.0", "dev.philipanda.mandelbrot");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    window = SDL_CreateWindow("Mandelbrot V3", W, H, SDL_WINDOW_OPENGL);
    SDL_CHECK_ERROR(window);
    DEBUG{SDL_Log("Window created");}

    SDL_GLContext* glcontext = InitGL(window);
    SDL_CHECK_ERROR(glcontext);
    DEBUG{SDL_Log("GL Initialized!");}

    SDL_CHECK_ERROR(init_text_ui((int)(W*FONT_SCALE), (int)(H*FONT_SCALE)));
    DEBUG{SDL_Log("TextUI Initialized");}

    keyboard_states = SDL_GetKeyboardState(NULL);
    SDL_CHECK_ERROR(keyboard_states);
    DEBUG{SDL_Log("Keyboard initialized");}

    camera = init_mandelbrot();
    SDL_CHECK_ERROR(camera);
    camera->display_height=H;
    camera->display_width=W;
    camera->pos = (struct complex_t){0.0, 0.0};
    DEBUG{SDL_Log("Mandelbrot initialized");}

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
    static Uint64 past_tick_start = 0; // will keep the past frame time across iterations, initialized only at the start
    static Uint64 tick_time = 0;
    static double framerate = 0;
    const Uint64 FPS_BUFFER_NS = 1e09;
    static Uint64 fps_buffer_counter_ns = 0;
    static Uint64 fps_buffer_frame_counter = 0;
    Uint64 tick_start = SDL_GetTicksNS();
    Uint64 dt_ns = tick_start - past_tick_start;

    // camera movement
    move_camera(keyboard_states, dt_ns);

    DEBUG_C{
        char* c = malloc(256);
        print_camera(c, 256);
        SDL_Log(c);
        free(c);
    }

    // rendering
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    render_mandelbrot();
    static char ui_buffer[256];
    sprintf(ui_buffer, 
        "FPS:%6.2lf TICK:%7.2lfms\n"
        "PRECISION:%5d RES:%dx%d\n"
        "R: %10.2e I:%10.2e Zoom:%10.2e\n"
        ,framerate, (double)(dt_ns)*1e-6,
        (int)(camera->max_iter), camera->display_width, camera->display_height,
        camera->pos.r, camera->pos.i, camera->zoom
    );
    draw_text(ui_buffer, 0, 0);
    //debug_draw_all_chars(0, 0);
    render_text();

	SDL_GL_SwapWindow(window);

    Uint64 tick_end = SDL_GetTicksNS();

    if ((tick_end-tick_start) < TARGET_FRAMETIME_NS) {
        SDL_DelayNS(TARGET_FRAMETIME_NS - (tick_end-tick_start));
    }
    if ((tick_start - fps_buffer_counter_ns) < FPS_BUFFER_NS) {
        ++fps_buffer_frame_counter;
    } else {
        DEBUG{printf("%d\n", fps_buffer_frame_counter);}
        framerate = (double)fps_buffer_frame_counter / (double)(FPS_BUFFER_NS) * 1e9;
        fps_buffer_frame_counter = 0;
        fps_buffer_counter_ns = tick_start;
    }

    past_tick_start = tick_start;
    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    free_mandelbrot_camera();
    free_text_ui();
    /* SDL will clean up the window/renderer for us. */
}
