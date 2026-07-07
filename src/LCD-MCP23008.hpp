#ifndef I2CIP_LCD_MCP23008_H_
#define I2CIP_LCD_MCP23008_H_

#include <Arduino.h>

#include <I2CIP.h>

#define MCP23XXX_BANK 1 // Uncomment if IOCON.BANK = 1
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

// Pins
#define I2CIP_LCD_MCP23008_BACKLIGHT 0x7
#define I2CIP_LCD_MCP23008_RS 0x1
#define I2CIP_LCD_MCP23008_E 0x2

// Commands
#define I2CIP_LCD_CLEARDISPLAY 0x01 // Clear display, set cursor position to zero
#define I2CIP_LCD_RETURNHOME 0x02   // Set cursor position to zero
#define I2CIP_LCD_ENTRYMODESET 0x04 // Sets the entry mode
#define I2CIP_LCD_DISPLAYCONTROL 0x08 // Controls the display; does stuff like turning it off and on
#define I2CIP_LCD_CURSORSHIFT 0x10 // Lets you move the cursor
#define I2CIP_LCD_FUNCTIONSET 0x20 // Used to send the function to set to the display
#define I2CIP_LCD_SETCGRAMADDR 0x40 // Used to set the CGRAM (character generator RAM) with characters
#define I2CIP_LCD_SETDDRAMADDR 0x80 // Used to set the DDRAM (Display Data RAM)

// flags for display entry mode
#define I2CIP_LCD_ENTRYRIGHT 0x00 // Used to set text to flow from right to left
#define I2CIP_LCD_ENTRYLEFT 0x02  // Uset to set text to flow from left to right
#define I2CIP_LCD_ENTRYSHIFTINCREMENT 0x01 // Used to 'right justify' text from the cursor
#define I2CIP_LCD_ENTRYSHIFTDECREMENT 0x00 // Used to 'left justify' text from the cursor

// flags for display on/off control
#define I2CIP_LCD_DISPLAYON 0x04 // Turns the display on
#define I2CIP_LCD_DISPLAYOFF 0x00 // Turns the display off
#define I2CIP_LCD_CURSORON 0x02 // Turns the cursor on
#define I2CIP_LCD_CURSOROFF 0x00 // Turns the cursor off
#define I2CIP_LCD_BLINKON 0x01 // Turns on the blinking cursor
#define I2CIP_LCD_BLINKOFF 0x00 // Turns off the blinking cursor

// flags for display/cursor shift
#define I2CIP_LCD_DISPLAYMOVE 0x08 // Flag for moving the display
#define I2CIP_LCD_CURSORMOVE 0x00 // Flag for moving the cursor
#define I2CIP_LCD_MOVERIGHT 0x04 // Flag for moving right
#define I2CIP_LCD_MOVELEFT 0x00 // Flag for moving left

// flags for function set
#define I2CIP_LCD_8BITMODE 0x10 // LCD 8 bit mode
#define I2CIP_LCD_4BITMODE 0x00 // LCD 4 bit mode
#define I2CIP_LCD_2LINE 0x08 // LCD 2 line mode
#define I2CIP_LCD_1LINE 0x00 // LCD 1 line mode
#define I2CIP_LCD_5x10DOTS 0x04 // 10 pixel high font mode
#define I2CIP_LCD_5x8DOTS 0x00 // 8 pixel high font mode

// options
#define I2CIP_LCD_WRITE_RETRIES 3 // Number of times to retry a write operation before failing
#define I2CIP_LCD_DELAY_MICROS 100 // Delay between writes (micros)

typedef enum { LCD_ARGS_NONE = 0x0 } i2cip_lcd_args_t;

class LCD : public OutputInterface<String, i2cip_lcd_args_t>, private Print {
  I2CIP_OUTPUT_USE_FAILSAFE(String, i2cip_lcd_args_t);
  I2CIP_OUTPUT_USE_TOSTRING(String, "%s");

  private:
    MCP23008* mcp;

    uint8_t line = 0;

    inline i2cip_errorlevel_t pulseEnable(void) {
      if(this->mcp == nullptr) {
        return I2CIP_ERR_SOFT;
      }

      i2cip_errorlevel_t errlev = this->mcp->set((i2cip_mcp23008_t)(1 << I2CIP_LCD_MCP23008_E), (i2cip_mcp23008_bitmask_t)(1 << I2CIP_LCD_MCP23008_E)); // Leave RS low
      delayMicroseconds(1); // enable pulse must be >450ns

      // Don't handle errlev, we don't want to leave the E bit high if we fail to write the data
      
      errlev = this->mcp->set((i2cip_mcp23008_t)(0), (i2cip_mcp23008_bitmask_t)(1 << I2CIP_LCD_MCP23008_E));
      delayMicroseconds(100); // commands need > 37us to settle
      return errlev;
    }

    inline i2cip_errorlevel_t send(const uint8_t& value, const bool& rs = true) {
      if(this->mcp == nullptr) {
        return I2CIP_ERR_SOFT;
      }

      i2cip_errorlevel_t errlev = this->mcp->set((i2cip_mcp23008_t)(rs ? (1 << I2CIP_LCD_MCP23008_RS) : 0x00), (i2cip_mcp23008_bitmask_t)(
        (1 << I2CIP_LCD_MCP23008_RS) |
        (1 << I2CIP_LCD_MCP23008_E)
      )); // Set RS bit; set E bit low
      I2CIP_ERR_BREAK(errlev);
      
      // Send high nibble

      errlev = this->mcp->set((i2cip_mcp23008_t)(((value >> 4) & 0x0F) << 3), (i2cip_mcp23008_bitmask_t)(0b01111000)); // Set high nibble
      I2CIP_ERR_BREAK(errlev);

      errlev = pulseEnable();
      I2CIP_ERR_BREAK(errlev);

      // Send low nibble

      errlev = this->mcp->set((i2cip_mcp23008_t)((value & 0x0F) << 3), (i2cip_mcp23008_bitmask_t)(0b01111000)); // Set low nibble
      I2CIP_ERR_BREAK(errlev);

      return pulseEnable();
    }

  public:
    LCD(MCP23008* mcp);

    ~LCD();

    size_t write(uint8_t c) override;

    i2cip_errorlevel_t set(const String& value, const i2cip_lcd_args_t& args) override;
};

#endif