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
    while(commandport.Read() & 0x1)
    {
        dataport.Read();
    }

    /**
     * Tells the keyboard controller to start sending data to the CPU (through the PIC interrupts).
     */
    commandport.Write(0xAE);
}

KeyboardDriver::~KeyboardDriver()
{
}

uint32_t KeyboardDriver::HandleInterrupt(uint32_t esp)
{
    uint8_t key = dataport.Read();
    char *str = "\nKeyboard: 0x__";
    char *hex = "0123456789ABCDEF";
    str[13] = hex[(key >> 4) & 0xF];
    str[14] = hex[key & 0xF];
    printf(str);
    return esp;
}
