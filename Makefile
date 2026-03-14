# Makefile.tutorial -- a tiny guided tour of how this Makefile works.
# Read top to bottom; every variable is used later in a rule.

# 1) What are we building?
# The programs used in today's lab: each has a notmain.
# NOTE: this variable and many of the others are not required:
# we use them to (try to) add clarify.  The only real requirement
# is specifying what depends on what (the rules from "all:" to EOF).
PROG_SRC := $(filter-out utils.c interrupts.c hardware.c threads.c, $(wildcard *.c))

# 2) For each PROG_SRC X.c we build a X.bin
PROGS := $(PROG_SRC:.c=.bin)

# These common object files are linked into every program
# so we track them using a variable
DRIVER_SRC := $(wildcard drivers/*.c)

OBJS := start.o utils.o interrupts.o hardware.o
OBJS += $(DRIVER_SRC:.c=.o)

THREAD_OBJS := threads.o threads-asm.o  # ← add this line

# The linker script decides where code/data go in memory.
MEMMAP := ./memmap

# 3) Cross‑compiler toolchain: your laptop probably does not
# have the same CPU as the pi, so we have to "cross-compile"
# using a different compiler than your native one.  You set
# this up in lab: 0-setup-pi.
ARM     := arm-none-eabi
CC      := $(ARM)-gcc

# 4) Compiler flags.
# gnu99 + error checking: don't compile if with warnings.
CFLAGS := -O -Wall -Wno-unused-variable -Werror -std=gnu99 
# no C runtime or libc.
CFLAGS += -nostdlib -nostartfiles -ffreestanding 
# our pi zero w runs on an ARM1176JZF-S (ARMv6ZK) processor
CFLAGS += -mcpu=arm1176jzf-s
# avoids unaligned access traps.
CFLAGS += -mno-unaligned-access
CFLAGS += -I. -Idrivers

# 5) The default target: build all binaries.
all: $(PROGS)


threaded_main.bin: threaded_main.o $(THREAD_OBJS) $(OBJS) $(MEMMAP)
	$(CC) $(CFLAGS) start.o $< $(THREAD_OBJS) $(filter-out start.o, $(OBJS)) -T $(MEMMAP) -lgcc -o threaded_main.elf
	$(ARM)-objcopy threaded_main.elf -O binary $@
	$(ARM)-objdump -D threaded_main.elf > threaded_main.list


threads_test.bin: threads_test.o $(THREAD_OBJS) $(OBJS) $(MEMMAP)
	$(CC) $(CFLAGS) start.o $< $(THREAD_OBJS) $(filter-out start.o, $(OBJS)) -T $(MEMMAP) -lgcc -o threads_test.elf
	$(ARM)-objcopy threads_test.elf -O binary $@
	$(ARM)-objdump -D threads_test.elf > threads_test.list

# 6) The rule for building a .bin.  We incidentally also
# produce a .elf and .list (disassembled elf).
%.bin: %.o $(OBJS) $(MEMMAP) Makefile *.h *.c drivers/*.h drivers/*.c
	$(CC) $(CFLAGS) $(OBJS) $< -T $(MEMMAP) -lgcc -o $*.elf
	$(ARM)-objcopy $*.elf -O binary $@
	$(ARM)-objdump -D $*.elf > $*.list

# 7) The rule for building a .o from a .c C file.  The 
# rule states that every .o will get remade if its 
# .c (same name), rpi.h, or Makefile changes (is newer
# then the .o)
%.o: %.c Makefile *.h *.c
	$(CC) $(CFLAGS) -c $< -o $@

# 8) The rule for building a .o from a .S file. Similar 
# dependencies.  Note: uppercase .S means the C preprocessor
# runs first, b/c we use gcc we can use macros in the .S (v handy).
%.o: %.S Makefile *.h *.c
	$(CC) $(CFLAGS) -c $< -o $@

# 9) "make clean": Delete all generated files (not sources).
clean:
	rm -f *.bin *.list *.elf *.o Makefile.bak *~

# 10) tell `make` these are fake targets (do not generate a file)
# to reduce some confusing errors.  (Won't arise today, but easily
# can in a larger project.)
.PHONY: all clean
