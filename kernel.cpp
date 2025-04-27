#include "types.h"
#include "gdt.h"

/**
 * @brief Prints a string to the display.
 *
 * This function places the charecters of a provided string directly into the VGA Video Memory
 * location (0xb8000). Fills the screen top to down, left to right. If the entire screen is filled,
 * next characterss will be printed at the top of the screen, after clearing the screen.
 *
 * @param str Pointer to the string being printed.
 */
void printf(char *str)
{
    /**
     * @brief The memory location where the VGA display expects data to be kept.
     *
     * The dimensions of the character grid are 80 columns and 25 rows.
     *
     */
    uint16_t *VideoMemory = (uint16_t *)0xb8000;

    /**
     * @brief A cursor telling which row/column we're on at the moment.
     */
    static uint8_t x = 0, y = 0;

    for (int i = 0; str[i] != '\0'; i++)
    {
        if (str[i] != '\n')
        {
            VideoMemory[80 * y + x] = (VideoMemory[80 * y + x] & 0xFF00) | str[i];
            x++;
            if (x == 80)
            {
                /**
                 * If the full row is filled, we go to the next line.
                 */
                y++;
                x = 0;
            }
        }
        else
        {
            /**
             * If '\n' encountered, we go to the next line.
             */
            y++;
        }
        if (y == 25)
        {
            /**
             * If all rows are filled, we clear the screen and go back to the top.
             */

            for (x = 0; x < 80; x++)
            {
                for (y = 0; y < 25; y++)
                {
                    VideoMemory[80 * y + x] = (VideoMemory[80 * y + x] & 0xFF00) | ' ';
                }
            }

            x = 0;
            y = 0;
        }
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

    printf("Code runs like a flowing stream,\n");
    printf("Bits and bytes weave the dream,\n");
    printf("Logic dances, circuits gleam,\n");
    printf("Kernel wakes in a silent scream.\n");
    printf("~ Copilot\n");
    printf("Run #1\n");

    GlobalDescriptorTable gdt;

    while (1)
        ;
}