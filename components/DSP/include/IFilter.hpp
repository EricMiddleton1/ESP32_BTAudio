#pragma once

#include "types.hpp"

namespace DSP {
  class IFilter {
  public:
    virtual void setSampleRate(int sampleRate)=0;

    virtual void processSamples(Audio::SampleBuffer& samples)=0;
  };
}
