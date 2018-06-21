#pragma once

#include <cstdint>

#include "types.hpp"

namespace DSP {
  class I2SBuffer;

  class SampleBuffer {
  public:
    SampleBuffer(I2SBuffer& buffer, Channel ch);

    int16_t& operator[](uint32_t i);
    const int16_t& operator[](uint32_t i) const;

    uint32_t size() const;

  private:
    I2SBuffer& m_buffer;
    Channel m_ch;
  };
}
