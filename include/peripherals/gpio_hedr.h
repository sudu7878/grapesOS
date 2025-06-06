// include/gpio_hedr.h

//just addressing some GPIO registers in CPU, so we can control them ahhh!

#pragma once
#include "base.h"
#include "common.h"

//intialize GPIO pin data so the code knows what we're accessing.
struct GpioPinData{
    reg32 reseved;
    reg32 data[2];
};


//link those induvidual GpioPinData structs to the GPIO registers, so we can access them easily.
struct GpioRegs{
    reg32 func_select[6];
    struct GpioPinData output_set;
    struct GpioPinData output_clear;
    struct GpioPinData level;
    struct GpioPinData rising_edge_detect_enable;
    struct GpioPinData falling_edge_detect_enable;
    struct GpioPinData high_detect_enable;
    struct GpioPinData low_detect_enable;
    struct GpioPinData async_rising_edge_detect_enable;
    struct GpioPinData async_falling_edge_detect_enable;
    reg32 reserved;
    reg32 pull_up_down_enable;
    reg32 pull_up_down_clocks[2];
};

#define REGS_GPIO ((struct GpioRegs *)(PBASE + 0x00200000)) //the actual logic to access the pointter that accesses it.