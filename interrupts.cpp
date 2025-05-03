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
        IDT_DESC_PRESENT | DescriptorType | ((DescriptorPriveledgeLevel & 0b11) << 5);
}

InterruptManager::InterruptManager(GlobalDescriptorTable *gdt)
{
    uint16_t CodeSegment = gdt->CodeSegmentSelector();
    const uint8_t IDT_INTERRUPT_GATE = 0xE;

    /**
     * Setting up initially that all interrupts be ignored.
     */
    for (uint8_t i = 0; i < 256; i++)
    {
        this->SetInterruptDescriptorTableEntry(
            i,
            CodeSegment,
            &this->IgnoreInterruptRequest,
            0,
            IDT_INTERRUPT_GATE);
    }

    /**
     * Timer interrupt.
     */
    this->SetInterruptDescriptorTableEntry(
        0x20,
        CodeSegment,
        &this->HandleInterruptRequest0x00,
        0,
        IDT_INTERRUPT_GATE);

    /**
     * Keyboard interrupt.
     */
    this->SetInterruptDescriptorTableEntry(
        0x21,
        CodeSegment,
        &this->HandleInterruptRequest0x01,
        0,
        IDT_INTERRUPT_GATE);

    InterruptDescriptorTablePointer idt;

    // Why "-1"?: See comment of InterruptDescriptorTablePointer::size
    idt.size = 256 * sizeof(GateDescriptor) - 1;

    idt.base = (uint32_t)interruptDescriptorTable;

    asm volatile("lidt %0" : : "m"(idt));
}

InterruptManager::~InterruptManager()
{
}

void InterruptManager::Activate()
{
    asm volatile("sti"); // "Start Interrupts"
}

uint32_t InterruptManager::handleInterrupt(uint8_t interruptNumber, uint32_t esp)
{
    printf(" Interrupt!");

    return esp; // for now.
}
