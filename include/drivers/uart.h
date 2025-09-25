// include/drivers/uart.h

//*Mini UART is a inteface in pi that is the derivation of the main UART or PL011 UART. This thing is used by bluetooth module by default, plus its much more advanced and difficult to set up, so I used mini uart instead.


#pragma once

#include "gpio.h"
#include "drivers/gpio_hedr.h"
#include "common.h"
#include "drivers/auxillary.h"

#define UART0DR ((volatile unsigned int *)0x9000000) // UART0 Data Register in ARM architecture (raspberry pi)

//defining nthe TX and RX pins

#define TXD 14  //set the TX UART pin 
#define RXD 15  //set the RX UART pin

//define terminal colors boom boom hacker vibes!
#define ANSI_RESET      "\033[0m"
#define ANSI_RED        "\033[31m"
#define ANSI_GREEN      "\033[32m"
#define ANSI_YELLOW     "\033[33m"
#define ANSI_BLUE       "\033[34m"
#define ANSI_MAGENTA    "\033[35m"
#define ANSI_CYAN       "\033[36m"

//(also i defined it here because it just makes it so much easier to use it everywhere around the proejct)

//a way to debug stuff for the kernel using the UART interface


void uart_send(char c);                     //thsi thing here returns the characters

void uart_send_string(char *str);     //this thing here returns strings, USING the characters

void uart_send_int(int num);                //this thing here returns integers

void uart_send_hex(unsigned int num);       //this thing here returns hexadecimals

void uart_push_rx(char c);


int uart_data_ready(void);
int uart_rb_data_ready(void);
char uart_recv_nb(void);
char uart_recv(void);

//UART fucntion to recieve data

char uart_recv(void);


// initialize UART driver
void uart_init(void);

void uart_printf(const char *fmt, ...);  //formatted print function for uart (like printf in stdio.h)
