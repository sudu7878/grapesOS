#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdint.h>
#include "drivers/video_driver.h"
#include "lib/printf.h"
#include "display/text_rend.h"

void terminal_init(void);
void terminal_clr(void);

void terminal_putc(char c);
void terminal_puts(const char *str);

void terminal_set_fg_color(u32 color);
void terminal_set_bg_color(u32 color);
void terminal_set_text_size(int size);

int terminal_get_cols(void);
int terminal_get_rows(void);

#endif