// src/uart.c

//this stuff is for the uart driver, and we would be mostly using it for debugging purposes.


#define UART0DR ((volatile unsigned int *)0x101f1000) // UART0 Data Register in ARM architecture (raspberry pi)


//main uart modular functions


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

    if (num > 0) {
        buffer[i++] = (num % 10) + '0';     //getting the last digit as the character bruh
        num /= 10;
    }

    while (i--){
        uart_send_char(buffer[i]);
    }

}