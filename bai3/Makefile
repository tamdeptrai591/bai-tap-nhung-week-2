# Khai báo Toolchain
CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

# Các cờ compile (Đã bật Debug bằng -g3 và tắt tối ưu hóa bằng -O0)
CFLAGS = -mcpu=cortex-m3 -mthumb -O0 -g3 -Wall
CFLAGS += -I./Drivers/STM32F1xx_HAL_Driver/Inc
CFLAGS += -I./Drivers/CMSIS/Include
CFLAGS += -I./Drivers/CMSIS/Device/ST/STM32F1xx/Include
CFLAGS += -I.
CFLAGS += -DSTM32F103xB -DUSE_HAL_DRIVER

# Cờ dành riêng cho file Assembly
ASFLAGS = -mcpu=cortex-m3 -mthumb

# Script Linker 
LDFLAGS = -mcpu=cortex-m3 -mthumb -TSTM32F103C8Tx_FLASH.ld -Wl,--gc-sections --specs=nosys.specs

# Danh sách các file mã nguồn (Đã bổ sung thư viện rcc_ex)
SRCS = main.c \
       system_stm32f1xx.c \
       Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.c \
       Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cortex.c \
       Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc.c \
       Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc_ex.c \
       Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio.c \
       Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_uart.c \
       Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dma.c \
       Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_adc.c \
       Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_adc_ex.c \
       startup_stm32f103xb.s

OBJS = $(SRCS:.c=.o)
OBJS := $(OBJS:.s=.o)

all: firmware.bin

# Quy tắc biên dịch file Assembly (.s)
%.o: %.s
	$(CC) $(ASFLAGS) -c -o $@ $<

# Quy tắc biên dịch file C (.c)
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

firmware.elf: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

firmware.bin: firmware.elf
	$(OBJCOPY) -O binary $< $@

clean:
	rm -f $(OBJS) firmware.elf firmware.bin
