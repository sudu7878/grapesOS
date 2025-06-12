// armstub/src/armstub.s

.global _start
_start:
    ldr w0, kernel_entry32
    br x0

.ltorg

.org 0xF0
.globl stub_magic
stub_magic:
    .word 0x5AFE570B

.org 0x0F4
.globl stub_version
stub_version:
    .word 0

.org 0xFC
.globl kernel_entry32
kernel_entry32:
    .word 0x0