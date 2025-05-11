#include "types.h"
#include "gdt.h"
#include "interrupts.h"
#include "keyboard.h"
#include "mouse.h"
#include "stdio.h"

/**
 * This piece of code runs all the C++ constructors for any global or static objects.
 *
 * start_ctors and end_ctors are defined as labels in the linker.ld file.
 */
typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;

/**
 * @brief Runs all the constuctors for initialized global or static objects.
 */
extern "C" void callConstructors()
{
    /**
     * `c` is a pointer to a function pointer. `c` points into an array of function pointers, each
     * of which points to a constuctor. `c` causes these constructors to execute, based on the code
     * block in the loop, thereby initializing the global and static objects.
     */
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

    printf("Code runs like a flowing stream,\n");
    printf("Bits and bytes weave the dream,\n");
    printf("Logic dances, circuits gleam,\n");
    printf("Kernel wakes in a silent scream.\n");
    printf("~ Copilot\n");
    printf("Run #1\n");

    GlobalDescriptorTable gdt;
    InterruptManager interrupts(&gdt);
    KeyboardDriver keyboard(&interrupts);
    MouseDriver mouse(&interrupts);

    // Begin processing interrupts, once the hardware has been initialized above.
    interrupts.Activate();

    while (1)
        ;
}