#include "keyboard.h"

void printf(char *);

KeyboardDriver::KeyboardDriver(InterruptManager *manager)
    : InterruptHandler(0x21, manager),
      dataport(0x60),
      commandport(0x64)
{
    /**
     * Flushes the keyboard controller's output buffer before the keyboard driver starts.
     *
     * The condition below checks the bit 0 of the command port. If it is set, that means data is
     * present in the keyboard's output buffer.
     */
    while (commandport.Read() & 0x1)
    {
        dataport.Read();
    }

    /**
     * Tells the keyboard controller to start sending data to the CPU (through the PIC interrupts).
     */
    commandport.Write(0xAE);

    /**
     * Gets the current state of the keyboard controller's status byte on the data port.
     */
    commandport.Write(0x20);

    /**
     * Stores the state of the PS/2 controller, and:
     *
     * 1. Sets the bit 0 to 1. This enables keyboard interrupts (IRQ1).
     * 2. Unsets the bit 4 to 0. This enables keyboard clock.
     */
    uint8_t status = (dataport.Read() | 0b1) & (~0x10);

    /**
     * Change the status byte and write the new one to the controller.
     */
    commandport.Write(0x60);
    dataport.Write(status);

    /**
     * Asks the keyboard to begin sending keypress scan codes.
     */
    dataport.Write(0xF4);
}

KeyboardDriver::~KeyboardDriver()
{
}

uint32_t KeyboardDriver::HandleInterrupt(uint32_t esp)
{
    uint8_t key = dataport.Read();

    if (key < 0x80)
    {
        /**
         * Codes greater then 0x80 are key release codes. Codes less than 0x80 are key press codes.
         * 
         * We're ignoring key releases for now.
         * 
         */
        switch (key)
        {
        /**
         * Ignoring Num lock.
         */
        case 0x45:
        case 0xC5:
            break;
        /**
         * Ignoring ACK messages.
         */
        case 0xFA:
            break;
        default:
            char *str = "\nKeyboard: 0x__";
            char *hex = "0123456789ABCDEF";
            str[13] = hex[(key >> 4) & 0xF];
            str[14] = hex[key & 0xF];
            printf(str);
            break;
        }
    }
    return esp;
}
