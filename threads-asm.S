.globl restore_context
restore_context:
    cpsid i
    
    ldr r1, [r0, #64] @ load cpsr
    msr cpsr_c, r1
    
    bic r3, r1, #0x1F
    orr r3, r3, #0x13
    msr cpsr_c, r3
    ldr sp, [r0, #52] @ load sp and lr into super mode banked regs
    ldr lr, [r0, #56] 
    msr cpsr_c, r1     
    
    add r1, r0, #16
    ldmia r1, {r4-r11} @ load general regs
    
    ldr r12, [r0, #48] @ load caller regs
    ldr r3, [r0, #12]
    ldr r2, [r0, #8]
    ldr r1, [r0, #4]
    ldr lr, [r0, #60]  @ this lr is the program pc
    ldr r0, [r0, #0]
    
    mov pc, lr

.extern sched

.align 4
.globl interrupt_vector

interrupt_vector:
    cpsid i
    sub lr, lr, #4
    push {r0, r1, r2, r3, r12} @ push caller regs to we can use them

    push {lr}
    bl TIM_Check_IRQ
    tst r0, #1
    beq _not_timer
    pop {lr}
    
    push {lr}
    bl TIM_Clear_Pending
    pop {lr}

    ldr r0, =sched
    ldr r1, [r0, #4]            @ curr thread  
    ldr r2, [r0, #8]            @ threads
    ldr r2, [r2, r1, lsl #2]    @ threads[curr thread]
    add r0, r2, #4              @ threads[curr thread].ctx

    add r1, r0, #16
    stmia r1, {r4-r11}

    mrs r2, cpsr
    bic r3, r2, #0x1F
    orr r3, r3, #0x13           @ swap to super for banked regs
    msr cpsr_c, r3          
    str sp, [r0, #52]
    str lr, [r0, #56]
    msr cpsr_c, r2
    
    str lr, [r0, #60]           @ this is the pc of the code we interrupted
    mrs r1, spsr
    str r1, [r0, #64]           @ spsr contains the cpsr of the code we interrupted

    pop {r1, r2, r3}            @ pop r0-2 off the stack into r1-3 so we can keep r0 pointing at context
    str r1, [r0, #0]
    str r2, [r0, #4]
    str r3, [r0, #8]
    pop {r3, r12}               @ pop r3, r12
    str r3, [r0, #12]
    str r12, [r0, #48]

    ldr r0, =sched
    ldr r1, [r0, #4]            @ curr thread
    ldr r2, [r0, #0]            @ num threads
    add r1, r1, #1              @ increment and modulo thread
    cmp r1, r2
    movge r1, #0
    str r1, [r0, #4]            @ store new thread

    ldr r2, [r0, #8]            
    ldr r2, [r2, r1, lsl #2]    @ threads[new thread]
    add r0, r2, #4              @ new thread context
    
    mrs r2, cpsr
    bic r3, r2, #0x1F
    orr r3, r3, #0x13           @ swap to super mode
    msr cpsr_c, r3
    ldr sp, [r0, #52]           @ load banked regs
    ldr lr, [r0, #56]
    msr cpsr_c, r2

    ldr r1, [r0, #64]           @ load to spsr (movs takes care of putting it into cpsr on return)
    msr spsr_cxsf, r1
    
    add r1, r0, #16
    ldmia r1, {r4-r11}

    ldr lr, [r0, #60]
    ldr r12, [r0, #48]    
    ldr r3, [r0, #12]        
    ldr r2, [r0, #8]       
    ldr r1, [r0, #4]        
    ldr r0, [r0, #0]
    
    movs pc, lr

_not_timer:
    pop {lr}                    @ we have to do this since we pushed it before calling the irq check
    pop {r0, r1, r2, r3, r12}   @ we have to do this since we pushed it at start
    
    push {r0-r3, r12, lr}       @ as the caller save all regs
    bl c_interrupt_handler
    pop {r0-r3, r12, lr}
    
    movs pc, lr