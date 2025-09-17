//  src/kernel/cacheF.c

//Helps in flushing out CPU registers to the main memory, particularly written for mailbox.c
#include "kernel/cacheF.h"


void clean_data_cache_for_address(void *addr, u32 size){
    uintptr_t start_addr = ((uintptr_t)addr) & ~(CACHE_LINE_SIZE -1);
    uintptr_t end_addr = ((uintptr_t)addr + size + CACHE_LINE_SIZE - 1) & ~(CACHE_LINE_SIZE - 1);
    while (start_addr < end_addr){
        __asm__ __volatile__("dc civac, %0" :: "r"(start_addr));
        start_addr += CACHE_LINE_SIZE;
    }
    __asm__ __volatile__("dsb sy");
    __asm__ __volatile__("isb sy");
}

void invalidate_data_cache_for_address(void* addr, u32 size) {
    uintptr_t start = (uintptr_t)addr;
    uintptr_t end = start + size;

    // Align to 64-byte cache line size
    start &= ~(64 - 1);

    for (uintptr_t line = start; line < end; line += 64) {
        __asm__ volatile ("dc ivac, %0" :: "r"(line) : "memory");
    }

    __asm__ volatile ("dsb sy");  // Ensure completion
    __asm__ volatile ("isb");
}