.section .text
.global _start

_start:
    //boot code starts here, yayyyy (sorry lol i got too excited)

    bl kernel_main
    b . // make the kernel loop forever (for testing purposes)

.size _start, .-_start

