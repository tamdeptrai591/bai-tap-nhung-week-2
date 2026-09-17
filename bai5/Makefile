# =====================================================
# Compiler
# =====================================================

CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

PROGRAMMER = /opt/st/stm32cubeide_2.2.0/plugins/com.st.stm32cube.ide.mcu.externaltools.cubeprogrammer.linux64_2.2.500.202603051304/tools/bin/STM32_Programmer_CLI
# =====================================================
# MCU
# =====================================================

CPU = cortex-m3

CFLAGS = -mcpu=$(CPU) \
         -mthumb \
         -O0 \
         -g \
         -ffreestanding \
         -Wall \
         -Wextra \
         -Iinc


# =====================================================
# Linker
# =====================================================

LDFLAGS = -mcpu=$(CPU) \
          -mthumb \
          -Tstm32f103c8.ld \
          -nostdlib \
          -Wl,-Map=pro.map


# =====================================================
# Source files
# =====================================================

C_SOURCES = \
    src/main.c \
    src/pwm.c \
    src/uart.c\
    src/clock.c

ASM_SOURCES = \
    src/startup.s


# =====================================================
# Object files
# =====================================================

C_OBJECTS = \
    main.o \
    pwm.o \
    uart.o \
    clock.o

ASM_OBJECTS = \
    startup.o

OBJECTS = $(C_OBJECTS) $(ASM_OBJECTS)


# =====================================================
# Target
# =====================================================

TARGET = pro


# =====================================================
# Default
# =====================================================

all: $(TARGET).elf $(TARGET).bin
	@echo ""
	@echo "======================================"
	@echo " BUILD SUCCESS"
	@echo "======================================"
	@$(SIZE) $(TARGET).elf


# =====================================================
# Compile C
# =====================================================

main.o: src/main.c
	$(CC) $(CFLAGS) -c $< -o $@

pwm.o: src/pwm.c
	$(CC) $(CFLAGS) -c $< -o $@

uart.o: src/uart.c
	$(CC) $(CFLAGS) -c $< -o $@

clock.o: src/clock.c
	$(CC) $(CFLAGS) -c $< -o $@

# =====================================================
# Assemble startup
# =====================================================

startup.o: src/startup.s
	$(CC) $(CFLAGS) -c $< -o $@


# =====================================================
# Link
# =====================================================

$(TARGET).elf: $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@


# =====================================================
# Convert ELF -> BIN
# =====================================================

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@


# =====================================================
# Clean
# =====================================================

clean:
	rm -f *.o
	rm -f *.elf
	rm -f *.bin
	rm -f *.map
	rm -f $(OBJECTS)


.PHONY: all clean

flash: $(TARGET).bin
	$(PROGRAMMER) -c port=SWD -w $(TARGET).bin 0x08000000 -v -rst

.PHONY: all clean flash