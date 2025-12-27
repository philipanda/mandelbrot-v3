#ifndef TEXT_H
#define TEXT_H

// https://8bitworkshop.com/bitmapfontgenerator/
// aaron 4thd
#include <SDL3/SDL.h>
#include <stdbool.h>

#define LOCHAR ' '
#define HICHAR 128
#define FONT_BWIDTH 1
#define FONT_HEIGHT 8
extern const char FONT[HICHAR-LOCHAR+1][FONT_HEIGHT*FONT_BWIDTH];

void draw_char(char c, int x, int y);
void draw_text(const char *text, int x, int y);
void free_text_ui();
bool init_text_ui(SDL_Renderer *renderer, unsigned int W, unsigned int H);
unsigned int render_text(SDL_Renderer *renderer);
void debug_draw_all_chars(int x, int y);
#endif