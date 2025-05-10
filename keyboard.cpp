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

char us_qwerty_scancode_to_char(uint8_t scancode)
{
    switch (scancode)
    {
    case 0x02:
        return '1';
    case 0x03:
        return '2';
    case 0x04:
        return '3';
    case 0x05:
        return '4';
    case 0x06:
        return '5';
    case 0x07:
        return '6';
    case 0x08:
        return '7';
    case 0x09:
        return '8';
    case 0x0A:
        return '9';
    case 0x0B:
        return '0';
    case 0x0C:
        return '-';
    case 0x0D:
        return '=';
    case 0x0E:
        return '\b'; // Backspace
    case 0x0F:
        return '\t'; // Tab
    case 0x10:
        return 'q';
    case 0x11:
        return 'w';
    case 0x12:
        return 'e';
    case 0x13:
        return 'r';
    case 0x14:
        return 't';
    case 0x15:
        return 'y';
    case 0x16:
        return 'u';
    case 0x17:
        return 'i';
    case 0x18:
        return 'o';
    case 0x19:
        return 'p';
    case 0x1A:
        return '[';
    case 0x1B:
        return ']';
    case 0x1C:
        return '\n'; // Enter
    case 0x1E:
        return 'a';
    case 0x1F:
        return 's';
    case 0x20:
        return 'd';
    case 0x21:
        return 'f';
    case 0x22:
        return 'g';
    case 0x23:
        return 'h';
    case 0x24:
        return 'j';
    case 0x25:
        return 'k';
    case 0x26:
        return 'l';
    case 0x27:
        return ';';
    case 0x28:
        return '\'';
    case 0x29:
        return '`';
    case 0x2C:
        return 'z';
    case 0x2D:
        return 'x';
    case 0x2E:
        return 'c';
    case 0x2F:
        return 'v';
    case 0x30:
        return 'b';
    case 0x31:
        return 'n';
    case 0x32:
        return 'm';
    case 0x33:
        return ',';
    case 0x34:
        return '.';
    case 0x35:
        return '/';
    case 0x39:
        return ' '; // Space
    default:
        return '\0';
    }
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
            break;
        /**
         * Ignoring ACK messages.
         */
        case 0xFA:
            break;
        default:
            char result = us_qwerty_scancode_to_char(key);
            if (result != '\0')
            {
                char *str = " ";
                str[0] = result;
                printf(str);
            }
            else
            {
                char *str = "\nKeyboard: 0x__";
                char *hex = "0123456789ABCDEF";
                str[13] = hex[(key >> 4) & 0xF];
                str[14] = hex[key & 0xF];
                printf(str);
            }
            break;
        }
    }
    return esp;
}
