## C run time (crt0)

**What it is**:

It is not a linker, but it is a small program (compiled to .o) which initializes sections of memory in a specific way (see below).

**What it is supposed to do (see boot.*.s)**:

1] initialize the pre-initialized variables before the main() is called. In other words, it copies the initial values of initialized variables from non-volatile to volatile memory (ram). Example of pre initialized variable:
unsigned int var = 3; (data section?)
int main ()
...

2] initialize to 0 all the non pre-installed variables (the ones in bss section)

3] setup the stack pointer.

See the boot folder for files implementing a custom ctr0.o
