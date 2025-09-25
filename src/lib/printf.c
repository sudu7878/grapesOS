//src/lib/printf.c

//A custom print function that writes character data to a framebuffer. This supports the inbuilt text renderer.

#include "lib/printf.h"
#include "display/text_rend.h"
#include "drivers/video_driver.h"
#include "lib/memcpy.h"
#include <stdarg.h>
#include <stdint.h>

static u32 fg_color = 0x00FFFFFF;
static u32 bg_color = 0x00000000;

// integer glyph spacing and small vertical gap
static int txt_size = 2;
static int char_spacing = 1;   // integer spacing
static int vert_spacing = 1;

static inline int char_width_px(void){
    return (FONT_W * txt_size) + char_spacing;
}

static inline int line_height_px(void){
    return (FONT_H * txt_size) + vert_spacing;
}

/* simple scroll-up by pixel rows; uses project's memcpy (no libc) */
static void scroll_up_pixels(int pixels) {
    if (!video_info.virtual_address || pixels <= 0) return;
    u32 pitch = video_info.pitch;
    u32 width = video_info.width;
    u32 height = video_info.height;
    u8 *buf = video_info.virtual_address;

    u32 src_off = (u32)pixels * pitch;
    u32 dst_off = 0;
    u32 copy_bytes = (height * pitch) - src_off;

    /* safe forward copy (dst < src) */
    memcpy(buf + dst_off, buf + src_off, copy_bytes);

    /* clear bottom area to bg_color */
    for (u32 yy = height - (u32)pixels; yy < height; yy++) {
        for (u32 xx = 0; xx < width; xx++) {
            /* video_draw_px is used elsewhere; use it to honor pixel format */
            video_draw_px((int)xx, (int)yy, bg_color);
        }
    }
}

void screen_putc(char c){
    int cw = char_width_px();
    int lh = line_height_px();

    if (c == '\r') {
        cursor_x = 0;
        return;
    }

    if (c == '\n') {
        cursor_x = 0;
        cursor_y += lh;
        if (cursor_y + lh > (int)video_info.height) {
            scroll_up_pixels(lh);
            cursor_y -= lh;
            if (cursor_y < 0) cursor_y = 0;
        }
        return;
    }

    /* printable */
    draw_char(cursor_x, cursor_y, c, fg_color, txt_size);
    cursor_x += cw;

    /* wrap when we don't have space for another glyph */
    if (cursor_x + cw > (int)video_info.width) {
        cursor_x = 0;
        cursor_y += lh;
        if (cursor_y + lh > (int)video_info.height) {
            scroll_up_pixels(lh);
            cursor_y -= lh;
            if (cursor_y < 0) cursor_y = 0;
        }
    }
}

void screen_puts(const char *str){
    while(*str){
        screen_putc(*str++);
    }
}

void printf(const char *fmt, ...){
    /* start each printf with a sensible default text size so previous calls
       cannot leave the renderer stuck in a large/small glyph mode */
    txt_size = 2;
     va_list args;
     va_start(args, fmt);

    while(*fmt){
        if(*fmt == '\x01'){             //white 
            fg_color = 0xFFFFFF;
            fmt++;
        } else if (*fmt == '\x02'){     //black
            fg_color = 0x000000;
            bg_color = 0xFFFFFF;
            fmt++;
        } else if (*fmt == '\x03'){     //red
            fg_color = 0xFF0000;
            fmt++;
        } else if (*fmt == '\x04'){     //green
            fg_color = 0x00FF00;
            fmt++;
        } else if (*fmt == '\x05'){     //blue
            fg_color = 0x0000FF;
            fmt++;
        } else if (*fmt == '\x06'){     //yellow
            fg_color = 0xFFFF00;
            fmt++;
        } else if (*fmt == '\x07') {    //cyan
            fg_color = 0x00FFFF;
            fmt++;
        } else if (*fmt == '\x08'){     //magenta
            fg_color = 0xFF00FF;
            fmt++;
        } else if (*fmt == '\x09'){     //gray
            fg_color = 0x808080;
            fmt++;
        } else if (*fmt == '\x0A'){
            txt_size = 1;
            fmt++;
        } else if (*fmt == '\x0B'){
            txt_size = 2;
            fmt++;
        } else if (*fmt == '\x0C'){
            txt_size = 4;
            fmt++;
        } else if (*fmt == '\x0E'){
            fg_color = 0x00FFFFFF;
            bg_color = 0x00000000;
            fmt++;
        }


        else if(*fmt == '%'){

            fmt++;

            if(*fmt == 'd'){
                int num = va_arg(args, int);
                char buf[12];
                int i = 0;

                if(num == 0){
                    screen_putc('0');
                } else{
                    if (num < 0){
                        screen_putc('-');
                        num = -num;
                    }
                    while (num > 0){
                        buf[i++] = (num % 10) + '0';
                        num /= 10;
                    }
                    for (int j = i -1; j>= 0; j--){
                        screen_putc(buf[j]);
                    }
                }

            } else if(*fmt == 'x'){
                unsigned int num = va_arg(args, unsigned int);
                screen_puts("0x");
                char buf[9];
                int i = 0;
                if (num == 0){
                    screen_putc('0');

                } else{
                    while(num > 0){
                        int digit = num &0xF;
                        buf [i++] = (digit < 10) ? (digit + '0') : (digit - 10 + 'A');
                        num >>= 4;
                    }
                    for (int j = i-1; j>=0; j--){
                        screen_putc(buf[j]);
                    }
                }

            } else if(*fmt == 'c'){
                screen_putc((char)va_arg(args, int));

            } else if(*fmt == 's'){
                char *str = va_arg(args, char*);
                while (*str) screen_putc((*str++));

            } else {
                screen_putc(*fmt);
            }

            fmt ++;
        } else{
            screen_putc(*fmt++);
        }
    }
    va_end(args);
}
