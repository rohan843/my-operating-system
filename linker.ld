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
        # This line sets start_ctors to be a label pointing to the current memory location.
        start_ctors = .;

        # The following `init_array` sections and sub-sections contain an _array_ of pointers,
        # pointing to relevant constructors.

        # This line ensures all the `init_array` sections are preserved.
        KEEP(*( .init_array ));

        # This line ensures all the sub-sections under the `init_array` sections are preserved, with
        # any pre-defined priority order.
        KEEP(*(SORT_BY_INIT_PRIORITY( .init_array.* )));

        # This line sets end_ctors to be a label pointing to the current memory location.
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
