PROG_SRC := $(filter-out utils.c interrupts.c hardware.c threads.c, $(wildcard *.c))
PROGS := $(PROG_SRC:.c=.bin)

DRIVER_SRC := $(wildcard drivers/*.c)
OBJS := start.o utils.o interrupts.o hardware.o
OBJS += $(DRIVER_SRC:.c=.o)
THREAD_OBJS := threads.o threads-asm.o

MEMMAP := ./memmap

ARM     := arm-none-eabi
CC      := $(ARM)-gcc

CFLAGS := -O -Wall -Wno-unused-variable -Werror -std=gnu99 
CFLAGS += -nostdlib -nostartfiles -ffreestanding 
CFLAGS += -mcpu=arm1176jzf-s
CFLAGS += -mno-unaligned-access
CFLAGS += -I. -Idrivers

all: $(PROGS)

main.bin: main.o $(THREAD_OBJS) $(OBJS) $(MEMMAP)
	$(CC) $(CFLAGS) start.o $< $(THREAD_OBJS) $(filter-out start.o, $(OBJS)) -T $(MEMMAP) -lgcc -o main.elf
	$(ARM)-objcopy main.elf -O binary $@
	$(ARM)-objdump -D main.elf > main.list

threads_test.bin: threads_test.o $(THREAD_OBJS) $(OBJS) $(MEMMAP)
	$(CC) $(CFLAGS) start.o $< $(THREAD_OBJS) $(filter-out start.o, $(OBJS)) -T $(MEMMAP) -lgcc -o threads_test.elf
	$(ARM)-objcopy threads_test.elf -O binary $@
	$(ARM)-objdump -D threads_test.elf > threads_test.list

%.bin: %.o $(OBJS) $(MEMMAP) Makefile *.h *.c drivers/*.h drivers/*.c
	$(CC) $(CFLAGS) $(OBJS) $< -T $(MEMMAP) -lgcc -o $*.elf
	$(ARM)-objcopy $*.elf -O binary $@
	$(ARM)-objdump -D $*.elf > $*.list

%.o: %.c Makefile *.h *.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S Makefile *.h *.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.bin *.list *.elf *.o Makefile.bak *~

.PHONY: all clean
