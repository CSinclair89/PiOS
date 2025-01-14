
# Verbosity toggle
VERBOSE ?= 0
ifeq ($(VERBOSE), 1)
	Q =
else
	Q = @
endif

# Compiler setup
CC := aarch64-linux-gnu-gcc
LD := aarch64-linux-gnu-ld
OBJDUMP := aarch64-linux-gnu--objdump
OBJCOPY := aarch64-linux-gnu-objcopy
CONFIGS := -DCONFIG_HEAP_SIZE=4096

CFLAGS := -O0 -ffreestanding -fno-pie -fno-stack-protector -g3 -mcpu=cortex-a53+nofp -Wall $(CONFIGS)

# Directories and objects
ODIR = obj
SDIR = src

OBJS = \
	boot.o \
	kernel_main.o \
	ds.o \
	timer.o \
	page.o \
	mmu.o \
	test.o \
	string.o \
	algos.o \
	tree.o \
	mem.o \
	io.o \


OBJ = $(patsubst %,$(ODIR)/%,$(OBJS))

# Compilation rules
$(ODIR)/%.o: $(SDIR)/%.c
	$(Q)echo "Compiling $< ..."
	$(Q)$(CC) $(CFLAGS) -c -g -o $@ $^

$(ODIR)/%.o: $(SDIR)/%.s
	$(Q)echo "Assembling $< ..."
	$(Q)$(CC) $(CFLAGS) -c -g -o $@ $^


all: bin rootfs.img

bin: $(OBJ)
	@echo "Linking kernel image..."
	$(Q)$(LD) $(OBJ) -Tkernel.ld -o kernel8.img
	$(Q)cp kernel8.img kernel8.elf
	$(Q)$(OBJCOPY) -O binary kernel8.img
	$(Q)aarch64-linux-gnu-size kernel8.elf

clean:
	@echo "Cleaning build files..."
	rm -f obj/*
	rm -f rootfs.img
	rm -f rootfs.img
	rm -f kernel8.img
	rm -f kernel8.elf

debug:
	screen -S qemu -d -m qemu-system-aarch64 -machine raspi3b -kernel kernel8.img -hda rootfs.img -S -s -serial null -serial stdio -monitor none -nographic -k en-us 
	TERM=xterm gdb-multiarch -x gdb_init_prot_mode.txt && killall qemu-system-aarch64

run:
	qemu-system-aarch64 -machine raspi3b -kernel kernel8.img -hda rootfs.img -serial null -serial stdio -monitor none -nographic -k en-us

disassemble:
	$(OBJDUMP) -D kernel8.elf

rootfs.img:
	dd if=/dev/zero of=rootfs.img bs=1M count=16
	mkfs.fat -F12 rootfs.img
	sudo mkdir -p /mnt/disk
	sudo mount rootfs.img /mnt/disk
	sudo mkdir -p /mnt/disk/boot/firmware
	sudo mkdir /mnt/disk/bin
	sudo mkdir /mnt/disk/etc
	sudo umount /mnt/disk

