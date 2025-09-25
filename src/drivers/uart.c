// src/drivers/uart.c

//mini UART interface controller

#include "drivers/gpio.h"
#include "lib/utils.h"
#include "drivers/auxillary.h"
#include "drivers/uart.h"
#include <stdarg.h>

#define TXD 14      //define GPIO 14 or pin 8 as TX (for transmitting)
#define RXD 15      //define GPIO 15 or pin 10 as RX (for recieving)

#define RX_BUF_SIZE 256
static volatile unsigned int rx_head = 0, rx_tail = 0;
static volatile char rx_buf[RX_BUF_SIZE];

void uart_push_rx(char c){
    unsigned int next_head = (rx_head + 1)%RX_BUF_SIZE;
    if (next_head == rx_tail){
        return;
    }
    rx_buf[rx_head] = c;
    rx_head = next_head;
}

int uart_rb_data_ready(void){
    return rx_head != rx_tail;
}

void uart_init() {
    gpio_pin_set_func(TXD, GFAlt5);
    gpio_pin_set_func(RXD, GFAlt5);

    gpio_pin_enable(TXD);
    gpio_pin_enable(RXD);

    REGS_AUX->enables |= 1;
    REGS_AUX->mu_control = 0;
    REGS_AUX->mu_ier = 0;   //change this again!!
    REGS_AUX->mu_lcr = 3;
    REGS_AUX->mu_iir = 0xC6; // FIFO enabled, clear both TX and RX FIFOs
    REGS_AUX->mu_mcr = 0;

    #if RPI_VERSION == 4
         REGS_AUX->mu_baud_rate = 541; // = 115200 @ 500 Mhz (for rpi 4)
    #endif
    #if RPI_VERSION == 3
         REGS_AUX->mu_baud_rate = 270; // = 115200 @ 250 Mhz (for rpi 3)
    #endif


    REGS_AUX->mu_control = 3;

    uart_send('\r');
    uart_send('\n');
    uart_send('\n');
}

int uart_data_ready(void){
    return (REGS_AUX->mu_lsr & 0x01) ? 1: 0;
}

char uart_recv_nb(void){
    if (uart_data_ready()){
        return REGS_AUX->mu_io & 0xFF;
    } else {
        return -1;  // Indicate no data available
    }
}

void uart_send(char c) {                            //send basic characters
    while(!(REGS_AUX->mu_lsr & 0x20));

    REGS_AUX->mu_io = c;
}

char uart_recv() {                                  //recieve basic characters
    while (!uart_data_ready()){}
    return uart_recv_nb();
}

void uart_send_string(char *str) {                  //send strings of characters    
    while(*str) {
        if (*str == '\n') {
            uart_send('\r');
        }
        uart_send(*str);
        str++;
    }
}

void uart_send_int(int num) {                       //send integers
    if (num == 0) {
        uart_send('0');
        return;
    }

    if (num < 0) {
        uart_send('-');
        num = -num;
    }

    char buffer[12];
    int i = 0;

    while (num > 0) {
        buffer[i++] = (num % 10) + '0';
        num /= 10;
    }

    for (int j = i - 1; j >= 0; j--) {
        uart_send(buffer[j]);
    }
}

void uart_send_hex(unsigned int num) {                 //send hexadecimals
    uart_send_string("0x"); //prefix for hexadecimals
    if (num == 0) {
        uart_send('0');
        return;
    }

    char buffer[9];
    int i = 0;

    while (num > 0) {
        int digit = num & 0xF;
        if (digit < 10) {       //0-9 classification
            buffer[i++] = digit + '0';
        } else {                //A-F classification
            buffer[i++] = digit - 10 + 'A';
        }
        num >>= 4;      //divide by 16 since hexadecimals are base 16 number sys
    }

    for (int j = i - 1; j >= 0; j--) {      //print in reverse order
        uart_send(buffer[j]);
    }
}

void uart_printf(const char *fmt, ...){                 //formatted print function that works with all data types
    va_list args;
    va_start(args, fmt);

    while(*fmt) {
        if(*fmt == '\n'){
           uart_send('\r');
        }
        if(*fmt == '%'){
            fmt++;  // move past '%'

            if(*fmt=='d') uart_send_int(va_arg(args, int));                 //send decimals
            else if(*fmt=='x') uart_send_hex(va_arg(args, unsigned int));   //send hexadecimals
            else if(*fmt=='c') uart_send((char)va_arg(args, int));          //send characters
            else if(*fmt=='s') uart_send_string(va_arg(args, char*));       //send strings
            else uart_send(*fmt); 

            fmt++;  

        } else {                //regular character, just send it without classifying
            uart_send(*fmt);
            fmt++;  
        }
    }

    va_end(args);
}
