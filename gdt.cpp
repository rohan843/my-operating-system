#include "gdt.h"

GlobalDescriptorTable::GlobalDescriptorTable()
    : nullSegmentSelector(0, 0, 0), unusedSegmentSelector(0, 0, 0),
      codeSegmentSelector(0, 64 * 1024 * 1024, 0x9A), dataSegmentSelector(0, 64 * 1024 * 1024, 0x92)
{
    /**
     * @brief A 6-byte struct containing the byte vector to be loaded into the GDTR.
     */
    struct GDTPointer
    {
        uint16_t limit;
        uint32_t base;
    } __attribute__((packed));

    GDTPointer ptr;

    ptr.limit = sizeof(GlobalDescriptorTable) - 1;
    ptr.base = (uint32_t)this;

    /**
     * @brief Loads the contents of above GDT Pointer into the GDTR.
     */
    asm volatile("lgdt %0" : : "m"(ptr));
}

GlobalDescriptorTable::~GlobalDescriptorTable() {}

uint16_t GlobalDescriptorTable::DataSegmentSelector()
{
    return (uint8_t *)&dataSegmentSelector - (uint8_t *)this;
}

uint16_t GlobalDescriptorTable::CodeSegmentSelector()
{
    return (uint8_t *)&codeSegmentSelector - (uint8_t *)this;
}

GlobalDescriptorTable::SegmentDescriptor::SegmentDescriptor(uint32_t base, uint32_t limit,
                                                            uint8_t type)
{
    uint8_t *target = (uint8_t *)this;

    /**
     * Limit poses a challenge. We only have 20 bits to store the limit. This means that the maximum
     * size our segments can take is limited to 2^20 bytes, i.e., to 1 MiB. This wouldn't do in our
     * case. (What if we wanted a segment to have 4 GiB, i.e., 4 * 2^30 bytes? We wouldn't be able
     * to store that here.)
     *
     * To prevent this problem, a solution exists. The number input to the 20 bits of limit gets
     * multiplied by 2^12, giving us 12 additional low significance bits, all 1. We essentially get
     * 20 + 12 = 32 bits, capable of making segments as large as 4 GiB. This essentially means the
     * limit provided will be measured in 4 KiB blocks instead of bytes. This conversion occurs when
     * the granularity flag is set to 1.
     */
    if (limit < 65536)
    {
        /**
         * If the limit fits within 16 bits, we set the extra 4 limit bits to 0000, and the 4 flag
         * bits to 0100.
         *
         * This means that, if limit fits within 16 bits, we'll assume the segment's limit is
         * specified in terms of bytes instead of 4 KiB blocks.
         *
         * The flag is set to 0100, meaning the granularity is 0 (the MSB) and the D/B flag is set
         * to 1. This means that any code in this memory segment should have access to 32 - bit
         * operations instead of 16 - bit operations, i.e., registers like EAX would be accessible
         * and stack operations will work on 32 bit of information.
         */
        target[6] = 0x40;
    }
    else
    {
        /**
         * Here is a design choice we implemented. The variable `limit` was a 32 bit variable.
         * According to the way we designed this system, if it is larger than what would fit in 16
         * bits, we'll assume the 12 least significant bits are all 1's, and the 20 most significant
         * bits will then be the count of 4 KiB blocks we need.
         *
         * The below if-else block simply checks if the lower 12 bits aren't actually all 1's, we
         * reduce the 13th lower significant bit (i.e., bit 12 if LSB was bit 0) by 1. This means
         * that we're reducing the size of this memory segment than what was asked, but are ensuring
         * we stay compliant with the 4 KiB block way of looking at the 20 high significant bits.
         *
         * Finally, note also that we left - shift `limit` by 12. This is (obviously) to ensure
         * `limit` reflects the count of 4 KiB blocks needed, instead of bytes needed.
         */
        if ((limit & 0xFFF) != 0xFFF)
        {
            limit = (limit >> 12) - 1;
        }
        else
        {
            limit = limit >> 12;
        }

        /**
         * Here, we set the flag to 1100. This means the granularity flag and the D/B flag, both are
         * set. We saw the function of D/B previously (in the if-block above). The granularity flag
         * specifies that we're interested in 4 KiB blocks rather than bytes as a unit, when looking
         * at the value of `limit`.
         */
        target[6] = 0xC0;
    }

    /**
     * At this point, we're aware that `limit` is a 20 - bit number (at most), and that the flag
     * bits are set. Now we're simply placing the value of `limit` into the segment descriptor, at
     * the appropriate places.
     */
    target[0] = (uint8_t)(limit & 0xFF);
    target[1] = (uint8_t)((limit >> 8) & 0xFF);
    target[6] |= (uint8_t)((limit >> 16) & 0x0F);

    /**
     * Setting the base pointer into the descriptor.
     */
    target[2] = (uint8_t)((base) & 0xFF);
    target[3] = (uint8_t)((base >> 8) & 0xFF);
    target[4] = (uint8_t)((base >> 16) & 0xFF);
    target[7] = (uint8_t)((base >> 24) & 0xFF);

    /**
     * Setting the type flags into the descriptor.
     */
    target[5] = type;
}

uint32_t GlobalDescriptorTable::SegmentDescriptor::Base()
{
    uint8_t *target = (uint8_t *)this;
    uint32_t result = target[7];
    result <<= 8;
    result |= target[4];
    result <<= 8;
    result |= target[3];
    result <<= 8;
    result |= target[2];

    return result;
}

uint32_t GlobalDescriptorTable::SegmentDescriptor::Limit()
{
    uint8_t *target = (uint8_t *)this;
    uint32_t result = target[6] & 0x0F;
    result <<= 8;
    result |= target[1];
    result <<= 8;
    result |= target[0];

    if ((target[6] & 0xC0) == 0xC0)
    {
        /**
         * Adding the 12 on bits for the case where limit was specified in terms of 4 KiB blocks.
         */
        result <<= 12;
        result |= 0x0FFF;
    }

    return result;
}