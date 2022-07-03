
#if !defined(__cplusplus)
#include <stdbool.h>
#endif
#include <stddef.h>
#include <stdint.h>

#ifdef IS_RPI3
#include "mbox.h"
#endif

static inline void mmio_write(uint32_t reg, uint32_t data)
{
	*(volatile uint32_t *)reg = data;
}
 
static inline uint32_t mmio_read(uint32_t reg)
{
	return *(volatile uint32_t *)reg;
}

/* Loop <delay> times in a way that the compiler won't optimize away. */
/* NOTE: this function is useless now... should we move it to a "helper" file?
   or do we remove it completely?
 */
static inline void delay(int32_t count)
{
	asm volatile("__delay_%=: subs %[count], %[count], #1; bne __delay_%=\n"
		 : : [count]"r"(count) : "cc");
}
 
size_t strlen(const char* str)
{
	size_t ret = 0;
	while ( str[ret] != 0 )
		ret++;
	return ret;
}
 
enum
{
    // The GPIO registers base address.
    #ifdef IS_EMULATE
	GPIO_BASE = 0x101F4000,
    #elif IS_RPI3
	GPIO_BASE = 0x3F200000,
    #elif IS_RPI4
	GPIO_BASE = 0xFE200000,
    #else
	GPIO_BASE = 0x20200000,
    #endif
 
    // The offsets for reach register:

    /********************************************* 
     * CTRL actuation
     *********************************************/
    
    // Controls actuation of pull up/down to ALL GPIO pins.
    GPPUD = (GPIO_BASE + 0x94),
 
    // Controls actuation of pull up/down for specific GPIO pin.
    GPPUDCLK0 = (GPIO_BASE + 0x98),

    /********************************************* 
     * UART addresses
     *********************************************/
    
    // The base address for UART. 0x20201000
    #ifdef IS_EMULATE
	UART0_BASE = 0x101F1000,
    #else
	UART0_BASE = (GPIO_BASE + 0x1000),
    #endif
 
    // The offsets for reach register for the UART.
    UART0_DR     = (UART0_BASE + 0x00),
    UART0_RSRECR = (UART0_BASE + 0x04),
    UART0_FR     = (UART0_BASE + 0x18),
    UART0_ILPR   = (UART0_BASE + 0x20),
    UART0_IBRD   = (UART0_BASE + 0x24),
    UART0_FBRD   = (UART0_BASE + 0x28),
    UART0_LCRH   = (UART0_BASE + 0x2C),
    UART0_CR     = (UART0_BASE + 0x30),
    UART0_IFLS   = (UART0_BASE + 0x34),
    UART0_IMSC   = (UART0_BASE + 0x38),
    UART0_RIS    = (UART0_BASE + 0x3C),
    UART0_MIS    = (UART0_BASE + 0x40),
    UART0_ICR    = (UART0_BASE + 0x44),
    UART0_DMACR  = (UART0_BASE + 0x48),
    UART0_ITCR   = (UART0_BASE + 0x80),
    UART0_ITIP   = (UART0_BASE + 0x84),
    UART0_ITOP   = (UART0_BASE + 0x88),
    UART0_TDR    = (UART0_BASE + 0x8C),

    
    /********************************************* 
     * GPIO function select (used for all GPIOs): 
     * Used to configure GPIO pins to output or input
     *********************************************/

    /* #define GPIO_GPFSEL1    1 */
    GPIO_GPFSE1L = (GPIO_BASE + 0x4), // DOUBLE CHECK ON MANUAL
    
    /********************************************* 
     * GPIO set output high (used for all GPIOs):
     * Used to set a GPIO configured as output in high state (VDD)
     *********************************************/

    /* #define GPIO_GPSET0	7 */
    GPIO_GPSET0 = (GPIO_BASE + 0x1C), // DOUBLE CHECK ON MANUAL
    
    /********************************************* 
     * GPIO set output low (used for all GPIOs)
     * Used to set a GPIO configured as output in low state (GND)
     *********************************************/

     /* #define GPIO_GPCLR0     10 */
    GPIO_GPCLR0 = (GPIO_BASE + 0x28), // DOUBLE CHECK ON MANUAL
};

void ok_led_init()
{
	/* Write 1 to the GPIO16 init nibble in the Function Select 1 GPIO
	   peripheral register to enable GPIO16 as an output. 
	   See page 90 manual,     
	   bits 18 to 20 in the ‘GPIO Function Select 1′ (GPFSEL1) register control the GPIO16 pin.
	*/
	uint32_t temp_reg;
	temp_reg = mmio_read(GPIO_GPFSE1L);
	mmio_write(GPIO_GPFSE1L, (1 << 18) | temp_reg);

}

void ok_init_write(uint8_t data)
{
	if (!data) {
		/* Set the GPIO16 (pin number 16) output low ( Turn OK LED on )*/
		mmio_write(GPIO_GPCLR0, (1 << 16));
	}
	else {
		/* Set the GPIO16 (pin number 16) output high ( Turn OK LED off )*/
	        mmio_write(GPIO_GPSET0, (1 << 16));
	}

}

