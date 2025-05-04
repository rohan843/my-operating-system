.set MAGIC, 0x1badb002
.set FLAGS, (1 << 0 | 1 << 1)
.set CHECKSUM, -(MAGIC + FLAGS)

.section .multiboot
    .long MAGIC
    .long FLAGS
    .long CHECKSUM

.section .text
.extern kernelMain
.extern callConstructors
.global loader

loader:
    # Initialize the stack pointer to the top of the kernel stack.
    mov $kernel_stack, %esp

    call callConstructors

    push %eax
    push %ebx
    call kernelMain

_stop:
    cli
    hlt
    jmp _stop

# The following bss section is used for the kernel stack. It specifies a 2 MiB space in memory, with
# the label `kernel_stack` pointing to the top of the (currently empty) stack. As the stack grows,
# the stack pointer (esp) will move towards lower memory addresses, i.e., it will be decremented
# with each `push` instruction.
.section .bss
.space 2*1024*1024; # 2 MiB
kernel_stack:
