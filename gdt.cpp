#include "gdt.h"

GlobalDescriptorTable::GlobalDescriptorTable() : nullSegmentSelector(0, 0, 0),
                                                 unusedSegmentSelector(0, 0, 0),
                                                 codeSegmentSelector(0, 64 * 1024 * 1024, 0x9A),
                                                 dataSegmentSelector(0, 64 * 1024 * 1024, 0x92)
{
    uint32_t i[2];
    i[0] = (uint32_t)this;
    i[1] = sizeof(GlobalDescriptorTable) << 16;

    /**
     * Loads the information of the GDT into the GDT CPU register.
     *
     * TODO: Yet to elaborate on what exactly occurs.
     */
    asm volatile("lgdt (%0)" : : "p"(((uint8_t *)i) + 2));
}

GlobalDescriptorTable::~GlobalDescriptorTable()
{
}

/**
 * Returns a 2 byte number that contains the offset of the Data Segment Selector entry in the GDT in
 * terms of how many bytes from the start of GDT is the data segment entry present at.
 */
uint16_t GlobalDescriptorTable::DataSegmentSelector()
{
    return (uint8_t *)&dataSegmentSelector - (uint8_t *)this;
}

/**
 * Returns a 2 byte number that contains the offset of the Code Segment Selector entry in the GDT in
 * terms of how many bytes from the start of GDT is the code segment entry present at.
 */
uint16_t GlobalDescriptorTable::CodeSegmentSelector()
{
    return (uint8_t *)&codeSegmentSelector - (uint8_t *)this;
}

GlobalDescriptorTable::SegmentDescriptor::SegmentDescriptor(uint32_t base,
                                                            uint32_t limit,
                                                            uint8_t flags)
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
}
