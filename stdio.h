#ifndef __STDIO_H
#define __STDIO_H

#include "types.h"

/**
 * @brief Prints a string to the display.
 *
 * This function places the charecters of a provided string directly into the VGA Video Memory
 * location (0xb8000). Fills the screen top to down, left to right. If the entire screen is filled,
 * next characterss will be printed at the top of the screen, after clearing the screen.
 *
 * @param str Pointer to the string being printed.
 */
void printf(const char *str);

#endif