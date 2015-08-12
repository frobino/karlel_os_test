	// Define a section, so that later the linker can put this code in the
	// first portion of the binary
	
	.section ".text.boot"

	// Make _start global
	// NOTE: section _start is defined in linker script
	.globl _start

	// Entry point for the kernel.
	// ARMv6 (see XXX.pdf):
	// r15 -> pc - should start executing at 0x8000 -
	// r0 -> 0x00000000
	// r1 -> 0x00000C42 (gp reg, initialized by gpu bootloader)
	// r2 -> 0x00000100 (gp reg, initialized by gpu bootloader - start of ATAGS -)
	// NOTE:
	// the previous regs MUST BE PRESERVED as arguments for kernel_main()
_start:
	// setup the STACK
	mov sp, #0x8000

	// clear out .BSS
	// NOTE: sections _bss_start, _bss_end are defined in linker script
	ldr r4, =__bss_start
	ldr r9, =__bss_end
	mov r5, #0
	mov r6, #0
	mov r7, #0
	mov r8, #0
	// branch to label 2
	b 	2f
	
1:
	// store multiple at r4.
	stmia r4!, {r5-r8}
 
	// If we are still below bss_end, loop.
2:
	cmp r4, r9
	// this is the condition checking if we are below bss_end
	blo 1b
 
	// Call kernel_main
	ldr r3, =kernel_main
	blx r3
 
	// halt
halt:
	wfe
	b halt
	

	
