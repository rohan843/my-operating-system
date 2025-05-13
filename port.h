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
    ~Port();
};

/**
 * @brief A class that enables creation of a port object for an 8 - bit port.
 *
 * This class inherits the Port class.
 *
 */
class Port8Bit : public Port
{
  public:
    /**
     * @brief Construct a new Port 8 Bit object.
     *
     * @param portnumber The 16 - bit port number of the current port.
     */
    Port8Bit(uint16_t portnumber);
    ~Port8Bit();
    /**
     * @brief Writes 8 - bit of data to the port.
     *
     * @param data The 8 - bit data to write to the port.
     */
    virtual void Write(uint8_t data);
    /**
     * @brief Reads 8 - bit of data from the port.
     *
     * @return uint8_t The data read from the port.
     */
    virtual uint8_t Read();
};

/**
 * @brief A class that enables creation of a port object for a slow 8 - bit port.
 *
 * This class inherits the Port8Bit class.
 *
 */
class Port8BitSlow : public Port8Bit
{
  public:
    /**
     * @brief Construct a new Port 8 Bit Slow object.
     *
     * @param portnumber The 16 - bit port number of the current port.
     */
    Port8BitSlow(uint16_t portnumber);
    ~Port8BitSlow();
    /**
     * @brief Writes 8 - bit of data to the port.
     *
     * @param data The 8 - bit data to write to the port.
     */
    virtual void Write(uint8_t data);
};

/**
 * @brief A class that enables creation of a port object for an 16 - bit port.
 *
 * This class inherits the Port class.
 *
 */
class Port16Bit : public Port
{
  public:
    /**
     * @brief Construct a new Port 16 Bit object.
     *
     * @param portnumber The 16 - bit port number of the current port.
     */
    Port16Bit(uint16_t portnumber);
    ~Port16Bit();
    /**
     * @brief Writes 16 - bit of data to the port.
     *
     * @param data The 16 - bit data to write to the port.
     */
    virtual void Write(uint16_t data);
    /**
     * @brief Reads 16 - bit of data from the port.
     *
     * @return uint16_t The data read from the port.
     */
    virtual uint16_t Read();
};

/**
 * @brief A class that enables creation of a port object for an 32 - bit port.
 *
 * This class inherits the Port class.
 *
 */
class Port32Bit : public Port
{
  public:
    /**
     * @brief Construct a new Port 32 Bit object.
     *
     * @param portnumber The 32 - bit port number of the current port.
     */
    Port32Bit(uint16_t portnumber);
    ~Port32Bit();
    /**
     * @brief Writes 32 - bit of data to the port.
     *
     * @param data The 32 - bit data to write to the port.
     */
    virtual void Write(uint32_t data);
    /**
     * @brief Reads 32 - bit of data from the port.
     *
     * @return uint32_t The data read from the port.
     */
    virtual uint32_t Read();
};

#endif