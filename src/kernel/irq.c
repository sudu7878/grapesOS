// src/kernel/irq.c

// This thing here handles interrupts request in the kernel

#undef printf   //honestly, i have NO idea why my linker is screaming that ive defined it somewhere so imma just do this💀

#include "lib/utils.h"
#include "kernel/entry.h"
#include "drivers/gpio_hedr.h"
#include "drivers/auxillary.h"
#include "lib/printf.h"
#include "kernel/irq.h"
#include "drivers/uart.h"
#include "kernel/irq_hedr.h"


const char entry_error_messages[16][32] = {
	"SYNC_INVALID_EL1t",
	"IRQ_INVALID_EL1t",		
	"FIQ_INVALID_EL1t",		
	"ERROR_INVALID_EL1T",		

	"SYNC_INVALID_EL1h",
	"IRQ_INVALID_EL1h",		
	"FIQ_INVALID_EL1h",		
	"ERROR_INVALID_EL1h",		

	"SYNC_INVALID_EL0_64",		
	"IRQ_INVALID_EL0_64",		
	"FIQ_INVALID_EL0_64",		
	"ERROR_INVALID_EL0_64",	

	"SYNC_INVALID_EL0_32",		
	"IRQ_INVALID_EL0_32",		
	"FIQ_INVALID_EL0_32",		
	"ERROR_INVALID_EL0_32"	
};


void show_invalid_entry_message(u32 type, u64 esr, u64 address){
    uart_printf(ANSI_RED "[IRQ_HANDLER]: ERROR CAUGHT: %s - %d, ESR: %x, Address: %x\n",
        entry_error_messages[type], type, esr, address);
}


void enable_interrupt_controller(){
    #if RPI_VERSION == 4
        REGS_IRQ->irq0_enable0 = AUX_IRQ;
    #endif

      #if RPI_VERSION == 3
        REGS_IRQ->irq0_enable1 = AUX_IRQ;
    #endif
}

void disable_irq(){
    #if RPI_VERSION == 4
        REGS_IRQ->irq0_disable0 = AUX_IRQ;
    #endif

      #if RPI_VERSION == 3
        REGS_IRQ->irq0_disable1 = AUX_IRQ;
    #endif
}

void handle_irq(){
    u32 irq;

     #if RPI_VERSION == 4
        irq = REGS_IRQ->irq0_pending0;
    #endif

      #if RPI_VERSION == 3
        irq = REGS_IRQ->irq0_pending1;
    #endif

    while(irq) {
        if (irq & AUX_IRQ) {
            irq &= ~AUX_IRQ;
            while((REGS_AUX->mu_iir & 4) == 4){
                uart_printf("UART Recieved: ");
                uart_send(uart_recv());
                uart_printf("\n");
            }

            while (REGS_AUX->mu_lsr & 0x01){
                char ch = (char)(REGS_AUX->mu_io & 0xFF);
                uart_push_rx(ch);
                while(!(REGS_AUX->mu_lsr & 0x20)){}
                REGS_AUX->mu_io = (u32)ch;
            }
        }
    }
}