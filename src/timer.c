#include "printf.h"
#include "peripherals/timer.h"
#include "peripherals/irq_hedr.h"
#include "peripherals/auxillary.h"

const u32 interval_1 = CLOCKHZ;
u32 cur_val_1 = 0;

void timer_init() {
    cur_val_1 = REGS_TIMER->counter_lo;
    cur_val_1 += interval_1;
    REGS_TIMER->compare[1] = cur_val_1;
}