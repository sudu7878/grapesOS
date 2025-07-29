//include/peripherals/auxillary.h

/* just some standard helper registers that can be set for a particular parent function*/

#pragma once

#include "base.h"
#include "common.h"


// just makinh a data structure for the auxillary registers here.
struct AuxRegs {
    reg32 irq_status;
    reg32 enables;
    reg32 reserved[14];
    reg32 mu_io;        //0x10
    reg32 mu_ier;       //0x14
    reg32 mu_iir;       //0x18
    reg32 mu_lcr;       //0x1C
    reg32 mu_mcr;       //0x20
    reg32 mu_lsr;       //0x24
    reg32 mu_msr;       //0x28
    reg32 mu_scratch;   //0x2C
    reg32 mu_control;   //0x30
    reg32 mu_status;    //0x34
    reg32 mu_baud_rate; //0x38
};  

#define REGS_AUX ((struct AuxRegs *)(PBASE + 0x00215000))