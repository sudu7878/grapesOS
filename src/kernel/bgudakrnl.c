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


//Getting mailbox tags:
mbx_tag_t tags[2] ={
    /*0.*/{RPI_FIRMWARE_GET_TEMPERATURE, 8, 0, {0, 0} },
    /*1.*/{RPI_FIRMWARE_GET_BOARD_REVISION, 4, 0, {0}}
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

        printf("Available commands: \n");
        printf("1.    get_el - Get the current exception level in ARM CPU.\n");
        printf("2.    help - Show this help message.\n");
        printf("3.    clear - Clear the terminal screen.\n");
        printf("4.    GetTemp - Get the current temperature of the CPU core.");
        printf("5.    GetBoardRevision - Get the board's revision number.\n");
        printf("6.    0x100 - Test mailbox.\n");
        printf("7.    StartVdoEcho - Starting echoing characters from UART terminal to the screen. Required VdoInit first.\n");
        printf("8.    DrawLogo - Draw grapesOS logo on the screen. Required VdoInit.\n");
        printf("9.    ShutDown - Stops all kernel processes and puts system in idle mode.\n");

        
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
            printf("[KERNEL]: Mailbox write success.\n");
        } else{
            uart_printf(ANSI_RED "[KERNEL]: Mailbox write failed!\n" ANSI_RESET);
            printf(TXT_RED"[KERNEL]: Mailbox write failed.\n"TXT_REST);
        }
        unsigned int response = mbx_read(8);
        if (response == 0xFFFFFFFF){
            uart_printf(ANSI_RED"[KERNEL]: Fatal error: Invalid mail response!\n" ANSI_RESET);
            printf(TXT_RED"[KERNEL]: Fatal error: Invalid mail response!\n"TXT_REST);
        }
        uart_send_hex(response);
        uart_send_string("\n");
        

    }

    else if (strcmp(cmd, "GetTemp") == 0){

        if(mbx_multi_request(tags, 2) == 0) {
            uart_printf("Temp: %dC\n", tags[0].data[1]/1000);
            printf("Temp: %dC\n", tags[0].data[1]/1000);
        }
       
    }

    
    //Get Core Temperature
    else if (strcmp(cmd, "GetBoardRevision") == 0){
         if(mbx_multi_request(tags, 2) == 0) {
            uart_printf("Board Revision no: %x\n", tags[1].data[0]);
            printf("Board Revision no: %x\n", tags[1].data[0]);
        }
       
    }


    else if (strcmp(cmd, "DrawLogo") == 0){
        if(!video_inited){
            uart_printf(ANSI_RED"[KERNEL]: Cannot draw; video driver not intialized! Run 'VdoInit' first!\n"ANSI_RESET);
            
        } else{
            draw_Logo(100, 100);
        }
        ;
    }

    else if (strcmp(cmd, "EndSession") == 0){
        uart_printf(ANSI_YELLOW"[KERNEL]: Ending grapesOS session. Just pull the plug bru.\n");
        printf(TXT_RED"[KERNEL]: Ending grapesOS session.\n"TXT_REST);
        while(1);
    }

    else if(strcmp(cmd, "Shutdown") == 0){
        uart_printf(ANSI_YELLOW"[KERNEL]: Shutting down all processes and putting system in idle mode. Just reset the device to restart.\n" ANSI_RESET);
        printf(TXT_RED"[KERNEL]: Ending grapesOS session.\n"TXT_REST);
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
            printf(TXT_RED"Print to screen started. Press 'ESC' key to exit and return to kernel.\n"TXT_REST);
            while(1){
                char c = uart_recv();
                if (c == 27){   //ASCII value of ESC key is 27
                    uart_printf("Exiting echo mode and returning to kernel.\n");
                    printf("Exiting echo mode and returning to kernel.\n");
                    break;
                }
                printf("%c", c);
            }
            
           
        }
    }


   //Unknown command handler
    else{
        uart_send_string(ANSI_RED "[KERNEL]:Unknown command!\n" ANSI_RESET);
        printf(TXT_RED"Unknown command!\n"TXT_REST);
        
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
        
        // continue using UART-only input loop below
    } else {
        // Initialize and clear on-screen terminal
        terminal_init();
        terminal_clr();

        // optional: draw logo on screen
        draw_Logo(100, 100);
    }

    // keep serial logs
    uart_send_string(ANSI_GREEN "Hello from grapesOS! Running on UART.\n" ANSI_RESET);
    uart_send_string(ANSI_GREEN "System is now running in 64 bit mode.\n" ANSI_RESET);
    uart_send_string(ANSI_GREEN "Type 'help' for available commands.\n" ANSI_RESET);

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