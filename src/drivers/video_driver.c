#include "drivers/video_driver.h"
#include "drivers/mailbox.h"
#include "drivers/uart.h"

static video_info_t video_info;

volatile u32 mbox_buffer[MOBX_BUFFER_MAX_SIZE] __attribute__((aligned(16)));

static void get_display_res(u32* width, u32* height){
    mbx_tag_t tag = {
        .tag = RPI_FIRMWARE_FRAMEBUFFER_GET_PHYSICAL_WIDTH_HEIGHT,
        .size = 8,
        .req_code = 0,
        .data = {0, 0}
    };

    //sending mailbox to get properties of the display connneceted
    if (mbx_multi_request(&tag, 1) == 0) {
        *width = tag.data[0];
        *height = tag.data[1];
    } else {
        *width = 1024;
        *height = 768;
    }
}

video_info_t video_init(void) {
    u32 scrn_h, scrn_w;
    get_display_res(&scrn_h, &scrn_w);
    video_info.width = scrn_w;
    video_info.height = scrn_h;
    video_info.depth = 32;  //32bpp (BGRA)

    mbx_tag_t tags[2];

    tags[0].tag = RPI_FIRMWARE_FRAMEBUFFER_SET_PHYSICAL_WIDTH_HEIGHT;
    tags[0].size = 8;
    tags[0].req_code = 0;
    tags[0].data[0] = scrn_w;
    tags[0].data[1] = scrn_h;

    tags[1].tag = RPI_FIRMWARE_FRAMEBUFFER_ALLOCATE;
    tags[1].size = 8;
    tags[1].req_code = 0;
    tags[1].data[0] = 16;   //aligning to 16 bytes
    tags[1].data[1] = 0;    //intial address 0

    if (mbx_multi_request(tags, 2) != 0){
        uart_printf(ANSI_RED"Fatal error: Framebuffer allocation failed!.\n");
        return video_info;
    }

    if (tags[1].req_code == 0x80000000) {
        video_info.bus_address = tags[1].data[0];
        video_info.buffer_size = tags[1].data[1];
        video_info.virtual_address = (u8*)(uintptr_t)((u32)video_info.bus_address);
    } else {
        uart_printf(ANSI_RED"Fatal Error: Framebuffer allocation failed by GPU.\n" ANSI_RESET);
        return video_info;

    }

    mbx_tag_t pitch_tag = {
        .tag = RPI_FIRMWARE_FRAMEBUFFER_GET_PITCH,
        .size = 4,
        .req_code = 0,
        .data = {0}
    };

    if (mbx_multi_request(&pitch_tag, 1) == 0 && pitch_tag.req_code ==  0x80000000) {
        video_info.pitch = pitch_tag.data[0];
    } else{
        uart_printf(ANSI_YELLOW"Warning: Failed to get the pitch; using the default one.\n"ANSI_RESET);
        video_info.pitch = scrn_w * (video_info.depth/8);
    }
    return video_info;
}



//pixer drawer

void video_draw_px(u32 x, u32 y, u32 color){
    if (x >= video_info.width || y >= video_info.height) {
        return;
    }

    u32 offset = y * video_info.pitch + x * (video_info.depth / 8);
    *(volatile u32*)(video_info.virtual_address + offset) = color;
}

//this thing fills the entire screen with a specific color
void video_fill_screen(u32 color){
    for (u32 y = 0; y < video_info.height; y++){
        for(u32 x = 0; x < video_info.width; x++){
            video_draw_px(x, y, color);
        }
    }
}