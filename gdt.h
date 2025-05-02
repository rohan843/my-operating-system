#ifndef __GDT_H
#define __GDT_H

#include "types.h"

/*
This file contains code related to the global descriptor table.
*/

class GlobalDescriptorTable
{
public:
    /**
     * Desribes how an 8 byte segment entry looks like in the GDT.
     *
     * Limit: The size of the segment. (20 bits in all.) If limit fits in 16 bits, will be treated
     * as a count of bytes, otherwise as count of 4 KiB blocks of memory in the segment. THIS VALUE
     * MIGHT NOT BE THE ACTUAL SIZE ALLOCATED TO THE SEGMENT. CHECK THE SIZE AFTER SEGMENT CREATION.
     * Base: The base address of the segment. (32 bits in all.)
     * Type: The bits describing the type of memory segment. (8 bits in all.)
     * Flags: The bits describing flag information. (4 bits in all.)
     */
    class SegmentDescriptor
    {
    private:
        uint16_t limit_lo;
        uint16_t base_lo;
        uint8_t base_hi;
        uint8_t type;
        uint8_t flags_limit_hi;
        uint8_t base_vhi;

    public:
        SegmentDescriptor(uint32_t base, uint32_t limit, uint8_t type);

        /**
         * Returns the value of the base pointer for the segment.
         */
        uint32_t Base();

        /**
         * Returns the value of the limit for the segment. This is the number of bytes the segment
         * contains.
         */
        uint32_t Limit();
    } __attribute__((packed));

    SegmentDescriptor nullSegmentSelector;
    SegmentDescriptor unusedSegmentSelector;
    SegmentDescriptor codeSegmentSelector;
    SegmentDescriptor dataSegmentSelector;

public:
    GlobalDescriptorTable();
    ~GlobalDescriptorTable();

    /**
     * @brief Returns a 2 byte number that contains the offset of the Code Segment Selector entry in
     * the GDT in terms of how many bytes from the start of GDT is the code segment entry present
     * at.
     */
    uint16_t CodeSegmentSelector();

    /**
     * @brief Returns a 2 byte number that contains the offset of the Data Segment Selector entry in
     * the GDT in terms of how many bytes from the start of GDT is the data segment entry present
     * at.
     */
    uint16_t DataSegmentSelector();
};

#endif