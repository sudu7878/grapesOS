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


uint64_t get_el(void);


void kernel_main() {
    uart_init();
    uart_send_string("Hello from grapesOS!\n");
    uart_send_string("System is now running in 64 bit mode.\n");
    uart_send_string("Current EL: ");
    uart_send(get_el() + '0');  //convert the exception level to char and send it
    uart_send_string("\n");

    while(1){
        uart_send(uart_recv());
    }
}