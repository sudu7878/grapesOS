ENTRY(_start)

SECTIONS {
    . = 0x40000000;

    .boot . : {
        *(.text)
    }

    .text : {
        *(.text)
    }

    .rodata : {
        *(.rodata)
    }

    .data : {
        *(.data)
    }

    .bss_begin = .;
    .bss_end = .;

    .bss : {
        __bss_start = .;
        *(.bss .bss.* COMMON)
        __bss_end = .;
    }

    . = ALIGN(0x8);

    
    . = . + 0x1000;  
    stack_top = .;
}
