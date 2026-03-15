.globl _start
_start:
    ldr pc, _reset_h
    ldr pc, _undefined_instruction_vector_h
    ldr pc, _software_interrupt_vector_h
    ldr pc, _prefetch_abort_vector_h
    ldr pc, _data_abort_vector_h
    ldr pc, _unused_handler_h
    ldr pc, _interrupt_vector_h
    ldr pc, _fast_interrupt_vector_h

_reset_h:                           .word   _reset_
_undefined_instruction_vector_h:    .word   undefined_instruction_vector
_software_interrupt_vector_h:       .word   software_interrupt_vector
_prefetch_abort_vector_h:           .word   prefetch_abort_vector
_data_abort_vector_h:               .word   data_abort_vector
_unused_handler_h:                  .word   _reset_
_interrupt_vector_h:                .word   interrupt_vector
_fast_interrupt_vector_h:           .word   fast_interrupt_vector

_reset_:
    cpsid i

    @ 768 entry page table
    ldr r0, =page_table

    @ map 0x0 to 0x1FFFFFFF 512 MB of cached RAM
    mov r1, #0 @ address; section base address in page table
    ldr r3, =0xC0A
_loop1:
    orr r2, r1, r3 @ address or-ed with ap=11 and cache bit
    str r2, [r0], #4   @ go to the next entry
    add r1, r1, #(1 << 20) @ up by 1 mb
    cmp r1, #0x1FFFFFFF
    blt _loop1

    @ don't cache peripherals; 256 mb of peripheral memory
    ldr r3, =0xC02
_loop2:
    orr r2, r1, r3 @ ap=11, no caching
    str r2, [r0], #4
    add r1, r1, #(1 << 20)
    cmp r1, #0x2FFFFFFF
    blt _loop2

    @ set domain 0 perms to 11 
    mov r0, #3
    mcr p15, 0, r0, c3, c0, 0

    @ set tlb base
    ldr r0, =page_table
    mcr p15, 0, r0, c2, c0, 0

    @ invalidate icache, tlb
    mov r0, #0
    mcr p15, 0, r0, c8, c7, 0   @ invalidate tlb
    mcr p15, 0, r0, c7, c5, 0   @ invalidate icache

    @ enable mmu, i and d cache, branch prediction
    mrc p15, 0, r0, c1, c0, 0 @ rmw
    ldr r3, =0x1805
    orr r0, r0, r3
    mcr p15, 0, r0, c1, c0, 0

    mov sp, #0x8000000 

    mov r0, #0x8000
    mov r1, #0x0000
    ldmia r0!, {r2, r3, r4, r5, r6, r7, r8, r9}
    stmia r1!, {r2, r3, r4, r5, r6, r7, r8, r9}
    ldmia r0!, {r2, r3, r4, r5, r6, r7, r8, r9}
    stmia r1!, {r2, r3, r4, r5, r6, r7, r8, r9}

    @ set IRQ stack
    mrs r0, cpsr
    bic r0, r0, #0x1F
    orr r0, r0, #0x12
    msr cpsr_c, r0
    mov sp, #0x6000000

    bic r0, r0, #0x1F
    orr r0, r0, #0x13
    msr cpsr_c, r0

    bl main

.section .bss
.align 14 @ 16KB alignment required for page table
page_table:
.space 3072 @ 768 entries - 512 of memory and 256 of periph