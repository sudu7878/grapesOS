//include/peripherals/base.h

/* just some standard helper registers that can be set for a particular parent function*/

#pragma once

#include "base.h"
#include "common.h"


// just makinh a data structure for the auxillary registers here.
struct AuxRegs {
    reg32 irq_status;
    reg32 enables;
    reg32 reserved[2];
    reg32 mu_io;
    reg32 mu_ier;
    reg32 mu_iir;
    reg32 mu_lcr;
    reg32 mu_mcr;
    reg32 mu_lsr;
    reg32 mu_msr;
    reg32 mu_scratch;
    reg32 mu_control;
    reg32 mu_status;
    reg32 mu_baud_rate;
};

#define REGS_AUX ((struct AuxRegs *)(PBASE + 0x00215000))