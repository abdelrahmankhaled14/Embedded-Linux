/* src/linker.ld */
ENTRY(_start)

SECTIONS
{
    /* RPi3 loads kernel8.img at 0x80000 */
    . = 0x80000;

    .text : {
        KEEP(*(.text.start))
        *(.text*)
    }

    .rodata : {
        *(.rodata*)
    }

    .data : {
        *(.data*)
    }

    .bss (NOLOAD) : {
        __bss_start = .;
        *(.bss*)
        *(COMMON)
        __bss_end = .;
    }

    _end = .;

    /* Simple stack: 16 KB above end of bss */
    _stack_top = . + 0x4000;
}