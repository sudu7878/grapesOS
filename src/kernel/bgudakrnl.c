// src/kernel/bgudakrnl.c

//This is the Baguda Kernel source

    //kernel headers
        #include "kernel/bgudakrnl.h"
        #include "kernel/irq_hedr.h"
        #include "kernel/cacheF.h"
        #include "kernel/irq.h"
        
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
        uart_send_string("4.    GetTemp - Get the current temperature of the CPU core.\n");
        uart_send_string("5.    GetBoardRevision - Get the board's revision number.\n");
        uart_send_string("6.    VdoInit - Initialize Video drivers.\n");
        uart_send_string("7.    0x100 - Test mailbox.\n");
        get_prompt();
    }

    //Clear the terminal screen
    else if (strcmp(cmd, "clear") == 0){
        uart_send_string("\033[2J\033[H"); //ANSI escape code to clear the terminal screen and move cursor to home position
        get_prompt();
    }

    else if (strcmp(cmd, "0x100") == 0){
        unsigned int msg = 0x100;
        int result = mbx_write(8, msg);

        if (result == 0){
            uart_printf("Mailbox write success.\n");
        } else{
            uart_printf("Mailbox write failed!\n");
        }
        unsigned int response = mbx_read(8);
        if (response == 0xFFFFFFFF){
            uart_printf(ANSI_RED"Fatal error: Invalid mail response!\n" ANSI_RESET);
        }
        uart_send_hex(response);
        uart_send_string("\n");
        get_prompt();

    }

    else if (strcmp(cmd, "GetTemp") == 0){

        if(mbx_multi_request(tags, 2) == 0) {
            uart_printf("Temp: %dC\n", tags[0].data[1]/1000);
        }
        get_prompt();
    }

    
    //Get Core Temperature
    else if (strcmp(cmd, "GetBoardRevision") == 0){
         if(mbx_multi_request(tags, 2) == 0) {
            uart_printf("Board Revision no: %x\n", tags[1].data[0]);
        }
        get_prompt();
    }

    else if (strcmp(cmd, "VdoInit") == 0){
        if (!video_inited) {
            video_init();
         } else {
            uart_printf(ANSI_YELLOW"Video driver already initialied! Re-using pre-allocated one!\n" ANSI_RESET);
         }

         if (!video_info.virtual_address){
            uart_printf(ANSI_RED"Video initialization failed earlier; not drawing bruh.\n"ANSI_RESET);
            get_prompt();
            return;
         }

         draw_string(50, 50, "Oiii Hello from grapesOS!", 0x00FFFFFF);
         term_puts("Hello from grapesOS terminal!\nLine 2 here test.");
         draw_Logo(50, 50);

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
    uart_send_string(ANSI_GREEN "Hello from grapesOS! Running on UART.\n" ANSI_RESET);
    uart_send_string(ANSI_GREEN "System is now running in 64 bit mode.\n" ANSI_RESET);
    uart_send_string(ANSI_GREEN "Type 'help' for available commands.\n" ANSI_RESET);

    

    char buffer[100];
    int idx = 0;


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
        
        //set up basic command processor for UART
          //print the prompt so that user can enter commands
        char c = uart_recv();
        uart_send(c);  //echo the character back

        if (c == '\r' || c == '\n') {       //detect enter key press (as UART returns these characters when enter is pressed)
            buffer[idx] = '\0';             //null-terminate the string so that C doesnt spend time reading the string infinitely
            process_command(buffer);        //return the command to the process_command function so that we can generate the output
            //uart_printf("DEBUG: process_command() entered with cmd=%s\n", buffer);
            idx = 0;                        //reset the character index for the next commadn input!
        }
        else if (idx < sizeof(buffer) -1){      //this detects if we pressed enter key or not, if not, then we store the character into the buffer!
            buffer[idx++] = c;
        }
    }
}