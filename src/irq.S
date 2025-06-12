// src/irq.S

// This thing here is responsible for loading exception vector table in the CPU, and also enabling and disabling

#include "entry.h"

.globl irq_init_vectors

irq_init_vectors:
    adr x0, vectors
    msr vbar_el1, x0 
    ret

.globl irq_enable
irq_enable:
    msr daifclr, #2     //clear the daif register, so we can recieve the interrupts again. (unmasking)
    ret

.globl irq_disable
irq_disable:
    msr daifset, #2     //set the daif register, we disable the IRQs (masking)
    ret
