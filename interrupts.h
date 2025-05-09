#ifndef __INTERRUPTS_H
#define __INTERRUPTS_H

#include "types.h"
#include "port.h"
#include "gdt.h"

class InterruptManager;

class InterruptHandler
{
protected:
    uint8_t interruptNumber;
    InterruptManager *interruptManager;

    /**
     * @brief Construct a new Interrupt Handler object
     *
     * Made protected to ensure this class never gets instantiated.
     *
     */
    InterruptHandler(uint8_t interruptNumber, InterruptManager *interruptManager);
    ~InterruptHandler();

public:
    /**
     * @brief Handles the interrupt associated with an inherited object.
     *
     * @param esp The value of the stack pointer before the interrupt handling began.
     * @return The value of the stack pointer before the interrupt handling began.
     */
    uint32_t HandleInterrupt(uint32_t esp);
};

/**
 * @brief The Interrupt Manager class.
 *
 * This class specifies an object containing the interrupt descriptor table and various helpful
 * methods.
 *
 * This class must be instantiated once. In case of multiple instances, all will share the IDT and
 * the object activated last will be pointed to by `ActiveInterruptManager`.
 *
 */
class InterruptManager
{
    friend class InterruptHandler;

protected:
    /**
     * Points to the (single) Interrupt Manager object created. (Currently active object in case
     * multiple such objects got created.)
     */
    static InterruptManager *ActiveInterruptManager;

    InterruptHandler* handlers[256];

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
     * @param DescriptorPriveledgeLevel Goes from 0 to 3. Lower = more priveledge, 0 for kernel.
     * @param DescriptorType Type of gate (interrupt = 0xE, trap = 0xF, etc.)
     */
    static void SetInterruptDescriptorTableEntry(
        uint8_t interruptNumber,
        uint16_t codeSegmentSelectorOffset,
        void (*handler)(),
        uint8_t DescriptorPriveledgeLevel,
        uint8_t DescriptorType);

    Port8BitSlow picMasterCommand;
    Port8BitSlow picMasterData;
    Port8BitSlow picSlaveCommand;
    Port8BitSlow picSlaveData;

public:
    InterruptManager(GlobalDescriptorTable *gdt);
    ~InterruptManager();

    /**
     * @brief Tells the processor to begin processing interrupts.
     */
    void Activate();

    /**
     * @brief Stops interrupts from being processed.
     */
    void Deactivate();

    /**
     * @brief A static method that handles interrupts. Calls the `DoHandleInterrupt` method of the
     * currently active interrupt manager object.
     *
     * Is is important to have this as the assembly code calls this function, but this function
     * can't access the port objects as they aren't static.
     *
     * @param interruptNumber The number of the interrupt that occurred.
     * @param esp The current stack pointer.
     * @return The stack pointer as it was before the handler was called.
     */
    static uint32_t handleInterrupt(uint8_t interruptNumber, uint32_t esp);

    /**
     * @brief A non-static method handling interrupts.
     *
     * @param interruptNumber
     * @param esp
     * @return uint32_t
     */
    uint32_t DoHandleInterrupt(uint8_t interruptNumber, uint32_t esp);

    /**
     * @brief The timer interrupt handler.
     * This is defined in assembly in the file "interruptstubs.s"
     */
    static void HandleInterruptRequest0x00();

    /**
     * @brief The keyboard interrupt handler.
     *
     * This is defined in assembly in the file "interruptstubs.s"
     */
    static void HandleInterruptRequest0x01();

    /**
     * @brief Ignores a given interrupt.
     *
     * This is defined in assembly in the file "interruptstubs.s"
     */
    static void IgnoreInterruptRequest();
};

#endif