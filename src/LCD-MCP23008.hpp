#ifndef I2CIP_LCD_MCP23008_H_
#define I2CIP_LCD_MCP23008_H_

#include <Arduino.h>

#include <I2CIP.h>

// #define MCP23XXX_BANK 1 // Uncomment if IOCON.BANK = 1
#define MCP23XXX_USE_OLAT 1 // Uncomment to use OLAT instead of GPIO for output

// Registers
#ifdef MCP23XXX_BANK
#define MCP23XXX_IODIR    (uint8_t)0x00   //!< I/O direction register
#define MCP23XXX_IPOL     (uint8_t)0x01    //!< Input polarity register
#define MCP23XXX_GPINTEN  (uint8_t)0x02 //!< Interrupt-on-change control register
#define MCP23XXX_DEFVAL   (uint8_t)0x03 //!< Default compare register for interrupt-on-change
#define MCP23XXX_INTCON   (uint8_t)0x04 //!< Interrupt control register
#define MCP23XXX_IOCON    (uint8_t)0x05  //!< Configuration register
#define MCP23XXX_GPPU     (uint8_t)0x06   //!< Pull-up resistor configuration register
#define MCP23XXX_INTF     (uint8_t)0x07   //!< Interrupt flag register
#define MCP23XXX_INTCAP   (uint8_t)0x08 //!< Interrupt capture register
#define MCP23XXX_GPIO     (uint8_t)0x09   //!< Port register
#define MCP23XXX_OLAT     (uint8_t)0x0A   //!< Output latch register
#define I2CIP_MCP23008_BANKJUMP     (uint8_t)0xA
#else
#define MCP23XXX_IODIR    (uint8_t)0x00   //!< I/O direction register
#define MCP23XXX_IPOL     (uint8_t)0x02    //!< Input polarity register
#define MCP23XXX_GPINTEN  (uint8_t)0x04 //!< Interrupt-on-change control register
#define MCP23XXX_DEFVAL   (uint8_t)0x06 //!< Default compare register for interrupt-on-change
#define MCP23XXX_INTCON   (uint8_t)0x08 //!< Interrupt control register
#define MCP23XXX_IOCON    (uint8_t)0x0A  //!< Configuration register
#define MCP23XXX_GPPU     (uint8_t)0x0C   //!< Pull-up resistor configuration register
#define MCP23XXX_INTF     (uint8_t)0x0E   //!< Interrupt flag register
#define MCP23XXX_INTCAP   (uint8_t)0x10 //!< Interrupt capture register
#define MCP23XXX_GPIO     (uint8_t)0x12   //!< Port register
#define MCP23XXX_OLAT     (uint8_t)0x14   //!< Output latch register
#define I2CIP_MCP23008_BANKJUMP     (uint8_t)0x1
#endif

#define I2CIP_MCP23008_ADDRESS     32    // MCP23008 address (0x20-0x27)
// #define I2CIP_MCP23008_TIMEOUT  1000   // If we're going to crash on a module ping fail, we should wait a bit

// #define I2CIP_MCP23008_USEPULLUPS true // Uncomment to enable pullups

class MCP23008;

using namespace I2CIP;

typedef uint8_t i2cip_mcp23008_t;
typedef uint8_t i2cip_mcp23008_bitmask_t;

typedef enum { PIN_0 = 0, PIN_1, PIN_2, PIN_3, PIN_4, PIN_5, PIN_6, PIN_7 } i2cip_mcp23008_pinsel_t;

class MCP23008 : public Device, public IOInterface<i2cip_mcp23008_t, i2cip_mcp23008_bitmask_t, i2cip_mcp23008_t, i2cip_mcp23008_bitmask_t> {
  I2CIP_DEVICE_CLASS_BUNDLE(MCP23008);

  I2CIP_OUTPUT_USE_FAILSAFE(i2cip_mcp23008_t, i2cip_mcp23008_bitmask_t);
  I2CIP_OUTPUT_USE_TOSTRING(i2cip_mcp23008_t, "0x%04X");

  I2CIP_INPUT_USE_RESET(i2cip_mcp23008_t, i2cip_mcp23008_bitmask_t)
  I2CIP_INPUT_USE_TOSTRING(i2cip_mcp23008_t, "%u"); // JSON-friendly
  I2CIP_INPUT_ADD_PRINTCACHE(i2cip_mcp23008_t, "0x%04X"); // HEX


  private:
    uint16_t readBuffer = 0x00;

  // protected:
    i2cip_errorlevel_t getIODIR(i2cip_mcp23008_t& dest);
    i2cip_errorlevel_t setIODIR(const i2cip_mcp23008_t& value);
  public:
    MCP23008(i2cip_fqa_t fqa, const i2cip_id_t& id);

    ~MCP23008();

    /**
     * Read a section from MCP23008.
     * @param dest Destination heap (pointer reassigned, not overwritten)
     * @param args Number of bytes to read
     **/
    i2cip_errorlevel_t get(i2cip_mcp23008_t& dest, const i2cip_mcp23008_bitmask_t& args) override;
  
    /**
     * Write to a section of MCP23008.
     * @param value Value to write (null-terminated)
     * @param args Number of bytes to write
     **/
    i2cip_errorlevel_t set(const i2cip_mcp23008_t& value, const i2cip_mcp23008_bitmask_t& args) override;
};

#endif