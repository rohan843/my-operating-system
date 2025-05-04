/*
 * This file is a linker script written for the GNU Linker.
*/

# Tells the linker the `loader` label (present in loader.s) is the entry point into the code.
ENTRY(loader)

# Specifies the output format of the binary to be 32-bit ELF for x86 architecture.
OUTPUT_FORMAT(elf32-i386)

# Specifies the target architecture of the system to be 32-bit Intel x86.
OUTPUT_ARCH(i386:i386)

SECTIONS
{
    # Sets the starting location of the program to 0x0100000, i.e., 1 MiB onwards.
    . = 0x0100000;

    # Contains executable code and read-only data.
    .text :
    {
        *(.multiboot)
        *(.text*)
        *(.rodata)
    }

    # Contains initialized global and static variables.
    .data :
    {
        start_ctors = .;
        KEEP(*( .init_array ));
        KEEP(*(SORT_BY_INIT_PRIORITY( .init_array.* )));
        end_ctors = .;

        *(.data)
    }

    # Contains uninitialized global and static variables.
    .bss :
    {
        *(.bss)
    }

    /DISCARD/ :
    {
        *(.fini_array*)
        *(.comment)
    }
}
