//include/peripherals/base.h

#pragma once

#ifndef RPI_VERSION
#define RPI_VERSION 4 // Default to RPI_VERSION 4 if not defined
#endif

#if RPI_VERSION == 4
#define PBASE 0xFE000000

#elif RPI_VERSION == 3
#define PBASE 0x3F000000

#else
#define PBASE 0
//#error RPI_VERSION not defined or unsupported version!

#endif