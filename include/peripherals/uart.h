// include/peripherals/uart.h

//*Mini UART is a inteface in pi that is the derivation of the main UART or PL011 UART. This thing is used by bluetooth module by default, plus its much more advanced and difficult to set up, so I used mini uart instead.


#pragma once

#include "gpio.h"
#include "peripherals/gpio_hedr.h"
#include "common.h"
#include "peripherals/auxillary.h"

#define UART0DR ((volatile unsigned int *)0x9000000) // UART0 Data Register in ARM architecture (raspberry pi)

//defining nthe TX and RX pins

#define TXD 14  //set the TX UART pin 
#define RXD 15  //set the RX UART pin

//(also i defined it here because it just makes it so much easier to use it everywhere around the proejct)

//a way to debug stuff for the kernel using the UART interface


void uart_send(char c);                     //thsi thing here returns the characters

void uart_send_string(char *str);     //this thing here returns strings, USING the characters

void uart_send_int(int num);                //this thing here returns integers



//UART fucntion to recieve data

char uart_recv(void);


// initialize UART driver
void uart_init(void);
   
