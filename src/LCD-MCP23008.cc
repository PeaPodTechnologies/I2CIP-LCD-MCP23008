#include <LCD-MCP23008.h>

#ifndef I2CIP_H_
#error "I2CIP must be in I2CIP-LCD-MCP23008/libs, or, adjacent to I2CIP-LCD-MCP23008 together in $PWD/libs"
#else

using namespace I2CIP;

I2CIP_DEVICE_INIT_STATIC_ID(MCP23008);
// By default, ALL input, no/low output
I2CIP_INPUT_INIT_RESET(MCP23008, i2cip_mcp23008_t, 0x00, i2cip_mcp23008_bitmask_t, 0xFF);
I2CIP_OUTPUT_INIT_FAILSAFE(MCP23008, i2cip_mcp23008_t, 0x00, i2cip_mcp23008_bitmask_t, 0x00);

void MCP23008::parseJSONArgs(I2CIP::i2cip_args_io_t& argsDest, JsonVariant argsA, JsonVariant argsS, JsonVariant argsB) {
  if(argsA.is<int>()) {
    int in = argsA.as<int>();
    if(in >= 0 && in <= 0xFFFF) { // TODO: Fix upper bounds
      // Set input
      argsDest.a = new int(in);
    }
  }
  if(argsS.is<int>() && argsB.is<int>()) {
    int out = argsS.as<int>();
    int outmask = argsB.as<int>();
    if(out >= 0 && out <= 0xFFFF && outmask >= 0 && outmask <= 0xFFFF) { // TODO: Fix upper bounds
      // Set output
      argsDest.s = new int(out);
      argsDest.b = new int(outmask);
    }
  }
}

void MCP23008::deleteArgs(I2CIP::i2cip_args_io_t& args) {
  delete((i2cip_mcp23008_bitmask_t*)args.a);
  delete((i2cip_mcp23008_t*)args.s);
  delete((i2cip_mcp23008_bitmask_t*)args.b);
}

MCP23008::MCP23008(i2cip_fqa_t fqa, const i2cip_id_t& id) : Device(fqa, id), IOInterface<i2cip_mcp23008_t, i2cip_mcp23008_bitmask_t, i2cip_mcp23008_t, i2cip_mcp23008_bitmask_t>((Device*)this) { }

MCP23008::~MCP23008() {
  #ifdef I2CIP_DEBUG_SERIAL
    DEBUG_DELAY();
    I2CIP_DEBUG_SERIAL.print(F("MCP23008 Destructed "));
    I2CIP_DEBUG_SERIAL.print(I2CIP_FQA_SEG_I2CBUS(this->fqa), HEX);
    I2CIP_DEBUG_SERIAL.print(F(":"));
    I2CIP_DEBUG_SERIAL.print(I2CIP_FQA_SEG_MODULE(this->fqa), HEX);
    I2CIP_DEBUG_SERIAL.print(F(":"));
    I2CIP_DEBUG_SERIAL.print(I2CIP_FQA_SEG_MUXBUS(this->fqa), HEX);
    I2CIP_DEBUG_SERIAL.print(F(":"));
    I2CIP_DEBUG_SERIAL.print(I2CIP_FQA_SEG_DEVADR(this->fqa), HEX);
    I2CIP_DEBUG_SERIAL.print(F(" @0x"));
    I2CIP_DEBUG_SERIAL.println((uintptr_t)this, HEX);
    DEBUG_DELAY();
  #endif

  // Cleanup
  // for(uint8_t i = 0; i < 16; i++) {
  //   if(pins[i] != nullptr) {
  //     delete pins[i];
  //   }
  // }
}

