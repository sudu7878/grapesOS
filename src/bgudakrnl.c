// src/bgudakrnl.c

//This is the Baguda Kernel source

    //kernel headers
        #include "bgudakrnl.h"
        #include "gpio.h"
        #include "printf.h"
        #include "utils.h"
        #include "mailbox.h"
        #include "mem.h"
        #include "irq.h"
    
    //peripheral headers
        #include "peripherals/uart.h"
        #include "peripherals/gpio_hedr.h"
        #include "peripherals/timer.h"
        #include "peripherals/irq_hedr.h"

    //standard library headers
        #include <stdint.h>
        #include <stddef.h>


uint64_t get_el(void);  //gets the exception level of the CPU, defined in utils.h

//mini strcmp function to compare two strings
int strcmp(const char *s1, const char *s2){
    while (*s1 && *s2 && *s1 == *s2){
        s1++;
        s2++;
    }
    return (unsigned char)*s1 - (unsigned char)*s2;
}

//function to get the prompt from the user
void get_prompt(){
    uart_send_string(ANSI_GREEN "grapesOS> " ANSI_RESET);
}

// basic command processor
void process_command(const char *cmd) {

    //Get exception Level
    if (strcmp(cmd, "get_el") == 0){
        uart_send_string(ANSI_CYAN "Current EL: " ANSI_RESET);
        uart_send(get_el() + '0');  //convert the exception level to char(ASCII) and send it
        uart_send_string("\n");
        get_prompt();
    }

    //Get Help 
    else if (strcmp(cmd, "help") == 0){
        uart_send_string("Available commands:\n");
        uart_send_string("1.    get_el - Get the current exception level in ARM CPU.\n");
        uart_send_string("2.    help - Show this help message.\n");
        uart_send_string("3.    clear - Clear the terminal screen.\n");
        uart_send_string("4.    getcoretemp - Get the current temperature of the CPU core.\n");
        uart_send_string("5.    i2cpowerste - Gets the I2C interface power state.\n");
        get_prompt();

    }

    //Clear the terminal screen
    else if (strcmp(cmd, "clear") == 0){
        uart_send_string("\033[2J\033[H"); //ANSI escape code to clear the terminal screen and move cursor to home position
        get_prompt();
    }

    //Get Core Temperature
    else if (strcmp(cmd, "getcoretemp") == 0){
        u32 cur_temp = 0;
        mailbox_generic_command(RPI_FIRMWARE_GET_TEMPERATURE, 0, &cur_temp);
        uart_printf(ANSI_CYAN "Current temperature: %dC\n", cur_temp / 1000, ANSI_RESET);
        get_prompt();
    }

    else if (strcmp(cmd, "i2cpowerstate") == 0){
        bool on = mailbox_power_check(2); //2 is the power domain for I2C
        uart_printf(ANSI_CYAN "I2C Power state is %s\n", on ? "ON" : "OFF", ANSI_RESET);
        get_prompt();
    }

   //Unknown command handler
    else{
        uart_send_string(ANSI_RED "Unknown command!\n" ANSI_RESET);
        get_prompt();
    }
}

//MAIN KERNEL FUNCTION!
void kernel_main() {

    //initialize UART
    uart_init();
    uart_send_string(ANSI_GREEN "Hello from grapesOS!\n" ANSI_RESET);
    uart_send_string(ANSI_GREEN "System is now running in 64 bit mode.\n" ANSI_RESET);
    uart_send_string(ANSI_GREEN "Type 'help' for available commands.\n" ANSI_RESET);

    uart_send_string(ANSI_RED "All good till here\n" ANSI_RESET);

    char buffer[100];
    int idx = 0;

    uart_send_string(ANSI_RED "All good till here two!\n" ANSI_RESET);

     //check CPU temp and issue return a warning if too high
    /*
    u32 max_temp = 0;
    mailbox_generic_command(RPI_FIRMWARE_GET_MAX_TEMPERATURE, 0, &max_temp);
    u32 cur_temp = 0;
    mailbox_generic_command(RPI_FIRMWARE_GET_TEMPERATURE, 0, &cur_temp);
    if (cur_temp > max_temp){
        uart_printf(ANSI_RED "WARNING! Your CPU is overheating; Please shutdown your device and let it cool, or attach a CPU cooler!\n" ANSI_RESET);
    }
    uart_send_string(ANSI_RED "All good till here three!\n" ANSI_RESET);
    */
   

  


get_prompt();
    //MAIN KERNEL LOOP CODE
    while(1){
        
        //below is the logic for checking errors from mailbox and preventing CPU halting
        if (mailbox_read(MAIL_POLL) == 0xFFFFFFFF){
            uart_send_string(ANSI_RED " Fatal error: Mailbox crashed! Reason: Invalid poll response. The thread crashed with error code: %x\r\n", 0xFFFFFFFF);
            continue;   //skip the rest of the loop and continue to the next iteration
        }

        //set up basic command processor for UART
          //print the prompt so that user can enter commands
        char c = uart_recv();
        uart_send(c);  //echo the character back

        if (c == '\r' || c == '\n') {       //detect enter key press (as UART returns these characters when enter is pressed)
            buffer[idx] = '\0';             //null-terminate the string so that C doesnt spend time reading the string infinitely
            process_command(buffer);        //return the command to the process_command function so that we can generate the output
            idx = 0;                        //reset the character index for the next commadn input!
        }
        else if (idx < sizeof(buffer) -1){      //this detects if we pressed enter key or not, if not, then we store the character into the buffer!
            buffer[idx++] = c;
        }
    }
}