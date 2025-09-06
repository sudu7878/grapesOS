// include/drivers/timer.h

#pragma once

//this thing here lets me use the system crystal clock to count the time for various stuff throughout the system

#include "drivers/base.h"
#include "common.h"

#define CLOCKHZ 1000000

struct timer_regs {         //the timer registers
    reg32 control_status;       //control register
    reg32 counter_lo;           //counter low register
    reg32 counter_hi;           //counter high register
    reg32 compare[4];           //compare registers (there are 4 of them 0-3)
};



#define REGS_TIMER ((struct timer_regs *)(PBASE + 0x00003000))