i2cip_errorlevel_t MCP23008::get(i2cip_mcp23008_t& dest, const i2cip_mcp23008_bitmask_t& args) {
  #ifdef I2CIP_DEBUG_SERIAL
    DEBUG_DELAY();
    I2CIP_DEBUG_SERIAL.print(F("MCP23008 GET: MASK 0b"));
    I2CIP_DEBUG_SERIAL.print((uint8_t)(args >> 8), BIN);
    I2CIP_DEBUG_SERIAL.print(' ');
    I2CIP_DEBUG_SERIAL.print((uint8_t)args, BIN);
    I2CIP_DEBUG_SERIAL.print(F(" -> ["));
    uint8_t bits = sizeof(i2cip_mcp23008_bitmask_t) * 8;
    uint8_t cmask = 1; bool comma = false;
    for(uint8_t bit = 0; bit < bits; bit++) {
      if(args & cmask) {
        if(comma) { I2CIP_DEBUG_SERIAL.print(F(", ")); } else { comma = true; }
        I2CIP_DEBUG_SERIAL.print(bit > 7 ? F("B") : F("A"));
        I2CIP_DEBUG_SERIAL.print(bit % 8);
      }
      cmask << 1;
    }
    I2CIP_DEBUG_SERIAL.println(']');
    DEBUG_DELAY();
  #endif

  // IODIR: 1 = input, 0 = output

  // uint16_t iodir = 0; // All output; no read
  i2cip_mcp23008_bitmask_t iodir = getArgsA();

  i2cip_errorlevel_t errlev = getIODIR(iodir);
  I2CIP_ERR_BREAK(errlev);

  // Set selected zeroes to ones, leave everything else
  iodir |= args;

  // Write IODIR
  errlev = setIODIR(iodir);
  I2CIP_ERR_BREAK(errlev);

  // errlev = getIODIR(iodir); // Uncomment to verify
  // I2CIP_ERR_BREAK(errlev);

  // Read GPIO LSB Bank A
  uint8_t gpioa = 0; // Low
  errlev = readRegisterByte(MCP23XXX_GPIO, gpioa, false);
  I2CIP_ERR_BREAK(errlev);

  // Read GPIO MSB Bank B
  // uint8_t gpiob = 0;
  // errlev = readRegisterByte((uint8_t)(MCP23XXX_GPIO + I2CIP_MCP23017_BANKJUMP), gpiob, false);
  // I2CIP_ERR_BREAK(errlev);

  // dest = ((uint16_t)gpiob << 8) | gpioa;
  dest = (i2cip_mcp23008_t)gpioa;

  return errlev;
}

// #ifdef I2CIP_DEBUG_SERIAL
//   DEBUG_DELAY();
//   I2CIP_DEBUG_SERIAL.print(F("MCP23017 IODIR 0b"));
//   I2CIP_DEBUG_SERIAL.print(iodir, BIN);
//   I2CIP_DEBUG_SERIAL.print(F(" -> 0b"));
//   I2CIP_DEBUG_SERIAL.print((iodir & 0xFF), BIN);
//   #endif

i2cip_errorlevel_t MCP23008::getIODIR(i2cip_mcp23008_t& dest) {
  // Read IODIR LSB Bank A
  uint8_t reg = 0;
  i2cip_errorlevel_t errlev = readRegisterByte(MCP23XXX_IODIR, reg, false);
  I2CIP_ERR_BREAK(errlev);

  // Read IODIR MSB Bank B
  // uint8_t reg2 = 0;
  // errlev = readRegisterByte((uint8_t)(MCP23XXX_IODIR + I2CIP_MCP23017_BANKJUMP), reg2, false, false);
  // I2CIP_ERR_BREAK(errlev);

  // dest = (((uint16_t)reg2) << 8) + (uint16_t)reg;
  dest = (i2cip_mcp23008_t)reg;

  #ifdef I2CIP_DEBUG_SERIAL
    DEBUG_DELAY();
    I2CIP_DEBUG_SERIAL.print(F("MCP23008 IODIR 0b"));
    I2CIP_DEBUG_SERIAL.print((uint8_t)dest, BIN);
    I2CIP_DEBUG_SERIAL.println();
    DEBUG_DELAY();
  #endif

  return errlev;
}

i2cip_errorlevel_t MCP23008::setIODIR(const i2cip_mcp23008_t& value) {
  // Write IODIR
  i2cip_errorlevel_t errlev = writeRegister(MCP23XXX_IODIR, (uint8_t)(value & 0xFF), false);
  I2CIP_ERR_BREAK(errlev);

  return errlev;
}

