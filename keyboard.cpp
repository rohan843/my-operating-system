#include "keyboard.h"

#include "stdio.h"

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
    static bool shift = false;
    static bool capslock = false;

    if (scancode == 0x2A || scancode == 0x36)
    {
        shift = true;
        return '\0';
    }
    else if (scancode == 0xAA || scancode == 0xB6)
    {
        shift = false;
        return '\0';
    }

    if (scancode == 0x3A)
    {
        capslock = !capslock;
        return '\0';
    }

    bool uppercase = shift xor capslock;

    switch (scancode)
    {
    case 0x02:
        return shift ? '!' : '1';
    case 0x03:
        return shift ? '@' : '2';
    case 0x04:
        return shift ? '#' : '3';
    case 0x05:
        return shift ? '$' : '4';
    case 0x06:
        return shift ? '%' : '5';
    case 0x07:
        return shift ? '^' : '6';
    case 0x08:
        return shift ? '&' : '7';
    case 0x09:
        return shift ? '*' : '8';
    case 0x0A:
        return shift ? '(' : '9';
    case 0x0B:
        return shift ? ')' : '0';
    case 0x0C:
        return shift ? '_' : '-';
    case 0x0D:
        return shift ? '+' : '=';
    case 0x0E:
        return '\b'; // Backspace
    case 0x0F:
        return '\t'; // Tab
    case 0x10:
        return uppercase ? 'Q' : 'q';
    case 0x11:
        return uppercase ? 'W' : 'w';
    case 0x12:
        return uppercase ? 'E' : 'e';
    case 0x13:
        return uppercase ? 'R' : 'r';
    case 0x14:
        return uppercase ? 'T' : 't';
    case 0x15:
        return uppercase ? 'Y' : 'y';
    case 0x16:
        return uppercase ? 'U' : 'u';
    case 0x17:
        return uppercase ? 'I' : 'i';
    case 0x18:
        return uppercase ? 'O' : 'o';
    case 0x19:
        return uppercase ? 'P' : 'p';
    case 0x1A:
        return shift ? '{' : '[';
    case 0x1B:
        return shift ? '}' : ']';
    case 0x1C:
        return '\n'; // Enter
    case 0x1E:
        return uppercase ? 'A' : 'a';
    case 0x1F:
        return uppercase ? 'S' : 's';
    case 0x20:
        return uppercase ? 'D' : 'd';
    case 0x21:
        return uppercase ? 'F' : 'f';
    case 0x22:
        return uppercase ? 'G' : 'g';
    case 0x23:
        return uppercase ? 'H' : 'h';
    case 0x24:
        return uppercase ? 'J' : 'j';
    case 0x25:
        return uppercase ? 'K' : 'k';
    case 0x26:
        return uppercase ? 'L' : 'l';
    case 0x27:
        return shift ? ':' : ';';
    case 0x28:
        return shift ? '"' : '\'';
    case 0x29:
        return shift ? '~' : '`';
    case 0x2C:
        return uppercase ? 'Z' : 'z';
    case 0x2D:
        return uppercase ? 'X' : 'x';
    case 0x2E:
        return uppercase ? 'C' : 'c';
    case 0x2F:
        return uppercase ? 'V' : 'v';
    case 0x30:
        return uppercase ? 'B' : 'b';
    case 0x31:
        return uppercase ? 'N' : 'n';
    case 0x32:
        return uppercase ? 'M' : 'm';
    case 0x33:
        return shift ? '<' : ',';
    case 0x34:
        return shift ? '>' : '.';
    case 0x35:
        return shift ? '?' : '/';
    case 0x39:
        return ' '; // Space
    default:
        return '\0';
    }
}

uint32_t KeyboardDriver::HandleInterrupt(uint32_t esp)
{
    uint8_t key = dataport.Read();

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
        // else
        // {
        //     char *str = "\nKeyboard: 0x__";
        //     char *hex = "0123456789ABCDEF";
        //     str[13] = hex[(key >> 4) & 0xF];
        //     str[14] = hex[key & 0xF];
        //     printf(str);
        // }
        break;
    }

    return esp;
}
