// src/kernel/bgudakrnl.c

//This is the Baguda Kernel source

    //kernel headers
        #include "kernel/bgudakrnl.h"
        #include "kernel/irq_hedr.h"
        #include "kernel/cacheF.h"
        #include "kernel/irq.h"
        #include "kernel/terminal.h"
        
    //driver headers
        #include "drivers/gpio.h"
        #include "drivers/mailbox.h"
        #include "drivers/uart.h"
        #include "drivers/gpio_hedr.h"
        #include "drivers/timer.h"
        #include "drivers/video_driver.h"
    
    //lib headers
        #include "lib/printf.h"
        #include "lib/utils.h"

    //dislpay headers
      #include "display/text_rend.h"

    //standard library headers
        #include <stdint.h>
        #include <stddef.h>


uint64_t get_el(void);  //gets the exception level of the CPU, defined in utils.h

#define TAG_GET_CLOCK_RATE 0x00030002
#define CLOCK_ID_ARM    0



//Getting mailbox tags:
mbx_tag_t tags[4] ={
    /*0.*/{RPI_FIRMWARE_GET_TEMPERATURE, 8, 0, {0, 0} },
    /*1.*/{RPI_FIRMWARE_GET_BOARD_REVISION, 4, 0, {0}},
    /*2.*/{RPI_FIRMWARE_GET_CLOCK_RATE, 8, 0, {CLOCK_ID_ARM, 0}},
    /*3.*/{RPI_FIRMWARE_GET_ARM_MEMORY, 8, 0, {0, 0}}
};

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
    uart_send_string(ANSI_GREEN "\ngrapesOS> " ANSI_RESET);
    if (video_inited && video_info.virtual_address){
        terminal_puts("\n");
        terminal_puts(TXT_GREEN "\ngrapesOS> " TXT_REST);
    }
}


// basic command processor
void process_command(const char *cmd) {

    //Get exception Level
    if (strcmp(cmd, "get_el") == 0){
        uart_send_string(ANSI_CYAN "Current EL: " ANSI_RESET);
        uart_send(get_el() + '0');  //convert the exception level to char(ASCII) and send it
        uart_send_string("\n");
        
        terminal_puts("\n");
        printf("%d", get_el());
        
    }

    //Get Help 
    else if (strcmp(cmd, "help") == 0){
        uart_send_string("Available commands:\n");
        uart_send_string("1.    get_el - Get the current exception level in ARM CPU.\n");
        uart_send_string("2.    help - Show this help message.\n");
        uart_send_string("3.    clear - Clear the terminal screen.\n");
        uart_send_string("4.    GetTemp - Get the current temperature of the CPU core.\n");
        uart_send_string("5.    GetBoardRevision - Get the board's revision number.\n");
        uart_send_string("6.    0x100 - Test mailbox.\n");
        uart_send_string("7.    StartVdoEcho - Starting echoing characters from UART terminal to the screen. Required VdoInit first.\n");
        uart_send_string("8.    DrawLogo - Draw grapesOS logo on the screen. Required VdoInit.\n");
        uart_send_string("9.    ShutDown - Stops all kernel processes and puts system in idle mode.\n");

        terminal_puts("\n");
        terminal_puts(TXT_WHITE"Available commands: \n");
        terminal_puts(TXT_WHITE"1.    get_el - Get the current exception level in ARM CPU.\n");
        terminal_puts(TXT_WHITE"2.    help - Show this help message.\n");
        terminal_puts(TXT_WHITE"3.    clear - Clear the terminal screen.\n");
        terminal_puts(TXT_WHITE"4.    GetTemp - Get the current temperature of the CPU core.\n");
        terminal_puts(TXT_WHITE"5.    GetBoardRevision - Get the board's revision number.\n");
        terminal_puts(TXT_WHITE"6.    0x100 - Test mailbox.\n");
        terminal_puts(TXT_WHITE"7.    StartVdoEcho - Starting echoing characters from UART terminal to the screen. Required VdoInit first.\n");
        terminal_puts(TXT_WHITE"8.    DrawLogo - Draw grapesOS logo on the screen. Required VdoInit.\n");
        terminal_puts(TXT_WHITE"9.    ShutDown - Stops all kernel processes and puts system in idle mode.\n");

        
    }

    //Clear the terminal screen
    else if (strcmp(cmd, "clear") == 0){
        uart_send_string("\033[2J\033[H"); //ANSI escape code to clear the terminal screen and move cursor to home position
        terminal_clr();
        
    }

    else if (strcmp(cmd, "0x100") == 0){
        unsigned int msg = 0x100;
        int result = mbx_write(8, msg);

        if (result == 0){
            uart_printf("[KERNEL]: Mailbox write success.\n");
            terminal_puts("\n");
            printf("[KERNEL]: Mailbox write success.\n");
        } else{
            uart_printf(ANSI_RED "[KERNEL]: Mailbox write failed!\n" ANSI_RESET);
            term_puts("\n");
            printf(TXT_RED"[KERNEL]: Mailbox write failed.\n"TXT_REST);
        }
        unsigned int response = mbx_read(8);
        if (response == 0xFFFFFFFF){
            uart_printf(ANSI_RED"[KERNEL]: Fatal error: Invalid mail response!\n" ANSI_RESET);
            terminal_puts("\n");
            printf(TXT_RED"[KERNEL]: Fatal error: Invalid mail response!\n"TXT_REST);
        }
        uart_send_hex(response);
        printf("%x", response);
        uart_send_string("\n");
        

    }

    else if (strcmp(cmd, "GetTemp") == 0){

        if(mbx_multi_request(tags, 2) == 0) {
            uart_printf("Temp: %dC\n", tags[0].data[1]/1000);
            terminal_puts("\n");
            printf("Temp: %dC\n", tags[0].data[1]/1000);
        }
       
    }

    
    //Get Core Temperature
    else if (strcmp(cmd, "GetBoardRevision") == 0){
         if(mbx_multi_request(tags, 2) == 0) {
            uart_printf("Board Revision no: %x\n", tags[1].data[0]);
            term_puts("\n");
            printf("Board Revision no: %x\n", tags[1].data[0]);
        }
       
    }

    else if (strcmp(cmd, "EndSession") == 0){
        uart_printf(ANSI_YELLOW"[KERNEL]: Ending grapesOS session. Just pull the plug bru.\n");
        printf(TXT_RED"[KERNEL]: Ending grapesOS session.\n"TXT_REST);
        while(1);
    }

    else if(strcmp(cmd, "Shutdown") == 0){
        uart_printf(ANSI_YELLOW"[KERNEL]: Shutting down all processes and putting system in idle mode. Just reset the device to restart.\n" ANSI_RESET);
        printf(TXT_RED"[KERNEL]: Ending grapesOS session: Interrupts have been disabled. It's now safe to power off the machine.\n"TXT_REST);
        //disable interrupts
        disable_irq();
        
        while(1){
            asm volatile("wfi");  
        }
    }

    else if (strcmp(cmd, "StartVdoEcho") == 0){
        if(!video_inited){
            uart_printf(ANSI_RED"[KERNEL]: Cannot echo; video driver not intialized! Run 'VdoInit' first!\n"ANSI_RESET);
          
        } else{
            uart_printf("Print to screen started. Press 'ESC' key to exit and return to kernel.\n");
            terminal_puts("\n");
            printf(TXT_YELLOW"Print to screen started. Press 'ESC' key to exit and return to kernel.\n"TXT_REST);
            while(1){
                char c = uart_recv();
                if (c == 27){   //ASCII value of ESC key is 27
                    uart_printf("Exiting echo mode and returning to kernel.\n");
                    terminal_puts("\n");
                    printf(TXT_YELLOW"Exiting echo mode and returning to kernel.\n");
                    break;
                }
                printf("%c", c);
            }
            
           
        }
    }


   //Unknown command handler
    else{
        uart_send_string(ANSI_RED "[KERNEL]:Unknown command!\n" ANSI_RESET);
        terminal_puts("\n");
        printf(TXT_RED"[KERNEL]: Unknown command!\n"TXT_REST);
        
    }
        
}

