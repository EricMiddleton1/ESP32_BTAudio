#pragma once

#include <vector>
#include <cstdint>

#include "SampleBuffer.hpp"
#include "types.hpp"

namespace DSP {
  class I2SBuffer {
  public:
    I2SBuffer(int startSize = 1024);

    uint32_t size() const;
    
    SampleBuffer getSamples(Channel ch);
    
    void set(const uint8_t* buffer, uint32_t size);
    const uint8_t* get() const;
  
  protected:
    friend class SampleBuffer;

    int16_t& getSample(Channel ch, uint32_t i);
    const int16_t& getSample(Channel ch, uint32_t i) const;

  private:
    std::vector<uint8_t> m_buffer;
    uint32_t m_sampleCount;
  };
}
