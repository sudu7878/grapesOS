#include "drivers/video_driver.h"
#include "drivers/mailbox.h"
#include "drivers/uart.h"
#include "lib/utils.h"

video_info_t video_info;

#define BUS_TO_PHYSS(addr) ((addr) & ~0xC0000000)


volatile u32 mbox_buffer[MOBX_BUFFER_MAX_SIZE] __attribute__((aligned(16)));

static void get_display_res(u32* width, u32* height){
    mbx_tag_t tag = {
        .tag = RPI_FIRMWARE_FRAMEBUFFER_GET_PHYSICAL_WIDTH_HEIGHT,
        .size = 8,
        .req_code = 0,
        .data = {0, 0}
    };

    //sending mailbox to get properties of the display connneceted
    if (mbx_multi_request(&tag, 1) == 0 && (tag.req_code & 0x80000000)) {
        *width = tag.data[0];
        *height = tag.data[1];
    } else {
        *width = 1024;
        *height = 768;
    }
}

video_info_t video_init(void) {

    static bool inited = false;

    if (inited){
        uart_printf(ANSI_YELLOW"Uhh: Video drivers are already initialized.\n"ANSI_RESET);
        return video_info;
    } else {
            inited = true;

            u32 scrn_w, scrn_h;
            get_display_res(&scrn_w, &scrn_h);
            video_info.width = scrn_w;
            video_info.height = scrn_h;
            video_info.depth = 32;  //32bpp (BGRA)

            uart_printf("Attempting to send framebuffer request to GPU... w=%d h=%d\n" ANSI_RESET, scrn_w, scrn_h);

            mbx_tag_t tags[4];

            tags[0].tag = RPI_FIRMWARE_FRAMEBUFFER_SET_PHYSICAL_WIDTH_HEIGHT;
            tags[0].size = 8;
            tags[0].req_code = 0;
            tags[0].data[0] = scrn_w;
            tags[0].data[1] = scrn_h;

            tags[1].tag = RPI_FIRMWARE_FRAMEBUFFER_SET_VIRTUAL_WIDTH_HEIGHT;
            tags[1].size = 8;
            tags[1].req_code = 0;
            tags[1].data[0] = scrn_w;
            tags[1].data[1] = scrn_h;

            tags[2].tag = RPI_FIRMWARE_FRAMEBUFFER_SET_DEPTH;
            tags[2].size = 4;
            tags[2].req_code = 0;
            tags[2].data[0] = video_info.depth;

            tags[3].tag = RPI_FIRMWARE_FRAMEBUFFER_ALLOCATE;
            tags[3].size = 8;
            tags[3].req_code = 0;
            tags[3].data[0] = 16;   //aligning to 16 bytes
            tags[3].data[1] = 0;    //intial address 0

            for (int t = 0; t < 10; t++) {
                int rc = mbx_multi_request(tags, 4);

                if (rc == 0 && (tags[3].req_code & 0x80000000)){
                    break;  //we got the framebuffer so breakinh
                } else {
                    uart_printf(ANSI_RED"OOF: Framebuffer allocation failed! Retry attempt: %d, Error code: %x\n" ANSI_RED);

                    if (t == 9){
                        uart_printf(ANSI_RED"OOF: All framebuffer allocation attempts failed.\n"ANSI_RESET);
                        return video_info;
                    }
                    delay(500);
                }
            }
        
            //on success
            if (tags[3].req_code & 0x80000000) {
                video_info.bus_address = tags[3].data[0];
                video_info.buffer_size = tags[3].data[1];
                video_info.virtual_address = (u8*)(uintptr_t)BUS_TO_PHYSS(video_info.bus_address);    //doing bitwise NOT to filter out CPU's perspective of address.
                uart_printf("Wohoo! Framebuffer allocation completed successfully! Bus = %x, size = %x virt_addr = %x\n"ANSI_RESET,
                            video_info.bus_address, video_info.buffer_size, video_info.virtual_address);
            } else {
                uart_printf(ANSI_RED"OOF: Framebuffer allocation failed by GPU.\n" ANSI_RESET);
                return video_info;

            }

            mbx_tag_t pitch_tag = {
                .tag = RPI_FIRMWARE_FRAMEBUFFER_GET_PITCH,
                .size = 4,
                .req_code = 0,
                .data = {0}
            };

            if (mbx_multi_request(&pitch_tag, 1) == 0 && pitch_tag.req_code & 0x80000000) {
                video_info.pitch = pitch_tag.data[0];
                uart_printf("Yay! got the pitch!\n"ANSI_RESET);
            } else{
                uart_printf(ANSI_YELLOW"Uhh: Failed to get the pitch; using the default one.\n"ANSI_RESET);
                video_info.pitch = scrn_w * (video_info.depth/8);
            }
            return video_info;
        }
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