//MAIN KERNEL FUNCTION!
void kernel_main() {

    // initialize UART
    uart_init();

    // initialize video if needed
    if (!video_inited) {
        video_init();
    } else {
        uart_printf(ANSI_YELLOW "[KERNEL]: Video driver already initialied! Re-using pre-allocated one!\n" ANSI_RESET);
    }

    // ensure framebuffer is valid
    if (!video_info.virtual_address) {
        uart_printf(ANSI_RED "[KERNEL]: Video initialization failed earlier! Falling back to UART-only.\n" ANSI_RESET);
        
        
    } else {
        
        terminal_init();
        terminal_clr();

       
        draw_Logo(500, 500);
        delay(10000);
    }

    // keep serial logs
    uart_send_string(ANSI_GREEN "Hello from grapesOS! Running on UART.\n" ANSI_RESET);
    uart_send_string(ANSI_GREEN "System is now running in 64 bit mode.\n" ANSI_RESET);
    uart_send_string(ANSI_GREEN "Type 'help' for available commands.\n" ANSI_RESET);

    terminal_puts(TXT_WHITE "[KERNEL]: Bootloader was initialized correctly.\n");
    terminal_puts(TXT_WHITE "[KERNEL]: System is now ready for interrupts.\n");

    /*if(mbx_multi_request(tags, 4) == 0) {
            uart_printf("[KERNEL]: Clock rate: %dHz\n", tags[2].data[1]);
            terminal_puts("\n");
            printf("[KERNEL]: Clock rate: %dHz\n", tags[2].data[1]);
            terminal_puts("\n");
    }*/

    if(mbx_multi_request(tags, 4) == 0) {
            uart_printf("[KERNEL]: Total Memory detected: %d Bytes\n", tags[3].data[1]);
            terminal_puts("\n");
            printf("[KERNEL]: Total Memory detected: %d Bytes\n", tags[3].data[1]);
            terminal_puts("\n");
    }

    terminal_puts(TXT_GREEN "grapesOS Ver.: 0.0.3\n");
    terminal_puts(TXT_GREEN "Welcome to grapesOS temrinal interface! Have a great day!\n");
    terminal_puts(TXT_GREEN "Type the commands below or type 'help' for available commands.\n");

    // show prompt on both UART and display
    get_prompt();

    // simple input loop - reads from UART and echoes to display
    char buffer[100];
    int idx = 0;


    while (1) {
        if (!uart_data_ready()){
            continue;
        }

        char c = uart_recv_nb();
        uart_send(c);
        // handle Enter
        if (c == '\r' || c == '\n') {
            // show newline on display once
            terminal_putc('\n');

            buffer[idx] = '\0';
            if (idx > 0) {
                process_command(buffer);
            }
            idx = 0;
            get_prompt();            // reprint prompt after command handled
            continue;
        }

        // backspace
        if (c == '\b' || (unsigned char)c == 127) {
            if (idx > 0) {
                idx--;
                terminal_putc('\b'); // terminal clears the glyph cell
            }
            continue;
        }

        // printable characters
        if ((unsigned char)c >= 32 && idx < (int)sizeof(buffer) - 1) {
            buffer[idx++] = c;
            terminal_putc(c);       // echo typed char to screen
        }
    }
}