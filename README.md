# karlel_os_test

A basic OS to understand and document the basics on how to create a OS from scratch.

## Goals of the project:

- PORTABILITY: starting from the guide available at [http://wiki.osdev.org][wiki-osdev],
  develop a simple OS portable between my dev boards. The raspberry pi specific
  knowledge (v1,2,3,4) comes from [valvers tutorials][valvers].

- KNOWLEDGE OF BARE METAL: understand in details the boot sequence of an embedded system,
  create my own run time (memory segmentation - *boot.s*, placement of segments
  in memory - *linker script*, ...)

- TEST NEW LANGUAGES: e.g. a new "bare bones" in languages such as haskell or rust?

## Requirements

The local machine building the OS (i.e. your PC) requires the following packages
to be installed:

```
gcc-arm-none-eabi
qemu-system-arm
```

## How to build and run the karlel OS (default rpi v1)

Create the OS image (default rpi v1):

```
cd rasp
make
```

To run the OS image on qemu (default rpi v1):

```
make qemu
```

To run the OS on the real rpi v1 board:

1. [Create a bootable SD card](#How-to-create-bootable-SD-card-from-scratch)
2. Copy the [rpi1 specifi boot files](rasp/Proprietary_boot_files/) in the boot partition:
  - *bootcode.bin* (proprietary raspberry, the first binary read from GPU. Load and execute start.elf)
  - *start.elf* (proprietary raspberry, runs on GPU. Looks for kernel.img. When it founds it, the GPU triggers the reset line on the ARM processor, which starts executing code at address 0x80000)
  - *cmdline.txt* (used by start.elf to configure arm speed, etc.)
  - *kernel.img* (our os, created from the make command above, must be named so)
3. Insert the SD card into the Raspberry Pi, connect ethernet, and apply 5V power.

In order to see any thing from the raspberry pi, we need a to connect a uart
to the board, with baud 115200.

## How to build and run u-boot on rpi v4

Build [u-boot][u-boot] for your board. Follow the instructions in the [u-boot][u-boot] repo.
The build process will create the following files:
- boot.src
- u-boot.bin

In the SD card boot partition we MUST put the "first thing to run" as a file named kernel7l.img.

For example, when trying to start u-boot first, we take the uboot.bin,
and rename it to kernel7l.img (togheter with boot.src, which tells to u-boot
what to boot, the uImage file).

cp ~/Projects/u-boot/boot.scr /media/$USER/F141-D800/boot.scr 
cp ~/Projects/u-boot/u-boot.bin /media/$USER/F141-D800/kernel7l.img 
cp ~/Projects/<myOS> /media/$USER/F141-D800/uImage

If we do not want to use u-boot, pick the pre compiled fw at https://github.com/raspberrypi/firmware

Note that for rpi4, no gpu boot is needed? so no start.elf.

## Documentation

[My summary on C run time (crt0)](doc/crt0.md)

[QEMU for rpi v1 (1/2)][qemu-rpi1-1]

[QEMU for rpi v1 (2/2)][qemu-rpi1-2]

[Generic boot process description (broken link)][generic-boot]

### Raspberry Pi v1 specific

[My summary on bootload procedure](doc/rpi1/gpu-bootload-phase.md)

[Someone else summary on bootload procedure][rpi3-boot]

### How to create bootable SD card from scratch:

Start fdisk to partition the SD card:

    fdisk /dev/sdX

At the fdisk prompt, delete old partitions and create a new one:
    
	Type o. This will clear out any partitions on the drive.
    Type p to list partitions. There should be no partitions left.
    Type n, then p for primary, 1 for the first partition on the drive, press ENTER to
        accept the default first sector, then type +100M for the last sector.
    Type t, then c to set the first partition to type W95 FAT32 (LBA).
    Type n, then p for primary, 2 for the second partition on the drive, and then press
        ENTER twice to accept the default first and last sector.
    Write the partition table and exit by typing w.

Create and mount the FAT filesystem in the boot partition:
    
	mkfs.vfat /dev/sdX1
    mkdir boot
    mount /dev/sdX1 boot

Create and mount the ext4 filesystem in the root partition:
    
	mkfs.ext4 /dev/sdX2
    mkdir root 
    mount /dev/sdX2 root

Unmount the two partitions:
    
	umount boot root

NOTE:

- **boot** will contain the bootloader and everything called to initialize the OS
- **root** will contain the so called "system root", the classic OS partition (/)
  containing /bin, /boot, /home, /usr, etc.

References: [prepare SD card (1/2)][sd-1] and [prepare SD card (2/2)][sd-2]

# TODO next:

- continue with meaty tutorial (wiki osdev) and add clib support.
  The goal with this part is to: 1) reorg code structure in different projects (kernel, libcsupport,...)
  2) Each project will then create different files which must be copied to different places on the OS hd.
  For example, the kernel project will create the kernel.img file to be inserted in BOOT partition, while the libcsupport project will create files to be inserted in system root (ROOT) /usr/lib usr/include.
- I will have still to investigate how the kernel.img file will "mount/see" the ROOT partition... http://wiki.beyondlogic.org/index.php?title=RaspberryPi_RootNFS (see understanding rpi boot process) gives very good overview, explaining the roles of start.exe and *config.txt* and *cmdline.txt*. From there it is not clear yet how root get mounted, but it looks like start.exe passes some params to the kernel.img who is the responsable to mount/see the root partition (http://elinux.org/images/4/4f/02-linux-quick-start.pdf).  
- See also 
   - When the Raspberry Pi is first turned on, the ARM core is off, and the GPU core is on. At this point the SDRAM is disabled.
   - The GPU starts executing the first stage bootloader, which is stored in ROM on the SoC. The first stage bootloader reads the SD card, and loads the second stage bootloader (bootcode.bin) into the L2 cache, and runs it.
   - bootcode.bin enables SDRAM, and reads the third stage bootloader (loader.bin) from the SD card into RAM, and runs it.
   - loader.bin reads the GPU firmware (start.elf).
   - start.elf reads config.txt, cmdline.txt and kernel.img

Others:
- linker impl a interrupt vector table? see baremetal projects
- read more about freestanding and nostdlib flags. When using both, no stdlib is included -> I have to port my own as described in http://wiki.osdev.org/Meaty_Skeleton and http://wiki.osdev.org/Creating_a_C_Library, or linking a pre made std C library as newlib.
- if nostdlib flag is removed (and I add a call no malloc), I get a similar result as step2 -  armc09 from valvers tutorial. 
- http://wiki.osdev.org/Kernel_Multitasking

[wiki-osdev]: http://wiki.osdev.org
[valvers]: https://www.valvers.com/open-software/raspberry-pi/bare-metal-programming-in-c-part-1/
[generic-boot]: http://www.ibm.com/developerworks/library/l-linuxboot/index.html
[qemu-rpi1-1]: http://blog.bobuhiro11.net/2014/01-13-baremetal.html,
[qemu-rpi1-2]: https://upload.wikimedia.org/wikiversity/en/e/ef/ESys.5.A.Hello.20141112.pdf
[sd-1]: https://wiki.gentoo.org/wiki/Raspberry_Pi#Preparing_the_SD_card
[sd-2]: http://elinux.org/ArchLinux_Install_Guide
[u-boot]: https://gitlab.denx.de/u-boot/u-boot.git
[rpi3-boot]: https://github.com/bztsrc/raspi3-tutorial#about-the-hardware
