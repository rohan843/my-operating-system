#ifndef __PORT_H
#define __PORT_H

#include "types.h"

/**
 * @brief A base class for ports.
 *
 * This class contains the base layout of any port object. Any classes created for specific ports
 * must inherit this class.
 *
 */
class Port
{
protected:
    /**
     * @brief The 16 - bit port number for this port.
     */
    uint16_t portnumber;
    /**
     * @brief Construct a new Port object
     *
     * The consturctor is protected to ensure this base class isn't instantiated.
     *
     * @param portnumber The 16 - bit port number for this port.
     * 
     */
    Port(uint16_t portnumber);
};

#endif