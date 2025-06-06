// include/utils.h

//just some utility functions that are used throughout the project, like delay and memory access functions, nothing much to see here.

#pragma once
#include "common.h"

void delay(u64 ticks);
void put32(u64 address, u32 value);
unsigned int get32(u64 address);
