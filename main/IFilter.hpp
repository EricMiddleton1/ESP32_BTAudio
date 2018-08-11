#pragma once

#include "SampleBuffer.hpp"

namespace DSP {
  class IFilter {
  public:
    virtual void setSampleRate(int sampleRate)=0;

    virtual void processSamples(SampleBuffer& samples)=0;
  };
}
