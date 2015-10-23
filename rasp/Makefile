all: kernel.img

boot.o: boot/boot.s
	arm-none-eabi-gcc -mcpu=arm1176jzf-s -fpic -ffreestanding -c boot/boot.s -g -o boot.o

kernel.o: kernel/kernel.c
	arm-none-eabi-gcc -mcpu=arm1176jzf-s -fpic -ffreestanding -std=gnu99 -c kernel/kernel.c -g -o kernel.o -O2 -Wall -Wextra

myos.elf: boot.o kernel.o
	arm-none-eabi-gcc -T link/linker.ld -g -o myos.elf -ffreestanding -O2 -nostdlib boot.o kernel.o

kernel.img: myos.elf
	arm-none-eabi-objcopy myos.elf -O binary kernel.img

# requires qemu-system-arm installed.
# ctrl-a h for help in qemu nographic mode
qemu: boot.o
	arm-none-eabi-gcc -mcpu=arm1176jzf-s -fpic -ffreestanding -std=gnu99 -c kernel/kernel.c -g -o kernel.o -O2 -Wall -Wextra -D IS_EMULATE
	arm-none-eabi-gcc -T link/linker.ld -g -o myos.elf -ffreestanding -O2 -nostdlib boot.o kernel.o
	qemu-system-arm -nographic -kernel myos.elf -cpu arm1176 -m 256 -M versatilepb

clean:
	rm -f *.o

very_clean:
	rm -rf *.o
	rm -rf *.elf
	rm -rf *.img

