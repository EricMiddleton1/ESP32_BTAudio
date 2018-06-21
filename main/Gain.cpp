#include "Gain.hpp"

#include <cmath>
#include <cstdint>
#include "utility.hpp"

DSP::Filter::Gain::Gain(float gain)
  : m_gain{convertGain(gain)} {
}

uint32_t DSP::Filter::Gain::convertGain(float gain) {
  //Convert dB to raw amplitude
  gain = std::pow(10.f, gain/20.f);

  //Store as 16:16 fixed point
  return saturate<uint32_t>(gain * (1 << 16));
}

void DSP::Filter::Gain::setSampleRate(int sampleRate) {
}

int16_t DSP::Filter::Gain::processSample(int16_t sample) {
  auto scaled = static_cast<int64_t>(sample) * m_gain;

  return saturate<int16_t>(scaled >> 16);
}
