// src/mailbox.c

//The mailbox driver API is used to communicate with the VideoCore GPU on the Raspberry Pi.

#include "mailbox.h"
#include "peripherals/base.h"
#include "peripherals/uart.h"
#include "mem.h"
#include "common.h"
#include <stdint.h>
#include <stdbool.h>

typedef struct {            //define basic functionality of mailbox registers
    reg32 read;     //tells where to reaad data from GPU
    reg32 res[5];   //reserved space
    reg32 status;   //tells if the mailbox is full or empty
    reg32 config;   //config register
    reg32 write;    //tells where to send data to GPU
} mailbox_regs;


static inline mailbox_regs* MBX(){                                //return address pointer to physical mailbox registers
    return (mailbox_regs*)(PBASE + 0xB880);                       //offsetting the PBASE address to get the mailbox register address.
}

typedef struct{             //defining our poperty buffer structure
    u32 size;   //tells the total size of the buffer
    u32 code;   //indicates whether the it was a request or a response
    u8 tags[0]; //array of actual tags upon which the request is made.
} property_buffer;

static u32 property_data[1024] __attribute__((aligned(16)));    //A 4 KB aligned buffer to hold the property data, the aligned 16 bit ensures that the buffer starts at an address that is a multiple of 16 bytes, which is a requirement for the mailbox interface.

//Mailbox status flags. Note that these are constants, meaning it's pre-determined.
#define MAIL_FULL 0x80000000    
#define MAIL_EMPTY 0x40000000
#define MAIL_POLL  0xFFFFFFFF //to check if the mailbox is empty or full

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


static void mailbox_write(u8 channel, uintptr_t data){      //writing data to mailbox tuned to 64 bit address space
    while (MBX()->status & MAIL_FULL);
    MBX()->write = (data & 0xFFFFFFF0) | (channel & 0xF); //writing data to mailbox with channel identifier
}

static u32 mailbox_read(u8 channel){        //reading data from mailboxx tuned to the 64 bit address space.
    const u32 timeout = 100000;   //timeout value to prevent infinite loop
    u32 counter = 0;

    while (true){
        while(MBX()->status & MAIL_EMPTY) {
            if (++counter > timeout){

                return 0xFFFFFFFF; //return an error value if timeout occurts
            }
        } 
        //wait until there's data to read
        u32 data = MBX()->read;

        u8 read_channel = (u8)(data & 0xF);     //extract the channel identifier from the data

        if (read_channel == channel){
            return data & 0xFFFFFFF0;       //return the data without the channel identifier
        }
    }
}

bool mailbox_process(mailbox_tag *tag, u32 tag_size){       //process a mailbox tag command
    //clear the property buffer
    int buffer_size = tag_size + 12;    //12 bytes for size, code, and end tag

    memcpy(&property_data[2], tag, tag_size);  //copy the tag data into the property buffer, starting after the size and code fields

    property_buffer *buff = (property_buffer *)property_data;
    buff->size = buffer_size;
    buff->code = RPI_FIRMWARE_STATUS_REQUEST;       //just an indication that this is a request

    property_data[(tag_size + 12) / 4 - 1] = RPI_FIRMWARE_PROPERTY_END; //end tag: so the script knows where to stop reading the property buffer

    mailbox_write(MAIL_TAGS, (uintptr_t)property_data);     //actually write the processed data to the mailbox

    u32 result = mailbox_read(MAIL_TAGS);                                //wait for the response from the GPU by reading from the mailbopx
    if (result == 0xFFFFFFFF){
        uart_printf(ANSI_RED " Fatal error: Mailbox crashed! Reason: Invalid read response. The thread crashed with error code: %x\r\n", result);
        return false;
    }
    memcpy(tag, property_data + 2, tag_size);               //copy the response data back into the original tag structure
    return true;
}

bool mailbox_generic_command(u32 tag_id, u32 id, u32 *value){       //genric mailbox command processor
    mailbox_generic mbx;
    mbx.tag.id = tag_id;
    mbx.tag.value_length = 4;
    mbx.tag.buffer_size = sizeof(mailbox_generic) - sizeof(mailbox_tag);
    mbx.id = id;
    mbx.value = *value;

    if (!mailbox_process((mailbox_tag *)&mbx, sizeof(mbx))){
        uart_send_string(ANSI_RED"Failed to process: Mailbox generic command!\r\n");
        return false;
    }

    *value = mbx.value;
    return true;
}

u32 mailbox_clock_rate(clock_type clock_type) {     //get the clock rate of a specific clock type
    mailbox_clock c;    
    c.tag.id = RPI_FIRMWARE_GET_CLOCK_RATE;
    c.tag.value_length = 0;
    c.tag.buffer_size = sizeof(c) - sizeof(c.tag);
    c.id = clock_type;

    mailbox_process((mailbox_tag *)&c, sizeof(c));
    return c.rate;
}

bool mailbox_power_check(u32 type){     //check the power state of a specific power domain
    mailbox_power p;
    p.tag.id = RPI_FIRMWARE_GET_DOMAIN_STATE;
    p.tag.value_length = 0;
    p.tag.buffer_size = sizeof(p) - sizeof(p.tag);
    p.id = type;
    p.state = ~0;

    mailbox_process((mailbox_tag *)&p, sizeof(p));
    return p.state && p.state != ~0;
}
