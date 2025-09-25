#include "kernel/terminal.h"
#include "drivers/video_driver.h"
#include "display/text_rend.h"
#include <stdint.h>
#include "lib/memcpy.h"

/* Terminal state */
static u32 t_fg = 0x00FFFFFF;
static u32 t_bg = 0x00000000;

static int t_size = 2;
static int t_spacing = 1;

extern int cursor_x;
extern int cursor_y;

/* compute glyph metrics in pixels */
static inline int glyph_w(void) { return FONT_W * t_size; }
static inline int glyph_h(void) { return FONT_H * t_size; }
static inline int step_x(void)  { return glyph_w() + t_spacing; }
static inline int step_y(void)  { return glyph_h() + 1; }

/* color / size setters (defined before terminal_init so callers see them) */
void terminal_set_color(u32 fg, u32 bg){
    t_fg = fg;
    t_bg = bg;
    /* text_init is optional — draw_char accepts color per-glyph.
       If you have a text_init implementation that must be called, uncomment: */
    /* text_init(t_fg, t_bg); */
}

void terminal_set_fg_color(u32 color){
    t_fg = color;
    /* text_init(t_fg, t_bg); */
}

void terminal_set_bg_color(u32 color){
    t_bg = color;
    /* text_init(t_fg, t_bg); */
}

void terminal_set_size(int size){
    if (size <= 0) return;
    t_size = size;
}

void terminal_set_text_size(int size){
    terminal_set_size(size);
}

/* clear a glyph-sized rectangle (clip-safe) */
static void clear_glyph_at(int x, int y){
    if (!video_info.virtual_address) return;
    int w = glyph_w();
    int h = glyph_h();
    if (x < 0) { w += x; x = 0; }
    if (y < 0) { h += y; y = 0; }
    if (x >= (int)video_info.width || y >= (int)video_info.height) return;
    if (x + w > (int)video_info.width) w = video_info.width - x;
    if (y + h > (int)video_info.height) h = video_info.height - y;
    for (int yy = 0; yy < h; yy++){
        for (int xx = 0; xx < w; xx++){
            video_draw_px(x + xx, y + yy, t_bg);
        }
    }
}

/* scroll the framebuffer up by 'pixels' vertically */
static void scroll_up_pixels(int pixels){
    if (!video_info.virtual_address) return;
    if (pixels <= 0) return;
    const uint32_t pitch = video_info.pitch;
    const uint32_t height = video_info.height;
    const uint32_t width = video_info.width;
    uint8_t *buf = video_info.virtual_address;

    uint32_t bytes_move = (height - (uint32_t)pixels) * pitch;
    uint8_t *src = buf + (uint32_t)pixels * pitch;
    uint8_t *dst = buf;

    /* dst < src here, forward copy is safe — use project memcpy */
    memcpy(dst, src, bytes_move);

    /* clear the bottom area */
    for (uint32_t yy = height - (uint32_t)pixels; yy < height; yy++){
        for (uint32_t xx = 0; xx < width; xx++){
            video_draw_px(xx, yy, t_bg);
        }
    }
}

void terminal_clr(void){
    /* ensure renderer/bg color used by clearing routines */
    /* terminal_set_color(t_fg, t_bg); */
    video_fill_screen(t_bg);
    cursor_x = 0;
    cursor_y = 0;
}

/* single terminal_init implementation */
void terminal_init(void){
    cursor_x = 0;
    cursor_y = 0;
    /* terminal_set_color(t_fg, t_bg); */
}

/* public helpers */
int terminal_get_cols(void){
    int sx = step_x();
    if (sx <= 0) return 0;
    return video_info.width / sx;
}
int terminal_get_rows(void){
    int sy = step_y();
    if (sy <= 0) return 0;
    return video_info.height / sy;
}

void terminal_putc(char c){
    /* carriage return */
    if (c == '\r'){
        cursor_x = 0;
        return;
    }

    /* newline */
    if (c == '\n'){
        cursor_x = 0;
        cursor_y += step_y();
        if (cursor_y + glyph_h() > (int)video_info.height){
            scroll_up_pixels(step_y());
            cursor_y -= step_y();
            if (cursor_y < 0) cursor_y = 0;
        }
        return;
    }

    /* backspace (BS or DEL) */
    if (c == '\b' || (unsigned char)c == 127){
        if (cursor_x >= step_x()){
            cursor_x -= step_x();
        } else if (cursor_y >= step_y()){
            cursor_y -= step_y();
            int cols = terminal_get_cols();
            if (cols > 0) cursor_x = (cols - 1) * step_x();
            else cursor_x = 0;
        } else {
            cursor_x = 0;
            cursor_y = 0;
        }
        clear_glyph_at(cursor_x, cursor_y);
        return;
    }

    /* printable */
    draw_char(cursor_x, cursor_y, c, t_fg, t_size);
    cursor_x += step_x();

    /* wrap */
    if (cursor_x + glyph_w() > (int)video_info.width){
        cursor_x = 0;
        cursor_y += step_y();
        if (cursor_y + glyph_h() > (int)video_info.height){
            scroll_up_pixels(step_y());
            cursor_y -= step_y();
            if (cursor_y < 0) cursor_y = 0;
        }
    }
}

void terminal_puts(const char *s){
    while (*s){
        unsigned char c = (unsigned char)*s++;

        if(c >= 0x01 && c <= 0x09){
            switch(c){
                case 0x01: terminal_set_fg_color(0x00FFFFFF); break; // white
                case 0x02: terminal_set_fg_color(0x00000000); break; // black
                case 0x03: terminal_set_fg_color(0x00FF0000); break; // red
                case 0x04: terminal_set_fg_color(0x0000FF00); break; // green
                case 0x05: terminal_set_fg_color(0x000000FF); break; // blue
                case 0x06: terminal_set_fg_color(0x00FFFF00); break; // yellow
                case 0x07: terminal_set_fg_color(0x0000FFFF); break; // cyan
                case 0x08: terminal_set_fg_color(0x00FF00FF); break; // magenta
                case 0x09: terminal_set_fg_color(0x00808080); break; // gray
            }
            continue;
        }
        terminal_putc((char)c);
    }
}