//src/utils.s

/*this thing here helps me to read/write 32 bit values from/to memory addresses, so that i can use gpios and stuff, it also contains a delay
function*/

.section .text
.global delay
delay:
    subs x0, x0, #1
    bne delay
    ret

.global put_32
put_32:
    str w0, [x0]    // store the value in w0 at the address in x0
    ret

.global get_32
get_32:
    ldr w0, [x0]    // load that saved address from x0 into w0
    ret

.global get_el
get_el:
    mrs x0, CurrentEL
    lsr x0, x0, #2
    ret