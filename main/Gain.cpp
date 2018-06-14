#include "Gain.hpp"

#include <cstdint>
#include "utility.hpp"

DSP::Filter::Gain::Gain(float gain)
  : m_leftGain{convertGain(gain)}
  , m_rightGain{convertGain(gain)} {
}

DSP::Filter::Gain::Gain(float leftGain, float rightGain)
  : m_leftGain{convertGain(leftGain)}
  , m_rightGain{convertGain(rightGain)} {
}

uint32_t DSP::Filter::Gain::convertGain(float gain) {
  return saturate<uint32_t>(gain * (1 << 16));
}

void DSP::Filter::Gain::setSampleRate(int sampleRate) {
}

void DSP::Filter::Gain::processSamples(uint8_t* data, uint32_t len) {

  for(uint32_t i = 0; i < len; i += 4) {
    //16-bit samples interleaved right-left
    int16_t right = *reinterpret_cast<int16_t*>(data + i);
    int16_t left = *reinterpret_cast<int16_t*>(data + i + 2);

    auto rightScaled = static_cast<int64_t>(right) * m_rightGain;
    auto leftScaled = static_cast<int64_t>(left) * m_leftGain;

    *reinterpret_cast<int16_t*>(data + i) = saturate<int16_t>(rightScaled >> 16);
    *reinterpret_cast<int16_t*>(data + i + 2) = saturate<int16_t>(leftScaled >> 16);
  }
}
