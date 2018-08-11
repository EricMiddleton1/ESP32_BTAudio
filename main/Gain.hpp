#pragma once

#include "IFilter.hpp"

namespace DSP {
namespace Filter {
  class Gain : public IFilter {
  public:
    //@param gain: Scale in dB
    Gain(float gain);

    void setSampleRate(int sampleRate) override;

    void processSamples(SampleBuffer& samples) override;
  private:
    static uint32_t convertGain(float gain);

    uint32_t m_gain;
  };
}
}
