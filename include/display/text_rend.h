// include/display/text_rend.h

#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include "common.h"

#define FONT_W 8
#define FONT_H 8



void text_init(u32 fg_color, u32 bg_color);
void draw_char(int x, int y, char c, u32 color, int size);
void draw_string(int x, int y, const char *s, u32 color);
void draw_Logo(int x0, int y0);
void draw_Logo_centered_alpha(uint8_t alpha_percent);

void term_init(void);
void term_putc(char c);
void term_puts(const char *s);

extern int cursor_x;
extern int cursor_y;


#endif
