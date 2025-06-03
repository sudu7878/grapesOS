// src/bgudakrnl.c

//This is the main kernel file for the BAGUDA kernel.

#include "bgudakrnl.h"
#include "uart.c"




void kernel_main() {
    
    uart_send_string("Hello, World!\n");
    uart_send_string("This is the BAGUDA kernel.\n");
    while (1) {}

}