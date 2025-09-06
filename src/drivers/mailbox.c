// src/drivers/mailbox.c

//The mailbox driver API is used to communicate with the VideoCore GPU on the Raspberry Pi.

/*
    If you wish to understand this code, then you need to look at the GPU as a literal person, and you 
    as a user asking stuff to it. All of the functions are named in third person view; i.e, mbx_read() 
    is nothing but YOU reading the value returned by the GPU, not that its a function for GPU to read 
    stuff, like in traditional sense.
*/


/* Note on how to call the mailbox functions:
    This script has two main functions: mbx_request() and mbx_multiple_request(). Here's how to use them:
        1. mbx_request() 
            This thing has 4 arguments:
                ->Tag ID is the mailbox tag that you are requesting.
                ->Value size is the input buffer in bytes.
                ->Response size is the size of what GPU will be writing back in bytes.
                ->Value would be the actual input that you're sending to the firmware.
        
        2. mbx_multiple_request()
            This function is mainly used for getting a bunch of stuff in a single message. You need ot define
            the message array that then this function can fill this in. Then you just call that array for output.
            This thing has 2 arguments:
                ->Tags: This one is the array that you defined earlier in your script using mbx_tag_t tags[x] where
                       x is the number of tags. Each of the tag is then a seperate sub-array which consists of the
                       same arguments as mbx_request()
                ->num_tags: This is nothing but the number to tags you define in your parent array.
            How to call the outputs:
                Define it like so:
                    tags[x1].data[y1]
                    Here x1....xn represents your nth tag in the array.
                    The y1....yn represents the nth 32 bit word depending on the tag whether it exprects more data.
                    Be sure to add a placeholder for the return value which the GPU can then overwrite to.
                    Also note that data[0] is generally input and data[1....n] is generally output, GPU never overwrties
                    the first word (data[0]).

    Example usecase pseudocode:
        mbx_tag_t tags[3] = {
            { RPI_FIRMWARE_GET_TEMPERATURE, 8, 0, {0,0} },          // sensor_id=0, placeholder
            { RPI_FIRMWARE_GET_BOARD_REVISION, 4, 0, {0} },          // placeholder
            { RPI_FIRMWARE_ALLOCATE_FRAMEBUFFER, 20, 0, {1920,1080,32,0,0} } // w,h,bpp,etc
        };

        if (mbx_multi_request(tags, 3) == 0) {
            print_string("Temp: "); print_decimal(tags[0].data[1]/1000); print_string("C\n");
            print_string("Board rev: 0x"); print_hexadecimal(tags[1].data[0]); print_string("\n");
            print_string("Framebuffer base: 0x"); print_hexadecimal(tags[2].data[0]); print_string("\n");
        }
                
*/

#include "drivers/mailbox.h"
#include "drivers/base.h"
#include "drivers/uart.h"
#include "kernel/cacheF.h"
#include "common.h"
#include <stdint.h>
#include <stdbool.h>


//Mailbox channels. Note that these are constants, meaning it's pre-determined.
#define MAIL_POWER    0x0 // Mailbox Channel 0: Power Management Interface
#define MAIL_FB       0x1 // Mailbox Channel 1: Frame Buffer
#define MAIL_VUART    0x2 // Mailbox Channel 2: Virtual UART
#define MAIL_VCHIQ    0x3 // Mailbox Channel 3: VCHIQ Interface
#define MAIL_LEDS     0x4 // Mailbox Channel 4: LEDs Interface
#define MAIL_BUTTONS  0x5 // Mailbox Channel 5: Buttons Interface
#define MAIL_TOUCH    0x6 // Mailbox Channel 6: Touchscreen Interface
#define MAIL_COUNT    0x7 // Mailbox Channel 7: Counter
#define MAIL_TAGS     0x8 // Mailbox Channel 8: Tags (ARM to videocore)



#if RPI_VERSION >= 4
    
    #define ARM_TO_BUS_ADDR(addr) ((addr))
#else
    
    #define ARM_TO_BUS_ADDR(addr) ((addr) + 0xC0000000)
#endif

static __attribute__((aligned(16))) u32 mbox_buffer[36];     //Making sure that the buffer is aligned to 16 byte.

//static u32 mbx_size = 0;                                    //Defines the total size of the message buffer

extern void clean_data_cache_for_address(void *addr, u32 size);     //CPU is a nice bastard. It keeps the value in its cache which we need to flush in the main memory so videocore can read




//function to write the mailbox code to the mailbox register
int mbx_write(u32 channel, u32 data){

    
    if (data & 0xF){
        uart_printf(ANSI_RED"Mailbox write data is not 16-byte aligned!"ANSI_RESET);
        return -1;  //Failed; not aligned properly.
    }
    int t = 100000;
    while((MBOX_STATUS & MBOX_FULL) && --t); 
    if (t <= 0) return -1;
    MBOX_WRITE = data | (channel & 0xF);   
    return 0;
}


