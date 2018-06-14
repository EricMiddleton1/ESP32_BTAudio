#pragma once

#include <cstdint>

namespace DSP {
  class IFilter {
  public:
    virtual void setSampleRate(int sampleRate)=0;

    virtual void processSamples(uint8_t* data, uint32_t len)=0;
  };
}
