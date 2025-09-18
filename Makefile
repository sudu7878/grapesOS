# Makefile for grapesOS

# path thingy
SRC = src
ARMSTUB = SRC/ARMSTUB
ARMSTUB_SRC = ARMSTUB/src
ARMSTUB_BLD = ARMGNU/Builds
BUILD = Builds/TestBuilds
LINKER = link.ld
INCLUDE = include

#OBJs
OBJ = \
  $(BUILD)/boot.o \
  $(BUILD)/cacheF.o \
  $(BUILD)/mem_mang.o \
  $(BUILD)/video_driver.o\
  $(BUILD)/utils.o \
  $(BUILD)/entry.o \
  $(BUILD)/bgudakrnl.o \
  $(BUILD)/gpio.o \
  $(BUILD)/uart.o \
  $(BUILD)/text_rend.o\
  $(BUILD)/irq.o \
  $(BUILD)/irq_S.o \
  $(BUILD)/printf.o \
  $(BUILD)/mailbox.o


# tools
CC = aarch64-none-elf-gcc
AS = aarch64-none-elf-as
LD = aarch64-none-elf-ld
OBJCOPY = aarch64-none-elf-objcopy

# targets
C_SOURCES = $(SRC)/bgudakrnl.c $(SRC)/gpio.c $(SRC)/uart.c
ASM_SOURCES = $(SRC)/boot.S $(SRC)/mem_mang.S $(SRC)/utils.S $(SRC)/entry.S $(ARMSTUB_SRC)/armstub.S
BGUDAKRNL_ELF = $(BUILD)/kernel.elf
BGUDAKRNL_IMG = $(BUILD)/kernel8.img

# build directory
$(BUILD):
	mkdir -p $(BUILD)

# default target
all: $(BGUDAKRNL_IMG)

# assembly object files
$(BUILD)/boot.o: $(SRC)/aarch/boot.S | $(BUILD)
	$(CC) -c -mcpu=cortex-a72 -march=armv8-a -O2 -Wall -nostdlib -nostartfiles -ffreestanding -I$(INCLUDE) -o $@ $<

$(BUILD)/mem_mang.o: $(SRC)/aarch/mem_mang.S | $(BUILD)
	$(CC) -c -mcpu=cortex-a72 -march=armv8-a -O2 -Wall -nostdlib -nostartfiles -ffreestanding -I$(INCLUDE) -o $@ $<

$(BUILD)/cacheF.o: $(SRC)/kernel/cacheF.c | $(BUILD)
	$(CC) -c -mcpu=cortex-a72 -march=armv8-a -O2 -Wall -nostdlib -nostartfiles -ffreestanding -I$(INCLUDE) -o $@ $<

$(BUILD)/utils.o: $(SRC)/aarch/utils.S | $(BUILD)
	$(CC) -c -mcpu=cortex-a72 -march=armv8-a -O2 -Wall -nostdlib -nostartfiles -ffreestanding -I$(INCLUDE) -o $@ $<


# c object files

$(BUILD)/gpio.o: $(SRC)/drivers/gpio.c | $(BUILD)
	$(CC) -c -mcpu=cortex-a72 -march=armv8-a -O2 -Wall -nostdlib -nostartfiles -ffreestanding -I$(INCLUDE) -o $@ $<

$(BUILD)/uart.o: $(SRC)/drivers/uart.c | $(BUILD)
	$(CC) -c -mcpu=cortex-a72 -march=armv8-a -O2 -Wall -nostdlib -nostartfiles -ffreestanding -I$(INCLUDE) -o $@ $<

$(BUILD)/irq.o: $(SRC)/kernel/irq.c | $(BUILD)
	$(CC) -c -mcpu=cortex-a72 -march=armv8-a -O2 -Wall -nostdlib -nostartfiles -ffreestanding -I$(INCLUDE) -o $@ $<

$(BUILD)/video_driver.o: $(SRC)/drivers/video_driver.c | $(BUILD)
	$(CC) -c -mcpu=cortex-a72 -march=armv8-a -O2 -Wall -nostdlib -nostartfiles -ffreestanding -I$(INCLUDE) -o $@ $<

$(BUILD)/irq_S.o: $(SRC)/aarch/irq.S | $(BUILD)
	$(CC) -c -mcpu=cortex-a72 -march=armv8-a -O2 -Wall -nostdlib -nostartfiles -ffreestanding -I$(INCLUDE) -o $@ $<

$(BUILD)/printf.o: $(SRC)/printf.c | $(BUILD)
	$(CC) -c -mcpu=cortex-a72 -march=armv8-a -O2 -Wall -nostdlib -nostartfiles -ffreestanding -I$(INCLUDE) -o $@ $<

$(BUILD)/entry.o: $(SRC)/aarch/entry.S | $(BUILD)
	$(CC) -c -mcpu=cortex-a72 -march=armv8-a -O2 -Wall -nostdlib -nostartfiles -ffreestanding -I$(INCLUDE) -o $@ $<

$(BUILD)/mailbox.o: $(SRC)/drivers/mailbox.c | $(BUILD)
	$(CC) -c -mcpu=cortex-a72 -march=armv8-a -O2 -Wall -nostdlib -nostartfiles -ffreestanding -I$(INCLUDE) -o $@ $<

$(BUILD)/text_rend.o: $(SRC)/display/text_rend.c | $(BUILD)
	$(CC) -c -mcpu=cortex-a72 -march=armv8-a -O2 -Wall -nostdlib -nostartfiles -ffreestanding -I$(INCLUDE) -o $@ $<

$(BUILD)/bgudakrnl.o: $(SRC)/kernel/bgudakrnl.c | $(BUILD)
	$(CC) -c -mcpu=cortex-a72 -march=armv8-a -O2 -Wall -nostdlib -nostartfiles -ffreestanding -I$(INCLUDE) -o $@ $<


# link ELF
$(BGUDAKRNL_ELF): $(OBJ)		#builld all the obj files (defined above)
	$(LD) -T $(LINKER) -o $@ $^


# convert ELF to IMG
$(BGUDAKRNL_IMG): $(BGUDAKRNL_ELF)
	$(OBJCOPY) $< -O binary $@

armstyb/build/armstub_s.o: armstub/src/armstub.S
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(COPS) -mkdir -c $< -o $@

armstub: armstub/build/armstub_s.o
	$(ARMGNU)

# print debug
print-debug:
	@echo "SRC = $(SRC)"
	@echo "ASM_SOURCES = $(ASM_SOURCES)"
	@echo "Looking for files:"
	@ls -l $(ASM_SOURCES)

# cleanup
clean:
	rm -rf $(BUILD)/*.o $(BUILD)/*.elf $(BUILD)/*.img
