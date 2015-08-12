all: kernel.img

boot.o: boot/boot.s
	arm-none-eabi-gcc -mcpu=arm1176jzf-s -fpic -ffreestanding -c boot/boot.s -o boot.o

kernel.o: kernel/kernel.c
	arm-none-eabi-gcc -mcpu=arm1176jzf-s -fpic -ffreestanding -std=gnu99 -c kernel/kernel.c -o kernel.o -O2 -Wall -Wextra

myos.elf: boot.o kernel.o
	arm-none-eabi-gcc -T link/linker.ld -o myos.elf -ffreestanding -O2 -nostdlib boot.o kernel.o

kernel.img: myos.elf
	arm-none-eabi-objcopy myos.elf -O binary kernel.img

clean:
	rm -f *.o

very_clean:
	rm -f *.o
	rm -f *.elf
	rm -f *.bin

