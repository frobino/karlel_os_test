# karlel_os_test
A basic OS to understand and document the basics plus crazy ideas

# goals of the project:

- PORTABILITY: starting from the guide available at http://wiki.osdev.org, develope a simple OS portable between my dev boards. Merging material from tutorials at valvers.com

- KNOWLEDGE OF BARE METAL: understand in details the boot sequence of an embedded system, create my own run time (memory segmentation - boot.s - , placement of segments in memory - linker script -, ...)

- TEST NEW LANGUAGES: e.g. a new "bare bones" in languages such as haskell or rust?

# to add:

GPU - bootloading phase:

1) boots off of an on chip rom of some sort
2) reads the sd card and looks for additional gpu specific boot files
bootcode.bin and start.elf in the root dir of the first partition
(fat32 formatted, loader.bin no longer used/required)
3) in the same dir it looks for config.txt which you can do things like
change the arm speed from the default 700MHz, change the address where
to load kernel.img, and many others
4) it reads kernel.img the arm boot binary file and copies it to memory
5) releases reset on the arm such that it runs from the address where
the kernel.img data was written

C run time (crt0) - what it is:

It is not a linker, but it is a small program (compiled to .o) which initializes sections of memory in a specific way (see below).

C run time (crt0) - what it is supposed to do (see boot.*.s):

1] initialize the pre-initialized variables before the main() is called. In other words, it copies the initial values of initialized variables from non-volatile to volatile memory (ram). Example of pre initialized variable:
unsigned int var = 3; (data section?)
int main ()
...

2] initialize to 0 all the non pre-installed variables (the ones in bss section)

3] setup the stack pointer.

See the boot folder for files implementing a custom ctr0.o

Others:

The memory is split between the GPU and the ARM, I believe the default
is to split the memory in half.  And there are ways to change that
split (to give the ARM more).  Not going to worry about that here.

From the ARMs perspective the kernel.img file is loaded, by default,
to address 0x8000.  (there are ways to change that, not going to worry
about that right now).

# How to create bootable SD card from scratch:
References: https://wiki.gentoo.org/wiki/Raspberry_Pi#Preparing_the_SD_card and http://elinux.org/ArchLinux_Install_Guide

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
   Create and mount the FAT filesystem:
       mkfs.vfat /dev/sdX1
       mkdir boot
       mount /dev/sdX1 boot
   Create and mount the ext4 filesystem:
       mkfs.ext4 /dev/sdX2
       mkdir root 
       mount /dev/sdX2 root
   NOTE:
       root will contain the so called "system root", the OS partition containing /boot /usr etc.
       boot will contain the bootloader and everything called to initialize the OS
   Put in the boot partition the following files:
       bootcode.bin (proprietary raspberry)
       start.elf (proprietary raspberry)
       cmdline.txt (?)
       and finally the generated kernel.img (must be named so)
   Unmount the two partitions:
       umount boot root
   Insert the SD card into the Raspberry Pi, connect ethernet, and apply 5V power.
   
# References:
- HOWTO QEMU:
  http://blog.bobuhiro11.net/2014/01-13-baremetal.html,
  https://upload.wikimedia.org/wikiversity/en/e/ef/ESys.5.A.Hello.20141112.pdf

# TODO next:

X describe how I created bootable SDcard from scratch (see gentoo/arch tutorials for raspberry pi)
- continue with meaty tutorial (wiki osdev) and add clib support.
  The goal with this part is to: 1) reorg code structure in different projects (kernel, libcsupport,...)
  2) Each project will then create different files which must be copied to different places on the OS hd.
  For example, the kernel project will create the kernel.img file to be inserted in BOOT partition, while the libcsupport project will create files to be inserted in system root (ROOT) /usr/lib usr/include.
  I will have still to investigate how the kernel.img file will "mount/see" the ROOT partition...

- linker impl a interrupt vector table? see baremetal projects
- read more about freestanding and nostdlib flags. When using both, no stdlib is included -> I have to port my own as described in http://wiki.osdev.org/Meaty_Skeleton and http://wiki.osdev.org/Creating_a_C_Library, or linking a pre made std C library as newlib.
- if nostdlib flag is removed (and I add a call no malloc), I get a similar result as step2 -  armc09 from valvers tutorial. 
