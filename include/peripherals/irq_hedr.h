//peripherals/irq_hedr.h

/* Different raspberry pi versions had a slightly different irq register nomenclature, so we created two structs for each version thatr grapesOS 
    will support. Additionally, these things here are just the irq registers that control the IRQ response, Baguda Kernel calls them.*/

#pragma once


#include "peripherals/base.h"
#include "common.h"

enum vc_irqs {
    AUX_IRQ = (1 << 29)
};

struct arm_irq_regs_2711 {          //irq regitster struct for raspberry pi 4
    reg32 irq0_pending0;
    reg32 irq0_pending1;
    reg32 irq0_pending2;

    reg32 res0;

    reg32 irq0_enable0;
    reg32 irq0_enable1;
    reg32 irq0_enable2;

    reg32 res1;

    reg32 irq0_disable0;
    reg32 irq0_disable1;
    reg32 irq0_disable2;
};

struct arm_irq_regs_2837{           //irq regitster struct for raspberry pi 3
    reg32 irq0_pending0;
    reg32 irq0_pending1;
    reg32 irq0_pending2;

    reg32 fiq_control;

    reg32 irq0_enable1;
    reg32 irq0_enable2;
    reg32 irq0_enable0;

    reg32 res;

    reg32 irq0_disable1;
    reg32 irq0_disable2;
    reg32 irq0_disable0;
};

#if RPI_VERSION == 3
    typedef struct arm_irq_regs_2837 arm_irq_regs;
#endif

#if RPI_VERSION == 4
    typedef struct arm_irq_regs_2711 arm_irq_regs;
#endif

#define REGS_IRQ ((arm_irq_regs *)(PBASE + 0x0000B200))

