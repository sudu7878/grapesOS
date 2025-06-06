// include/mm.h


/* just mentioning the memory management header here, so that it can be included in assembly files, or else it just freakes out when
    doesnt understand this shit
*/

#pragma once

#define PAGE_SHIFT 12   // 12 bits for page offset, so like 4KB pages since 2^12 = 4096 bytes (page size)
#define TABLE_SHIFT 9   // 9 bits for table index, so like 512 entries per page table (2^9 = 512)
#define SEGMENT_SHIFT (PAGE_SHIFT + TABLE_SHIFT)
#define PAGE_SIZE (1 << PAGE_SHIFT)
#define SECTION_SIZE (1 << SEGMENT_SHIFT)


#define LOW_MEMORY (2 * SECTION_SIZE)

#ifndef __ASSEMBLER__

void memzero(unsigned long src, unsigned int n);

#endif
