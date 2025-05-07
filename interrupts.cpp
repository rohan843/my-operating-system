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

    /**
     * Sets bit 7 in `access` flag to 1. This is the `Present Bit`, which must be `1` for a
     * valid entry.
     */
    const uint8_t IDT_DESC_PRESENT = 0x80;

    interruptDescriptorTable[interruptNumber].handlerAddressLowBits = ((uint32_t)handler) & 0xFFFF;
    interruptDescriptorTable[interruptNumber].handlerAddressHighBits =
        (((uint32_t)handler) >> 16) & 0xFFFF;
    interruptDescriptorTable[interruptNumber].gdt_codeSegmentSelector = codeSegmentSelectorOffset;
    interruptDescriptorTable[interruptNumber].reserved = 0;
    interruptDescriptorTable[interruptNumber].access =
        IDT_DESC_PRESENT | DescriptorType | ((DescriptorPriveledgeLevel & 0b11) << 5);
}

InterruptManager::InterruptManager(GlobalDescriptorTable *gdt) : picMasterCommand(0x20),
                                                                 picMasterData(0x21),
                                                                 picSlaveCommand(0xA0),
                                                                 picSlaveData(0xA1)
{
    uint16_t CodeSegment = gdt->CodeSegmentSelector();
    /**
     * Flag marking a gate as an interrupt gate.
     */
    const uint8_t IDT_INTERRUPT_GATE = 0xE;

    /**
     * Setting up initially that all interrupts be ignored.
     */
    for (uint16_t i = 0; i < 256; i++)
    {
        this->SetInterruptDescriptorTableEntry(
            (uint8_t)i,
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

    /**
     * Initializes the 2 PICs to operate in cascade mode. They will expect 3 more control words (
     * sent below).
     */
    picMasterCommand.Write(0x11);
    picSlaveCommand.Write(0x11);

    /**
     * Telling the master PIC - 0x20 should be added to any interrupt. (8 initial hardware
     * interrupts come from the master PIC.)
     */
    picMasterData.Write(0x20);
    /**
     * Telling the slave PIC - 0x28 should be added to any interrupt. (8 final hardware interrupts
     * come from the slave PIC.)
     */
    picSlaveData.Write(0x28);

    /**
     * The following 2 lines initialize the master as the master and the slave as the slave.
     */
    picMasterData.Write(0x04);
    picSlaveData.Write(0x02);

    /**
     * Sets the interrupts to the 8086/88 mode.
     */
    picMasterData.Write(0x01);
    picSlaveData.Write(0x01);

    /**
     * Unmasks all IRQ lines, enabling all interrupts.
     */
    picMasterData.Write(0x00);
    picSlaveData.Write(0x00);

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
