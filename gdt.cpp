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
         * TODO: Why is flag set to 0100?
         */
        target[6] = 0x40;
    }
    else
    {
        /**
         *
         */
        if ((limit & 0xFFF) != 0xFFF)
        {
            limit = (limit >> 12) - 1;
        }
        else
        {
            limit = limit >> 12;
        }

        target[6] = 0xC0;
    }

    target[0] = (uint8_t)(limit & 0xFF);
    target[1] = (uint8_t)((limit >> 8) & 0xFF);
    target[6] |= (uint8_t)((limit >> 16) & 0x0F);
}
