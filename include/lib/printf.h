#pragma once
#include <stdarg.h>

//24-but rgb colors
#define TXT_WHITE   "\x01"
#define TXT_BLACK   "\x02"
#define TXT_RED     "\x03"
#define TXT_GREEN   "\x04"
#define TXT_BLUE    "\x05"
#define TXT_YELLOW  "\x06"
#define TXT_CYAN    "\x07"
#define TXT_MAGENTA "\x08"
#define TXT_GRAY    "\x09"
#define TXT_REST    "\x0E"  

void printf(const char *fmt, ...);
void screen_putc(char c);
void screen_puts(const char *s);