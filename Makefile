# path thingy
SRC = src
BUILD = Builds/TestBuilds
LINKER = link.ld
INCLUDE = include

# tools
CC = aarch64-none-elf-gcc
AS = aarch64-none-elf-as
LD = aarch64-none-elf-ld
OBJCOPY = aarch64-none-elf-objcopy

# targets
C_SOURCES = $(SRC)/bgudakrnl.c $(SRC)/gpio.c $(SRC)/uart.c
ASM_SOURCES = $(SRC)/boot.S $(SRC)/mem_mang.S $(SRC)/utils.S
BGUDAKRNL_ELF = $(BUILD)/bgudakrnl.elf
BGUDAKRNL_IMG = $(BUILD)/bgudakrnl.img

# build directory
$(BUILD):
	mkdir -p $(BUILD)

# default target
all: $(BGUDAKRNL_IMG)

# assembly object files
$(BUILD)/boot.o: $(SRC)/boot.S | $(BUILD)
	$(CC) -c -mcpu=cortex-a72 -march=armv8-a -O2 -Wall -nostdlib -nostartfiles -ffreestanding -I$(INCLUDE) -I$(INCLUDE)/peripherals -o $@ $<

$(BUILD)/mem_mang.o: $(SRC)/mem_mang.S | $(BUILD)
	$(CC) -c -mcpu=cortex-a72 -march=armv8-a -O2 -Wall -nostdlib -nostartfiles -ffreestanding -I$(INCLUDE) -I$(INCLUDE)/peripherals -o $@ $<

$(BUILD)/utils.o: $(SRC)/utils.S | $(BUILD)
	$(CC) -c -mcpu=cortex-a72 -march=armv8-a -O2 -Wall -nostdlib -nostartfiles -ffreestanding -I$(INCLUDE) -I$(INCLUDE)/peripherals -o $@ $<

# c object files
$(BUILD)/bgudakrnl.o: $(SRC)/bgudakrnl.c | $(BUILD)
	$(CC) -c -mcpu=cortex-a72 -march=armv8-a -O2 -Wall -nostdlib -nostartfiles -ffreestanding -I$(INCLUDE) -I$(INCLUDE)/peripherals -o $@ $<

$(BUILD)/gpio.o: $(SRC)/gpio.c | $(BUILD)
	$(CC) -c -mcpu=cortex-a72 -march=armv8-a -O2 -Wall -nostdlib -nostartfiles -ffreestanding -I$(INCLUDE) -I$(INCLUDE)/peripherals -o $@ $<

$(BUILD)/uart.o: $(SRC)/uart.c | $(BUILD)
	$(CC) -c -mcpu=cortex-a72 -march=armv8-a -O2 -Wall -nostdlib -nostartfiles -ffreestanding -I$(INCLUDE) -I$(INCLUDE)/peripherals -o $@ $<

# link ELF
$(BGUDAKRNL_ELF): $(BUILD)/boot.o $(BUILD)/mem_mang.o $(BUILD)/utils.o $(BUILD)/bgudakrnl.o $(BUILD)/gpio.o $(BUILD)/uart.o
	$(LD) -T $(LINKER) -o $@ $^

# convert ELF to IMG
$(BGUDAKRNL_IMG): $(BGUDAKRNL_ELF)
	$(OBJCOPY) $< -O binary $@

# print debug
print-debug:
	@echo "SRC = $(SRC)"
	@echo "ASM_SOURCES = $(ASM_SOURCES)"
	@echo "Looking for files:"
	@ls -l $(ASM_SOURCES)

# cleanup
clean:
	rm -rf $(BUILD)/*.o $(BUILD)/*.elf $(BUILD)/*.img
