#include "mouse.h"

MouseDriver::MouseDriver(InterruptManager *manager)
    : InterruptHandler(0x2C, manager), dataport(0x60), commandport(0x64)
{
    offset = 0;
    buttons = 0;

    uint16_t *VideoMemory = (uint16_t *)0xb8000;

    VideoMemory[80 * 12 + 40] = ((VideoMemory[80 * 12 + 40] & 0xF000) >> 4) |
                                ((VideoMemory[80 * 12 + 40] & 0x0F00) << 4) |
                                (VideoMemory[80 * 12 + 40] & 0x00FF);

    /**
     * Tests the mouse port of the PS/2 controller.
     */
    commandport.Write(0xAB);

    /**
     * Gets the current state of the mouse controller's status byte on the data port.
     */
    commandport.Write(0x20);

    /**
     * Stores the state of the PS/2 controller, and:
     *
     * 1. Sets the bit 1 to 1. This enables mouse interrupts (IRQ12).
     * 2. Sets bit 5 to 1. This enables the mouse clock.
     */
    uint8_t status = (dataport.Read() | 0b10) & (~0x20);

    /**
     * Change the status byte and write the new one to the controller.
     */
    commandport.Write(0x60);
    dataport.Write(status);

    /**
     * Tells the PS/2 to route the next byte sent to the data port to the mouse instead of the
     * keyboard.
     */
    commandport.Write(0xD4);

    /**
     * Enables data reporting by mouse.
     */
    dataport.Write(0xF4);
    /**
     * Reads and ignores the ACK from mouse.
     */
    dataport.Read();
}

MouseDriver::~MouseDriver() {}

uint32_t MouseDriver::HandleInterrupt(uint32_t esp)
{
    uint8_t status = commandport.Read();
    /**
     * Checks if the data came from the mouse or keyboard. If bit 5 is set, then it was the mouse.
     */
    if (!(status & 0x20))
    {
        /**
         * If there's no data from the mouse.
         */
        return esp;
    }

    /**
     * Initial location of the mouse pointer.
     */
    static int8_t x = 40, y = 12;

    buff[offset] = dataport.Read();
    offset = (offset + 1) % 3;

    /**
     * Each movement of the mouse generates a 3 byte packet. This goes to the PS/2 controller. The
     * controller generates a separate IRQ12 for each byte. Here, we are placing these bytes in the
     * buffer one by one.
     *
     * If we have all the 3 bytes and the buff array is full, we change the coordinates of the
     * pointer.
     */
    if (offset == 0)
    {
        uint16_t *VideoMemory = (uint16_t *)0xb8000;

        VideoMemory[80 * y + x] = ((VideoMemory[80 * y + x] & 0xF000) >> 4) |
                                  ((VideoMemory[80 * y + x] & 0x0F00) << 4) |
                                  (VideoMemory[80 * y + x] & 0x00FF);

        x += buff[1];
        if (x < 0)
        {
            x = 0;
        }
        if (x >= 80)
        {
            x = 79;
        }

        y -= buff[2];

        if (y < 0)
        {
            y = 0;
        }
        if (y >= 25)
        {
            y = 24;
        }

        VideoMemory[80 * y + x] = ((VideoMemory[80 * y + x] & 0xF000) >> 4) |
                                  ((VideoMemory[80 * y + x] & 0x0F00) << 4) |
                                  (VideoMemory[80 * y + x] & 0x00FF);

        for (uint8_t i = 0; i < 3; i++)
        {
            if ((buff[0] & (1 << i)) != (buttons & (1 << i)))
            {
                VideoMemory[80 * y + x] = ((VideoMemory[80 * y + x] & 0xF000) >> 4) |
                                          ((VideoMemory[80 * y + x] & 0x0F00) << 4) |
                                          (VideoMemory[80 * y + x] & 0x00FF);
            }
        }

        buttons = buff[0];
    }

    return esp;
}
