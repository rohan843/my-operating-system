#include "types.h"
#include "gdt.h"

/**
 * @brief Prints a string to the display.
 * 
 * This function places the charecters of a provided string directly into the VGA Video Memory
 * location (0xb8000). Repetitive invocations of this function will lead to display being
 *  overwritten.
 * 
 * @param str Pointer to the string being printed.
 */
void printf(char *str)
{
    uint16_t *VideoMemory = (uint16_t *)0xb8000;
    for (int i = 0; str[i] != '\0'; i++)
    {
        VideoMemory[i] = (VideoMemory[i] & 0xFF00) | str[i];
    }
}

/**
 * This piece of code runs all the C++ constructors.
 */
typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors()
{
    for (constructor *c = &start_ctors; c != &end_ctors; c++)
    {
        (*c)();
    }
}

/**
 * @brief The main kernel function. This is the entry point into the OS program.
 */
extern "C" void kernelMain(const void *multiboot_structure, uint32_t magicnumber)
{
    printf("Code runs like a flowing stream, Bits and bytes weave the dream, Logic dances, circuits gleam, Kernel wakes in a silent scream. ~ Copilot");

    GlobalDescriptorTable gdt;

    while (1)
        ;
}