//function to read the mailbox code from the mailbox register.
int mbx_read(u32 channel){
    int t = 200000;
    for (;;){
        while ((MBOX_STATUS & MBOX_EMPTY) && --t) { }
        if (t <= 0) return -1;
        u32 v = MBOX_READ;
        if ((v & 0xF) == (channel & 0xF)){
            return (int)(v & ~0xF);         // return the read data by casting it to the standard integer
        }
    }
}

static int mbx_wait_response(u32 channel, u32 bus_addr){
    int tries = 200;
    while (tries --) {
        int resp = mbx_read(channel);
        if (resp < 0) return -1;
        if((u32)resp == bus_addr) return 0;
    }
    return -1;
}


u32 mbx_request(u32 tag, u32 value_size, u32 response_size, u32 value) {

    for (int i = 0; i < 36; i++){
        mbox_buffer[i] = 0;
    }

    /* Note the following array list:
        mbox_buffer[0] --> Total message buffer size
        mbox_buffer[1] --> Status code
        mbox_buffer[2] --> Tag ID
        mbox_buffer[3] --> response size
        mbox_buffer[4] --> Activity code (request/response?)
        mbox_buffer[5] --> Response value
    */

    
    mbox_buffer[2] = tag;
    mbox_buffer[3] = response_size;
    mbox_buffer[4] = 0x0; 
    
    if (value_size > 0) {
      mbox_buffer[5] = value;
    }

    
    mbox_buffer[5 + (value_size / 4)] = 0;

    mbox_buffer[0] = (5 + (value_size / 4) + 1) * sizeof(u32); 
    mbox_buffer[1] = 0; 

    clean_data_cache_for_address(mbox_buffer, mbox_buffer[0]);

    u32 bus = ARM_TO_BUS_ADDR((u32)(uintptr_t)mbox_buffer);

    if(mbx_write(MAIL_TAGS, bus) != 0) {
        uart_printf(ANSI_RED"Failed to write to mailbox.\n" ANSI_RESET);
        return 0xFFFFFFFF;
    }
    if(mbx_wait_response(MAIL_TAGS, bus) != 0) {
        uart_printf(ANSI_RED"Failed to get mailbox response.\n" ANSI_RESET);
        return 0xFFFFFFF0;
    }

    if (mbox_buffer[1] != 0x80000000) {
        uart_printf(ANSI_RED"Firmware status: 0x%x\n" ANSI_RESET, mbox_buffer[1]);
        return 0xFFFFFFFF;
    }

   
    return mbox_buffer[5];
}

int mbx_multi_request(mbx_tag_t *tags, int num_tags) {
    for (int i = 0; i < 36; i++) {
        mbox_buffer[i] = 0;
    }
     int offset = 2;    //index offset, first two, i.e are: [0]=total size, [1]=status

     for(int t = 0; t < num_tags; t++){         //note: numtag is just the count of how many tags we sending in a mbx msg
        mbox_buffer[offset++] = tags[t].tag;        
        mbox_buffer[offset++] = tags[t].size;       
        mbox_buffer[offset++] = tags[t].req_code;   

        int words = tags[t].size / 4;               
        for (int i = 0; i < words; i++){
            mbox_buffer[offset++] = tags[t].data[i];    
        }
     }

     mbox_buffer[offset++] = 0;         
     mbox_buffer[0] = offset * sizeof(u32); 
     mbox_buffer[1] = 0;               

    clean_data_cache_for_address(mbox_buffer, mbox_buffer[0]);     

    u32 bus = ARM_TO_BUS_ADDR((u32)(uintptr_t)mbox_buffer);

    if (mbx_write(MAIL_TAGS, bus) != 0) {
        uart_printf(ANSI_RED"Failed to write to the mailbox!\n"ANSI_RESET);
        return -1;
    }
    if (mbx_wait_response(MAIL_TAGS, bus) != 0){
        uart_printf(ANSI_RED"Failed to get the mailbox response!\n"ANSI_RED);
        return -1;
    }

    //copying the results back to tags[] array
    offset = 2;
    for(int t = 0; t < num_tags; t++){
        tags[t].tag         = mbox_buffer[offset++];
        tags[t].size        = mbox_buffer[offset++];
        tags[t].req_code    = mbox_buffer[offset++];

        int words = tags[t].size/4;
        for (int i = 0; i < words; i++){
            tags[t].data[i] = mbox_buffer[offset++];
        }
    }
    return 0;   //success

}
