/**
 * @file keyboard.h
 * @author rohan843
 * @brief Contains the driver for the keyboard.
 * 
 * This driver will setup the keyboard in such a manner that any byte sent to the data port of the
 * keyboard will interrupt the CPU. It is the handler's responsibility to appropriately interpret
 * the byte. (For e.g., the byte could be a key scan code, an acknowledgement from the keyboard,
 * or something else.)
 * 
 */

#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include "types.h"
#include "interrupts.h"
#include "port.h"

class KeyboardDriver : public InterruptHandler
{
    Port8Bit dataport;
    Port8Bit commandport;

public:
    KeyboardDriver(InterruptManager *manager);
    ~KeyboardDriver();
    virtual uint32_t HandleInterrupt(uint32_t esp);
};

#endif