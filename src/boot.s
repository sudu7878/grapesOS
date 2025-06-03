// src/boot.s

.section .text
.global _start

_start:
    //boot code starts here, yayyyy (sorry lol i got too excited)
    LDR sp, =stack_top
    bl kernel_main
    b . // make the kernel loop forever (for testing purposes, temporary)

.size _start, .-_start

