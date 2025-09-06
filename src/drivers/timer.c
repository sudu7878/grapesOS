//include/drivers/timer.h

#include "printf.h"
#include "drivers/timer.h"
#include "drivers/gpio_hedr.h"
#include "drivers/auxillary.h"

const u32 interval_1 = CLOCKHZ;
u32 cur_val_1 = 0;

void timer_init() {
    cur_val_1 = REGS_TIMER->counter_lo;
    cur_val_1 += interval_1;
    REGS_TIMER->compare[1] = cur_val_1;
}