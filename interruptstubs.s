.section .text

.extern _ZN16InterruptManager15handleInterruptEhj ; // Comes from `nm interrupts.o`


.macro HandleInterruptRequest num
.global _ZN16InterruptManager26handleInterruptRequest\num\()Ev
    movb $\num, (interruptnumber)
    jmp int_bottom
.endm


int_bottom:
    pusha
    pushl %ds
    pushl %es
    pushl %fs
    pushl %gs

    pushl %esp
    push (interruptnumber)
    call _ZN16InterruptManager15handleInterruptEhj
    # Normally, we would restore the stack pointer here, but we don't need to
    # because we are going to overwrite it below.
    movl %eax, %esp

    popl %gs
    popl %fs
    popl %es
    popl %ds
    popa

    iret

.data
interruptnumber: .byte 0

