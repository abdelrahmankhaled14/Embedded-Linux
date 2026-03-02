// src/start.s
    .section .text.start
    .globl _start

_start:
    // Set up a simple stack pointer at _stack_top (defined in linker.ld)
    ldr x0, =_stack_top
    mov sp, x0

    // Clear bss (optional but good practice)
    ldr x1, =__bss_start
    ldr x2, =__bss_end

1:
    cmp x1, x2
    b.ge 2f
    str xzr, [x1], #8
    b 1b

2:
    // Call main()
    bl main

// If main ever returns, just hang
3:
    wfe
    b 3b