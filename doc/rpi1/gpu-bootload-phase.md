## GPU - bootloading phase:

The Raspberry Pi v1 has a boot process that is started from the GPU.
This is what the GPU does on power on:

1. boots off of an on chip rom of some sort
2. reads the sd card and looks for additional gpu specific boot files
bootcode.bin and start.elf in the root dir of the first partition
(fat32 formatted, loader.bin no longer used/required)
3. in the same dir it looks for config.txt which you can do things like
change the arm speed from the default 700MHz, change the address where
to load kernel.img, and many others
4. it reads kernel.img the arm boot binary file and copies it to memory
5. releases reset on the arm such that it runs from the address where
the kernel.img data was written

## Memory split

The memory is split between the GPU and the ARM, I believe the default
is to split the memory in half.  And there are ways to change that
split (to give the ARM more).  Not going to worry about that here.

## Which file is booted

From the ARMs perspective the kernel.img file is loaded, by default,
to address 0x8000 (there are ways to change that, not going to worry
about that right now).
