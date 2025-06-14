// src/boot.s

#include "mm.h"
#include "sysregs.h"

//this assembly file is responsible for booting the kernel, runs in 64 bit mode.

.section .text
.global _start

//setting this stuff so that linker doesnt freak out
.extern __bss_start
.extern __bss_end


_start:
    //boot code starts here, yayyyy (sorry lol i got too excited)
    mrs x0, mpidr_el1       // get the MPIDR register value from the cpu, to know whcih core are we talking to
    and x0, x0, #0xFF       // mask to get the cpu id by usng AND bitwise operator, and store it in the register x0
    cbz x0, master          // if this thing is 0, this is the master cpu, jump to master, and if its not then it just hangs the cpu
    b proc_hang


master:
    ldr x0, =SCTLR_VALUE_MMU_DISABLED
    msr sctlr_el1, x0

    ldr x0, =HCR_VALUE
    msr hcr_el2, x0

    ldr x0, =SCR_VALUE 
    msr scr_el3, x0

    ldr x0, =SPSR_VALUE
    msr spsr_el3, x0

    adr x0, el1_entry
    msr elr_el3, x0

    eret

el1_entry:
    adr x0, __bss_start     // load the address of the bss section
    adr x1, __bss_end       // load the address of the end of the bss section
    sub x1, x1, x0          // calculate the size of the bss section
    bl memzero              // zeroes out the bss section

    ldr x1, =LOW_MEMORY
    mov sp, x1              // just did that round process cuz the stupid compiler doesnt understand #LOW_MEMORY even though i mentioned it in the header.
    bl kernel_main          // intiialize the kernel code
    b proc_hang             // just holds the cpu

proc_hang:
    wfe                     // wait for event, kinda puts the CPU in a low power state until an interrupt occurs
    b proc_hang             // loop forever (currently for testing purposes)
    

.size _start, .-_start

