// include/drivers/video_drivers.h

#ifndef VIDEO_H
#define VIDEO_H

#include "common.h"

#define MOBX_BUFFER_MAX_SIZE 256

typedef struct {
    u32 width;
    u32 height;
    u32 pitch;
    u32 depth;
    u32 buffer_size;
    u32 bus_address;
    u8* virtual_address; 
}  video_info_t;

void video_init(void);

extern video_info_t video_info;

extern bool video_inited;

void video_draw_px(u32 x, u32 y, u32 color);

void video_fill_screen(u32 color);

#endif