// src/bgudakrnl.c

//This is the main kernel file for the BAGUDA kernel.

#include "bgudakrnl.h"
#include <stdint.h>
#include "peripherals/uart.h"
#include "peripherals/gpio_hedr.h"
#include <stddef.h>
#include "printf.h"
#include "irq.h"
#include "peripherals/irq_hedr.h"



/* 
size_t detect_memory(){         //this thing here tries to detect memory by following a simple loop.
    volatile uint32_t *addr;
    size_t step = 1024*1024;     //converting bytes to MB
    size_t max_size = 0;

    for (size_t i = 0x40000000; i < 0x80000000; i += step) {        //currently assuming the memory starts at 1GB and goes up to 2GB
        addr = (volatile uint32_t *)i;
        
        uint32_t old = *addr;
        *addr = 0xDEADBEEF;

        if (*addr == 0xDEADBEEF) {
            *addr = old;
            max_size += step;
        }
        else{
            break;
        }

    }

    return max_size;

}

ERROR: This thing doesnt work here, because memory detection is failing. The reason why its happening because we have not yet 
setup the memory management unit (MMU) and the memory map is not yet established. So if i run this piece of code, it would just
cause a segmentation fault or whaetever memory access violation error, and the kernel would freak out (crash).

So, im gonna just disable this function for now, and when i have the MMU ready, ill enable it. 

*/

u32 get_el();   //gets exception level

void putc(void *p, char c) {
    if (c == '\n') {
        uart_send('\r');
    }
    uart_send(c);
}

void kernel_main() {
    uart_init();
    init_printf(0, putc);
    printf("Hello, from Baguda Kernel, grapesOS! This system is now running in 64 bit mode.");

    irq_init_vectors();
    enable_interrupt_controller();
    irq_enable();

    printf("\nException Level: %d\n", get_el());

    while(1){}  //the infinite loop to keep the kernel running
    
}