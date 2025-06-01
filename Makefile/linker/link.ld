ENTRY(_start)

SECTIONS {
    . = 0x8000; /*The starting address where Pi starts executing kernel from, this address is located in the RAM */

    .text : {
        *(.text)
    }

    .data :{
        *(.data)
    }

    .bss :{
        *(.bss)
        *(COMMON)
    }
}

