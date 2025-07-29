// src/bgudakrnl.c

//This is the Baguda Kernel source

#include "bgudakrnl.h"
#include <stdint.h>
#include "peripherals/uart.h"
#include "peripherals/gpio_hedr.h"
#include "gpio.h"
#include <stddef.h>
#include "printf.h"
#include "utils.h"
#include "irq.h"
#include "peripherals/timer.h"
#include "peripherals/irq_hedr.h"


//u32 get_el();   //gets exception level

/*void putc(void *p, char c) {
    if (c == '\n') {
        uart_send('\r');
    }
    uart_send(c);
}*/

/*void kernel_main() {
    uart_init();
    init_printf(0, putc);
    
    uart_send_string("UART online!\n");
    gpio_pin_set_func(17, GFOutput);

    while(2){               //this thing signifies that the system booted and has initialized UART.
        gpio_pin_enable(17);
        delay(500);
    }
    printf("Hello, from Baguda Kernel, grapesOS! This system is now running in 64 bit mode.");

    irq_init_vectors();
    enable_interrupt_controller();
    irq_enable();

    printf("\nException Level: %d\n", get_el());

    while(1){
        char c = uart_recv();  //receive a character from UART
        uart_send(c);          //echo the character back
    }  //the infinite loop to keep the kernel running
    
}
*/
void kernel_main() {
    uart_init();
    uart_send_string("Hello world! \n");

    while(1){
        uart_send(uart_recv());
    }
}
