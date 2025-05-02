#include "interrupts.h"

void printf(char *str);

InterruptManager::GateDescriptor InterruptManager::interruptDescriptorTable[256];

void InterruptManager::SetInterruptDescriptorTableEntry(
    uint8_t interruptNumber,
    uint16_t codeSegmentSelectorOffset,
    void (*handler)(),
    uint8_t DescriptorPriveledgeLevel,
    uint8_t DescriptorType)
{

    const uint8_t IDT_DESC_PRESENT = 0x80;

    interruptDescriptorTable[interruptNumber].handlerAddressLowBits = ((uint32_t)handler) & 0xFFFF;
    interruptDescriptorTable[interruptNumber].handlerAddressHighBits =
        (((uint32_t)handler) >> 16) & 0xFFFF;
    interruptDescriptorTable[interruptNumber].gdt_codeSegmentSelector = codeSegmentSelectorOffset;
    interruptDescriptorTable[interruptNumber].reserved = 0;
    interruptDescriptorTable[interruptNumber].access =
        IDT_DESC_PRESENT | DescriptorType | DescriptorPriveledgeLevel;
}

InterruptManager::InterruptManager(GlobalDescriptorTable *gdt)
{
}

InterruptManager::~InterruptManager()
{
}

uint32_t InterruptManager::handleInterrupt(uint8_t interruptNumber, uint32_t esp)
{
    printf(" Interrupt!");

    return esp; // for now.
}
