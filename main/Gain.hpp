#pragma once

#include "IFilter.hpp"

namespace DSP {
namespace Filter {
  class Gain : public IFilter {
  public:
    //@param gain: 16:16 fixed-point gain (actual gain = 1 when gain = 2^16)
    Gain(float gain);
    Gain(float leftGain, float rightGain);

    void setSampleRate(int sampleRate) override;

    void processSamples(uint8_t* data, uint32_t len) override;
  private:
    static uint32_t convertGain(float gain);

    uint32_t m_leftGain, m_rightGain;
  };
}
}