i2cip_errorlevel_t MCP23008::set(const i2cip_mcp23008_t& value, const i2cip_mcp23008_bitmask_t& args) {
  // IODIR: 1 = input, 0 = output
  #ifdef I2CIP_DEBUG_SERIAL
    DEBUG_DELAY();
    I2CIP_DEBUG_SERIAL.print(F("MCP23008 SET: MASK 0b"));
    I2CIP_DEBUG_SERIAL.print((uint8_t)args, BIN);
    I2CIP_DEBUG_SERIAL.print(F(" -> {"));
    uint8_t bits = sizeof(i2cip_mcp23008_bitmask_t) * 8;
    i2cip_mcp23008_t tval = value; uint8_t cmask = 1; bool comma = false;
    for(uint8_t bit = 0; bit < bits; bit++) {
      if(args & cmask) {
        if(comma) { I2CIP_DEBUG_SERIAL.print(F(", ")); } else { comma = true; }
        I2CIP_DEBUG_SERIAL.print(bit > 7 ? F("B") : F("A"));
        I2CIP_DEBUG_SERIAL.print(bit % 8);
        I2CIP_DEBUG_SERIAL.print(F(": "));
        I2CIP_DEBUG_SERIAL.print(tval & 1 ? F("HIGH") : F("LOW"));
      }
      tval >>= 1;
      cmask <<= 1;
    }
    I2CIP_DEBUG_SERIAL.println('}');
    DEBUG_DELAY();
  #endif

  // SPECIAL CASE: Default args == 0; Just ping
  // if(args == 0) {
  //   I2CIP_DEBUG_SERIAL.println(F("MCP23017 SETNOP PING"));
  //   return ping();
  // }

  // Get current IODIR
  // uint8_t iodir = ~0; // All input; no output
  i2cip_mcp23008_bitmask_t iodir = getArgsB(); // Last known bitmask

  i2cip_errorlevel_t errlev = getIODIR(iodir);
  I2CIP_ERR_BREAK(errlev);

  iodir &= ~args; // Set selected ones to zeroes, leave everything else

  errlev = setIODIR(iodir);
  I2CIP_ERR_BREAK(errlev);

  // errlev = getIODIR(iodir); // Uncomment to verify
  // I2CIP_ERR_BREAK(errlev);

  // #ifdef I2CIP_MCP23017_USEPULLUPS
  //   // GPPU (100kOhm): 1 = input-pullup, 0 = none

  //   // Identical to IODIR (All inputs are pulled up)
  //   errlev = writeRegister(MCP23XXX_GPPU, (uint8_t)(iodir & 0xFF) & (args & 0xFF), false);
  //   I2CIP_ERR_BREAK(errlev);
  //   errlev = writeRegister((uint8_t)(MCP23XXX_GPPU + I2CIP_MCP23017_BANKJUMP), (uint8_t)((iodir >> 8) & 0xFF) & (args >> 8), false);
  //   I2CIP_ERR_BREAK(errlev);

  // #else
  //   // ALL OFF
  //   errlev = writeRegister(MCP23XXX_GPPU, (uint8_t)0x00, false);
  //   I2CIP_ERR_BREAK(errlev);
  //   errlev = writeRegister((uint8_t)(MCP23XXX_GPPU + I2CIP_MCP23017_BANKJUMP), (uint8_t)0x00, false);
  //   I2CIP_ERR_BREAK(errlev);

  // #endif

  // GPIO: 1 = high, 0 = low

  #ifdef MCP23XXX_USE_OLAT
  uint8_t reg = MCP23XXX_OLAT;
  #else
  uint8_t reg = MCP23XXX_GPIO;
  #endif

  // Read GPIO LSB Bank A
  uint8_t gpioa = 0; // Low
  errlev = readRegisterByte(reg, gpioa, false, false);
  I2CIP_ERR_BREAK(errlev);

  // Read GPIO MSB Bank B
  // uint8_t gpiob = 0;
  // errlev = readRegisterByte((uint8_t)(reg + I2CIP_MCP23017_BANKJUMP), gpiob, false, false);
  // I2CIP_ERR_BREAK(errlev);

  // i2cip_mcp23017_t gpio = ((uint16_t)gpiob << 8) | gpioa;
  i2cip_mcp23008_t gpio = (i2cip_mcp23008_t)gpioa;
  i2cip_mcp23008_t send = (gpio & ~args) | (value & args);

  #ifdef I2CIP_DEBUG_SERIAL
    DEBUG_DELAY();
    I2CIP_DEBUG_SERIAL.print(F("MCP23008 SET: OLD 0b"));
    I2CIP_DEBUG_SERIAL.print((uint8_t)gpio, BIN);
    I2CIP_DEBUG_SERIAL.print("; NEW 0b");
    I2CIP_DEBUG_SERIAL.print((uint8_t)send, BIN);
    DEBUG_DELAY();
  #endif

  // Write GPIO
  errlev = writeRegister(reg, (uint8_t)(send & 0xFF), false);
  I2CIP_ERR_BREAK(errlev);
  // errlev = writeRegister((uint8_t)(reg + I2CIP_MCP23017_BANKJUMP), (uint8_t)((send >> 8) & 0xFF), false);

  return errlev;
}

I2CIP_OUTPUT_INIT_FAILSAFE(LCD, String, "FAIL", i2cip_lcd_args_t, LCD_ARGS_NONE); // Default to just updating RGB

LCD::LCD(MCP23008* mcp) : mcp(mcp), OutputInterface<String, i2cip_lcd_args_t>(nullptr) { } // TODO: Should this not be nullptr? Should it be mcp?

LCD::~LCD() { }

