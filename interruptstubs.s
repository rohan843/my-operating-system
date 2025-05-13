# There are three kinds of interrupts:
# 1. Exceptions (0 to 31) - Created by the CPU, hardcoded
# 2. Hardware Interrupts (32 to 47) - Created by the PIC
# 3. Software Interrupts (48 to 255) - Created by code, typically by the OS

# The following variable essentially says IRQs (interrupt requests) begin 0x20, or, 32 onwards.
.set IRQ_BASE, 0x20

.section .text

.extern _ZN16InterruptManager15handleInterruptEhj # Comes from `nm interrupts.o`

.macro HandleException num
.global _ZN16InterruptManager16handleException\num\()Ev
_ZN16InterruptManager16handleException\num\()Ev:
    movb $\num, (interruptnumber)
    jmp int_bottom
.endm

.macro HandleInterruptRequest num
.global _ZN16InterruptManager26HandleInterruptRequest\num\()Ev
_ZN16InterruptManager26HandleInterruptRequest\num\()Ev:
    movb $\num + IRQ_BASE, (interruptnumber)
    jmp int_bottom
.endm

HandleInterruptRequest 0x00
HandleInterruptRequest 0x01
HandleInterruptRequest 0x0C

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

# .extern <symbol> -> This symbol is defined elsewhere.
# .global <symbol> -> This symbol is defined here.
.global _ZN16InterruptManager22IgnoreInterruptRequestEv
_ZN16InterruptManager22IgnoreInterruptRequestEv:
    iret

.data
interruptnumber: .byte 0

