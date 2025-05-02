#ifndef __INTERRUPTS_H
#define __INTERRUPTS_H

#include "types.h"
#include "port.h"

class InterruptManager
{
public:
    /**
     * @brief
     *
     * @param interruptNumber The number of the interrupt that occurred.
     * @param esp The current stack pointer.
     * @return uint32_t
     */
    static uint32_t handleInterrupt(uint8_t interruptNumber, uint32_t esp);
};

#endif