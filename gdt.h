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
     * Limit: The size of the segment. (20 bits in all.)
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
        uint32_t Base();
        uint32_t Limit();
    } __attribute__((packed));

    SegmentDescriptor nullSegmentSelector;
    SegmentDescriptor unusedSegmentSelector;
    SegmentDescriptor codeSegmentSelector;
    SegmentDescriptor dataSegmentSelector;

public:
    GlobalDescriptorTable();
    ~GlobalDescriptorTable();

    uint16_t CodeSegmentSelector();
    uint16_t DataSegmentSelector();
};

#endif