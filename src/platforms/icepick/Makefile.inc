CROSS_COMPILE ?= riscv-none-embed-
CC = $(CROSS_COMPILE)gcc
OBJCOPY = $(CROSS_COMPILE)objcopy

OPT_FLAGS = -Os
CPU_FLAGS = -march=rv32i -mabi=ilp32
CFLAGS += $(INCLUDES) $(CPU_FLAGS) -DBOARD_ICEPICK -DENABLE_DEBUG -DNO_LIBOPENCM3 -flto
ASFLAGS = $(CFLAGS)

LINKER_SCRIPT="platforms/icepick/icepick.ld"
LDFLAGS = -ffreestanding -nostartfiles $(CPU_FLAGS) -T$(LINKER_SCRIPT) -Wl,--gc-section --specs=nano.specs
	
VPATH += platforms/common
SRC += start.S timing.c libc.c

all: blackmagic.bin