void uart_init_rpi3(){
    #ifdef IS_RPI3

    /* TODO:
       there are theories that a mbox is needed for initializing
       uart1 in rpi3/4 (https://github.com/bztsrc/raspi3-tutorial/tree/master/05_uart0).
       Check if this is true. On qemu seems to be running fine.
     */
     register unsigned int r;
 
     /* initialize UART */
     // *UART0_CR = 0;         // turn off UART0
     *(volatile uint32_t *)UART0_CR = 0;
 
     /* set up clock for consistent divisor values */
     mbox[0] = 9*4;
     mbox[1] = MBOX_REQUEST;
     mbox[2] = MBOX_TAG_SETCLKRATE; // set clock rate
     mbox[3] = 12;
     mbox[4] = 8;
     mbox[5] = 2;           // UART clock
     mbox[6] = 4000000;     // 4Mhz
     mbox[7] = 0;           // clear turbo
     mbox[8] = MBOX_TAG_LAST;
     mbox_call(MBOX_CH_PROP);
 
     /* map UART0 to GPIO pins */
     r=GPIO_GPFSE1L;
     r&=~((7<<12)|(7<<15)); // gpio14, gpio15
     r|=(4<<12)|(4<<15);    // alt0
     // *GPIO_GPFSE1L = r;
     *(volatile uint32_t *)GPIO_GPFSE1L = r;
     *(volatile uint32_t *)GPPUD = 0;            // enable pins 14 and 15
     r=150; while(r--) { asm volatile("nop"); }
     *(volatile uint32_t *)GPPUDCLK0 = (1<<14)|(1<<15);
     r=150; while(r--) { asm volatile("nop"); }
     *(volatile uint32_t *)GPPUDCLK0 = 0;        // flush GPIO setup
 
     *(volatile uint32_t *)UART0_ICR = 0x7FF;    // clear interrupts
     *(volatile uint32_t *)UART0_IBRD = 2;       // 115200 baud
     *(volatile uint32_t *)UART0_FBRD = 0xB;
     *(volatile uint32_t *)UART0_LCRH = 0x7<<4;  // 8n1, enable FIFOs
     *(volatile uint32_t *)UART0_CR = 0x301;     // enable Tx, Rx, UART

     #endif
}

void uart_init()
{
	// Disable UART0.
	mmio_write(UART0_CR, 0x00000000);

	// Setup the GPIO pin 14 && 15 as UART (alt.fun.0, p.102 manual)
	// using a non destructive method (read first then write)
	uint32_t temp_reg;
	temp_reg = mmio_read(GPIO_GPFSE1L);
 	mmio_write(GPIO_GPFSE1L, (1 << 14) | (1 << 17) | temp_reg);
  
	// Clear pending interrupts.
	mmio_write(UART0_ICR, 0x7FF);
 
	// Set integer & fractional part of baud rate.
	// Divider = UART_CLOCK/(16 * Baud)
	// Fraction part register = (Fractional part * 64) + 0.5
	// UART_CLOCK = 3000000; Baud = 115200.
 
	// Divider = 3000000 / (16 * 115200) = 1.627 = ~1.
	// Fractional part register = (.627 * 64) + 0.5 = 40.6 = ~40.
	mmio_write(UART0_IBRD, 1);
	mmio_write(UART0_FBRD, 40);
 
	// Enable FIFO & 8 bit data transmissio (1 stop bit, no parity).
	mmio_write(UART0_LCRH, (1 << 4) | (1 << 5) | (1 << 6));
 
	// Mask all interrupts.
	mmio_write(UART0_IMSC, (1 << 1) | (1 << 4) | (1 << 5) | (1 << 6) |
	                       (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10));
 
	// Enable UART0, receive & transfer part of UART.
	mmio_write(UART0_CR, (1 << 0) | (1 << 8) | (1 << 9));
}
 
void uart_putc(unsigned char byte)
{
	// Wait for UART to become ready to transmit.
	while ( mmio_read(UART0_FR) & (1 << 5) ) { }
	mmio_write(UART0_DR, byte);
}
 
unsigned char uart_getc()
{
    // Wait for UART to have recieved something.
    while ( mmio_read(UART0_FR) & (1 << 4) ) { }
    return mmio_read(UART0_DR);
}
 
void uart_write(const unsigned char* buffer, size_t size)
{
	for ( size_t i = 0; i < size; i++ )
		uart_putc(buffer[i]);
}
 
void uart_puts(const char* str)
{
	uart_write((const unsigned char*) str, strlen(str));
}
 
#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif
void kernel_main(uint32_t r0, uint32_t r1, uint32_t atags)
{
	(void) r0;
	(void) r1;
	(void) atags;

	// Initialize pheripherals
	ok_led_init();
	uart_init();

	// Show that we are alive
	uint8_t data;
	data = 0;
	ok_init_write(data); // data = 0 turns the led ON

	
	// Print on uart
	uart_puts("Hello, kernel World!\r\n");

 
	while ( true ){
		// Every letter we get we switch the ok led on-off
		data = !data;
		ok_init_write(data);
		uart_putc(uart_getc());
	}
}
