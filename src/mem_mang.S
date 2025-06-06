//src/mem_mang.s

// an assembly loop that zeroes out the bss section of the bss memory addresses, so making sure it doesnt contain garbage values

.section .text
.global memzero
memzero:
    str xzr, [x0], #8   //store zero at register x0 and increments it by 8 bytes
    subs x1, x1, #8     //decrement x1 register by 8 bytes
    b.gt memzero        //if x1 is greater than 0, then just branch to memzero again, to form that closed loop, until it reads 0.
    ret

