.globl restore_context
restore_context:
    cpsid i
    
    ldr r1, [r0, #64]  
    msr cpsr_c, r1
    
    bic r3, r1, #0x1F
    orr r3, r3, #0x13
    msr cpsr_c, r3
    ldr sp, [r0, #52]
    ldr lr, [r0, #56] 
    msr cpsr_c, r1     
    
    add r1, r0, #16
    ldmia r1, {r4-r11}
    
    ldr r12, [r0, #48]      
    ldr r3, [r0, #12]        
    ldr r2, [r0, #8]       
    ldr r1, [r0, #4]        
    ldr lr, [r0, #60] 
    ldr r0, [r0, #0]       
    
    mov pc, lr

.extern sched

.align 4
.globl interrupt_vector

interrupt_vector:
    cpsid i
    sub lr, lr, #4
    push {r0, r1, r2, r3, r12}
    push {lr}
    bl TIM_Check_IRQ
    
    mov r1, r0
    mov r0, r1
    
    tst r0, #1
    beq _not_timer
    pop {lr}
    
    push {lr}
    bl TIM_Clear_Pending
    pop {lr}

    ldr r0, =sched
    ldr r1, [r0, #4]
    ldr r2, [r0, #8]
    ldr r2, [r2, r1, lsl #2]
    add r0, r2, #4

    add r1, r0, #16
    stmia r1, {r4-r11}

    mrs r2, cpsr
    bic r3, r2, #0x1F
    orr r3, r3, #0x13
    msr cpsr_c, r3          
    str sp, [r0, #52]
    str lr, [r0, #56]
    msr cpsr_c, r2
    
    str lr, [r0, #60]
    mrs r1, spsr
    str r1, [r0, #64]

    pop {r1, r2, r3}
    str r1, [r0, #0]
    str r2, [r0, #4]
    str r3, [r0, #8]
    pop {r3, r12}
    str r3, [r0, #12]
    str r12, [r0, #48]

    ldr r0, =sched
    ldr r1, [r0, #4]
    ldr r2, [r0, #0]
    add r1, r1, #1
    cmp r1, r2
    movge r1, #0
    str r1, [r0, #4]

    ldr r2, [r0, #8]
    ldr r2, [r2, r1, lsl #2]
    add r0, r2, #4
    
    mrs r2, cpsr
    bic r3, r2, #0x1F
    orr r3, r3, #0x13
    msr cpsr_c, r3
    
    ldr sp, [r0, #52]
    ldr lr, [r0, #56]
    
    msr cpsr_c, r2

    ldr r1, [r0, #64]
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
    pop {lr}  
    pop {r0, r1, r2, r3, r12}
    
    push {r0-r3, r12, lr}
    
    bl c_interrupt_handler
    
    pop {r0-r3, r12, lr}
    movs pc, lr