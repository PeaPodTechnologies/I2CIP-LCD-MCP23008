#include <Arduino.h>
#include <unity.h>

#include <I2CIP.h>
#include <LCD-MCP23008.h>

using namespace I2CIP;

i2cip_fqa_t fqa_mcp23008 = createFQA(0, I2CIP_MUX_NUM_FAKE, I2CIP_MUX_BUS_FAKE, 0x20);

MCP23008* mcp = nullptr;
// LCD* lcd = nullptr;

void test_oop(void) {
  mcp = new MCP23008(fqa_mcp23008, "test_mcp23008");
  TEST_ASSERT_NOT_NULL(mcp);

  // lcd = new LCD(mcp);
  // TEST_ASSERT_NOT_NULL(lcd);
}

void test_display_on(void) {
  if (mcp == nullptr) {
    TEST_FAIL_MESSAGE("MCP23008 object is null");
  }

  i2cip_errorlevel_t errlev = mcp->set((i2cip_mcp23008_t)(1 << I2CIP_LCD_MCP23008_BACKLIGHT), (i2cip_mcp23008_bitmask_t)(1 << I2CIP_LCD_MCP23008_BACKLIGHT));
  TEST_ASSERT_EQUAL(I2CIP_ERR_NONE, errlev);
}

void test_display_off(void) {
  if (mcp == nullptr) {
    TEST_FAIL_MESSAGE("MCP23008 object is null");
  }

  i2cip_errorlevel_t errlev = mcp->set((i2cip_mcp23008_t)0x00, (i2cip_mcp23008_bitmask_t)(1 << I2CIP_LCD_MCP23008_BACKLIGHT));
  TEST_ASSERT_EQUAL(I2CIP_ERR_NONE, errlev);
}

void setup() {
  delay(2000);

  UNITY_BEGIN();
  RUN_TEST(test_oop);
}

uint8_t i = 0;

void loop() {
  if (i < 100) {
    RUN_TEST(test_display_on);
    delay(1000);
    RUN_TEST(test_display_off);
    delay(1000);
    i++;
  } else {
    UNITY_END();
    while (1) {
      delay(1000);
    }
  }
}