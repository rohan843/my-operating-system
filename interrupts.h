#ifndef __INTERRUPTS_H
#define __INTERRUPTS_H

#include "types.h"
#include "port.h"
#include "gdt.h"

class InterruptManager
{
protected:
    /**
     * @brief An entry of the interrupt descriptor table.
     *
     * This is an 8 byte object that describes the address of the interrupt handler, the relevant
     * code segment's offset in the GDT and access rights of the handler.
     *
     * Gate Descriptor is a common name for such an entry.
     */
    struct GateDescriptor
    {
        uint16_t handlerAddressLowBits;
        uint16_t gdt_codeSegmentSelector;
        uint8_t reserved;
        uint8_t access;
        uint16_t handlerAddressHighBits;
    } __attribute__((packed));

    /**
     * @brief A 6 - byte structure pointing to the IDT.
     *
     */
    struct InterruptDescriptorTablePointer
    {
        /**
         * @brief The byte offset of the last byte in IDT. (Calculated as size - 1.)
         */
        uint16_t size;

        /**
         * @brief The base address of the table.
         */
        uint32_t base;
    } __attribute__((packed));

    /**
     * @brief The interrupt descriptor table array.
     *
     * This array contains pointers to interrupt handlers for the 256 interrupts (32 exceptions,
     * 16 hardware interrupts and 208 software interrupts).
     *
     */
    static GateDescriptor interruptDescriptorTable[256];

    /**
     * @brief Set the Interrupt Descriptor Table Entry for an interrupt.
     *
     * @param interruptNumber The number of the interrupt (0-255)
     * @param codeSegmentSelectorOffset The offset in number of bytes of the code segment descriptor
     * in the GDT.
     * @param handler Pointer to the interrupt handler function.
     * @param DescriptorPriveledgeLevel Flag
     * @param DescriptorType Flag
     */
    static void SetInterruptDescriptorTableEntry(
        uint8_t interruptNumber,
        uint16_t codeSegmentSelectorOffset,
        void (*handler)(),
        uint8_t DescriptorPriveledgeLevel,
        uint8_t DescriptorType);

public:
    InterruptManager(GlobalDescriptorTable *gdt);
    ~InterruptManager();

    /**
     * @brief Tells the processor to begin processing interrupts.
     */
    void Activate();

    /**
     * @brief
     *
     * @param interruptNumber The number of the interrupt that occurred.
     * @param esp The current stack pointer.
     * @return uint32_t
     */
    static uint32_t handleInterrupt(uint8_t interruptNumber, uint32_t esp);

    /**
     * @brief The timer interrupt handler.
     */
    static void HandleInterruptRequest0x00();

    /**
     * @brief The keyboard interrupt handler.
     */
    static void HandleInterruptRequest0x01();

    static void IgnoreInterruptRequest();
};

#endif