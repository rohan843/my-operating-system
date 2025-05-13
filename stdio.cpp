#include "stdio.h"

void printf(const char *str)
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
            x = 0;
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
