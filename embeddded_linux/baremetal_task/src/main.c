#include "../include/BMC2837.h"



static void delay(volatile unsigned int count)
{
    while (count--) {
        asm volatile("nop");
    }
}

int main(void)
{
    // We will blink GPIO17.
    // GPIO17 function select bits are in GPFSEL1 (bits 21..23).

    uint32_t sel;

    // Read current GPFSEL1
    sel = BCM2837_GET32(BCM2837_GPFSEL1);

    // Clear bits for GPIO17 (3 bits: 21,22,23)
    sel &= ~(0x7u << 21);

    // Set function to OUTPUT (001)
    sel |= (BCM2837_FSEL_OUTPUT << 21);

    // Write back
    BCM2837_PUT32(BCM2837_GPFSEL1, sel);

    // Main blink loop
    for (;;) {
        // Set GPIO17 high
        BCM2837_PUT32(BCM2837_GPSET0, (1u << 17));
        delay(500000);

        // Set GPIO17 low
        BCM2837_PUT32(BCM2837_GPCLR0, (1u << 17));
        delay(500000);
    }

    // We will never reach here, but keep return type happy
    return 0;
}