#pragma once

#include <cstdint>

namespace DSP {
  class IFilter {
  public:
    virtual void setSampleRate(int sampleRate)=0;

    virtual int16_t processSample(int16_t sample)=0;
  };
}
