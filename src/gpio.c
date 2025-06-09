// src/gpio.c

//gpio controller

#include "peripherals/gpio_hedr.h"
#include "gpio.h"
#include "utils.h"
#include "common.h"


void gpio_pin_set_func(u8 pinNumber, GpioFunc func) {
    u8 bitStart = (pinNumber * 3) % 30;
    u8 reg = pinNumber / 10;

    u32 selector = REGS_GPIO->func_select[reg];
    selector &= ~(7 << bitStart);
    selector |= (func << bitStart);

    REGS_GPIO->func_select[reg] = selector;
}


void gpio_pin_enable(u8 pinNumber) {
    REGS_GPIO->pull_up_down_enable = 0; // Disable pull-up/down
    delay(150);
    REGS_GPIO->pull_up_down_clocks[pinNumber / 32] = 1 << (pinNumber % 32); // Enable clock for the pin/
    delay(150);
    REGS_GPIO->pull_up_down_enable = 0; // Disable pull-up/down again
    REGS_GPIO->pull_up_down_clocks[pinNumber / 32] = 0; // Disable clock for the pin
}

