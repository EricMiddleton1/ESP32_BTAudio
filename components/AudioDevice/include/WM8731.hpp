#pragma once

#include <cstdint>

namespace AudioDevice {
  class WM8731 {
  public:
      WM8731();

      void start();

  private:
      static constexpr uint8_t DEVICE_ADDR = 0x1A; // = 0b0011010;
      static constexpr int ACK_CHECK_EN = 1;
      
      void i2cInit();
      void configureDevice();

      //7bit register address, 9bit value
      static int writeByte(uint8_t deviceAddr, uint8_t regAddr, uint16_t value);
  };
}
