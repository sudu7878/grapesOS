# path thingy
SRC = src
BUILD = Builds/TestBuilds
LINKER = link.ld
INCLUDE = include

# tools
CC = aarch64-none-elf-gcc #for compiling C code
AS = aarch64-none-elf-as #for assembling assembly code
LD = aarch64-none-elf-ld #for linking object files
OBJCOPY = aarch64-none-elf-objcopy # for converting ELF to binary

# targets
C_SOURCES = $(SRC)/bgudakrnl.c
ASM_SOURCES = $(SRC)/boot.s
BGUDAKRNL_ELF = $(BUILD)/bgudakrnl.elf
BGUDAKRNL_IMG = $(BUILD)/bgudakrnl.img

$(BUILD):
	mkdir -p $(BUILD)
# ensure build directory exists


# default builds
all: $(BGUDAKRNL_IMG)



# assembly stuff
$(BUILD)/boot.o: $(ASM_SOURCES) | $(BUILD)
	$(AS) -o $@ $<

# c stuff
$(BUILD)/bgudakrnl.o: $(C_SOURCES) | $(BUILD)
	$(CC) -c -mcpu=cortex-a72 -march=armv8-a -O2 -Wall -nostdlib -nostartfiles -ffreestanding -I$(INCLUDE) -o $@ $<

# img-ing
$(BGUDAKRNL_IMG): $(BGUDAKRNL_ELF)
	$(OBJCOPY) $< -O binary $@

# link elf-ing
$(BGUDAKRNL_ELF): $(BUILD)/boot.o $(BUILD)/bgudakrnl.o
	$(LD) -T link.ld -o $(BUILD)/bgudakrnl.elf $(BUILD)/boot.o $(BUILD)/bgudakrnl.o



print-debug:
	@echo "SRC = $(SRC)"
	@echo "ASM_SOURCES = $(ASM_SOURCES)"
	@echo "Looking for file:"
	@ls -l $(ASM_SOURCES)

#cleanup
clean:
	rm -rf $(BUILD)/*.o $(BUILD)/*.elf $(BUILD)/*.img