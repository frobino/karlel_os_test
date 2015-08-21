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

# TODO next:

- linker impl a interrupt vector table? see baremetal projects
- read more about freestanding and nostdlib flags. When using both, no stdlib is included -> I have to port my own as described in http://wiki.osdev.org/Meaty_Skeleton and http://wiki.osdev.org/Creating_a_C_Library, or linking a pre made std C library as newlib.
- if nostdlib flag is removed (and I add a call no malloc), I get a similar result as step2 -  armc09 from valvers tutorial. 
