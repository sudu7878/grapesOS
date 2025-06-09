// src/uart.c

//this stuff is for the uart driver, and we would be mostly using it for debugging purposes.

#include "gpio.h"
#include "peripherals/gpio_hedr.h"
#include "common.h"
#include "peripherals/uart.h"
#include "peripherals/auxillary.h"


//MAIN MODULAR FUNCTIONS


// UART functions to send data, and this thing here sends single character per data cycle.

void uart_send_char(char c){    //thsi shit here returns the characters
    *UART0DR = (unsigned int)c;
}

void uart_send_string(const char *str){     //this shit here returns the string
    while (*str){
        uart_send_char(*str++);
    }
}

void uart_send_int(int num) {
    char buffer [12]; //upper limit -2147486348\0
    int i = 0;

    if (num == 0) {     //handling 0 explicitly.
        uart_send_char('0');
        return;
    }

    if (num < 0) {
        uart_send_char('-');    //indicated the negative sign
        num = -num;
    }

    while (num > 0) {
        buffer[i++] = (num % 10) + '0';
        num /= 10;
    }

    while (i--){
        uart_send_char(buffer[i]);
    }
}


// initialize driver

void uart_init() {
    gpio_pin_set_func(TXD, GFAlt5); // Set TXD pin 
    gpio_pin_set_func(RXD, GFAlt5); // Set RXD pin 


    gpio_pin_enable(TXD);           // Enable TXD pin
    gpio_pin_enable(RXD);           // Enable RXD pin

    REGS_AUX->enables = 1;
    REGS_AUX->mu_control = 0;
    REGS_AUX->mu_ier = 0;
    REGS_AUX->mu_lcr = 3;
    REGS_AUX->mu_mcr = 0;

    #if RPI_VERSION == 3
        REGS_AUX->mu_baud_rate = 270; // Set baud rate for Raspberry Pi 3, so its like 115200  @250 MHz
    #elif RPI_VERSION == 4
        REGS_AUX->mu_baud_rate = 541; // Set baud rate for Raspberry Pi 4, so its like 115200 @250Mhz
    #endif
        REGS_AUX->mu_control = 3; // Enable transmitter and receiver

        uart_send_char('\n'); // Send a newline character to indicate uart initialization
        uart_send_string("UART initialized successfully!\n"); // Send a confirmation message
}



//UART fucntion to recieve data

char uart_recv(){
    while(!(REGS_AUX->mu_lsr & 1));
    return REGS_AUX->mu_io & 0xFF;
}