size_t LCD::write(uint8_t c) {
  int row_offsets[] = {0x00, 0x40, 0x14, 0x54};
  size_t r = 0; uint8_t i = 0;
  switch(c) {
    case '\0':
      this->line = 0; // Reset line counter on null character
      return 1;
    case '\r':
    case '\t':
      return 1; // NOP here is OK
    case '\n':
      this->line++;
      if(this->line >= 4) this->line = 0; // Wrap around if exceeding 4 lines
      while(r == 0 && i < I2CIP_LCD_WRITE_RETRIES) {
        i++;
        r = (this->send(I2CIP_LCD_SETDDRAMADDR | (row_offsets[this->line]), false) == I2CIP_ERR_NONE ? 1 : 0);
        delayMicroseconds(I2CIP_LCD_DELAY_MICROS);
      }
      break;
    default:
      while(r == 0 && i < I2CIP_LCD_WRITE_RETRIES) { // Fewer retries; multiples
        i++;
        r = (this->send(c, true) == I2CIP_ERR_NONE ? 1 : 0);
        delayMicroseconds(I2CIP_LCD_DELAY_MICROS);
      }
      break;
  }
  return r;
}

i2cip_errorlevel_t LCD::set(const String& value, const i2cip_lcd_args_t& args) {
  this->line = 0; // Reset line counter on new string
  
  if(this->mcp == nullptr) {
    return I2CIP_ERR_SOFT;
  }

  // ENTER 4-BIT MODE

  i2cip_errorlevel_t errlev = this->mcp->set((i2cip_mcp23008_t)0x00, (i2cip_mcp23008_bitmask_t)(
    (1 << I2CIP_LCD_MCP23008_RS) |
    (1 << I2CIP_LCD_MCP23008_E)
  )); // Set RS bit; set E bit low
  I2CIP_ERR_BREAK(errlev);

  errlev = this->mcp->set((i2cip_mcp23008_t)(0x03 << 3), (i2cip_mcp23008_bitmask_t)(I2CIP_LCD_DATAMASK)); // Set 4-bit mode
  I2CIP_ERR_BREAK(errlev);

  errlev = pulseEnable();
  I2CIP_ERR_BREAK(errlev);

  delayMicroseconds(4500);

  // second try
  errlev = this->mcp->set((i2cip_mcp23008_t)(0x03 << 3), (i2cip_mcp23008_bitmask_t)(I2CIP_LCD_DATAMASK)); // Set 4-bit mode
  I2CIP_ERR_BREAK(errlev);

  errlev = pulseEnable();
  I2CIP_ERR_BREAK(errlev);
  delayMicroseconds(4500); // wait min 4.1ms

  errlev = this->mcp->set((i2cip_mcp23008_t)(0x03 << 3), (i2cip_mcp23008_bitmask_t)(I2CIP_LCD_DATAMASK));
  I2CIP_ERR_BREAK(errlev);

  errlev = pulseEnable();
  I2CIP_ERR_BREAK(errlev);
  delayMicroseconds(150);

  // set to 8-bit interface
  errlev = this->mcp->set((i2cip_mcp23008_t)(0x02 << 3), (i2cip_mcp23008_bitmask_t)(I2CIP_LCD_DATAMASK));
  I2CIP_ERR_BREAK(errlev);

  errlev = pulseEnable();
  I2CIP_ERR_BREAK(errlev);

  // COMMAND Function set: 2-line, 5x8 dots (default), 4-bit mode (default)
  errlev = this->send(I2CIP_LCD_FUNCTIONSET | I2CIP_LCD_2LINE, false);
  I2CIP_ERR_BREAK(errlev);

  // COMMAND Display control: display on, cursor off (default), blink off (default)
  errlev = this->send(I2CIP_LCD_DISPLAYCONTROL | I2CIP_LCD_DISPLAYON, false);
  I2CIP_ERR_BREAK(errlev);

  // COMMAND Entry mode: increment cursor (default), no display shift (default)
  errlev = this->send(I2CIP_LCD_ENTRYMODESET | I2CIP_LCD_ENTRYLEFT, false);
  I2CIP_ERR_BREAK(errlev);

  // COMMAND Clear display
  errlev = this->send(I2CIP_LCD_CLEARDISPLAY, false);
  I2CIP_ERR_BREAK(errlev);

  // COMMAND Return home
  errlev = this->send(I2CIP_LCD_RETURNHOME, false);
  I2CIP_ERR_BREAK(errlev);

  // Backlight
  errlev = this->mcp->set((i2cip_mcp23008_t)(1 << I2CIP_LCD_MCP23008_BACKLIGHT), (i2cip_mcp23008_bitmask_t)(1 << I2CIP_LCD_MCP23008_BACKLIGHT));
  I2CIP_ERR_BREAK(errlev);

  return this->print(value) == 0 ? I2CIP_ERR_SOFT : I2CIP_ERR_NONE; // Return error if nothing was printed
}

#endif