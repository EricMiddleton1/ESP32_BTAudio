#pragma once

#include <cstdint>

#include "I2SPort.hpp"
#include "I2SSink.hpp"
#include "I2SSource.hpp"


namespace AudioDevice {
  class WM8731 {
  public:
      WM8731(I2S::Port& i2sPort, int i2sSinkPin, int i2sSourcePin,
        Audio::AudioCallback&& sinkCB);

      void start();
      void stop();

      bool running() const;

      void writeSamples(const Audio::SampleBuffer& leftSamples,
        const Audio::SampleBuffer& rightSamples);

  private:
      static constexpr uint8_t DEVICE_ADDR = 0x1A; // = 0b0011010;
      static constexpr int ACK_CHECK_EN = 1;
      
      void i2cInit();
      void configureDevice();

      //7bit register address, 9bit value
      static int writeByte(uint8_t deviceAddr, uint8_t regAddr, uint16_t value);

      I2S::Sink m_i2sSink;
      I2S::Source m_i2sSource;
  };
}
