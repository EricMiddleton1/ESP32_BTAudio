#include "BiquadFilter.hpp"

#include <cmath>

#include "utility.hpp"

extern "C" {
#include <esp_log.h>
}

namespace DSP {
namespace Filter {

MonoBiquad::MonoBiquad()
  : m_x1{0}
  , m_x2{0}
  , m_y1{0}
  , m_y2{0} {

  setCoefficients(0.f, 0.f, 0.f, 1.f, 0.f, 0.f);
}

void MonoBiquad::setCoefficients(float b0, float b1, float b2,
  float a0, float a1, float a2) {
  
  ESP_LOGI("MonoBiquad", "Coefficients: {%f, %f, %f, %f, %f, %f}",
    b0, b1, b2, a0, a1, a2);

  m_b0 = convertCoefficient(b0/a0);
  m_b1 = convertCoefficient(b1/a0);
  m_b2 = convertCoefficient(b2/a0);
  m_a1 = convertCoefficient(a1/a0);
  m_a2 = convertCoefficient(a2/a0);

  m_x1 = m_x2 = 0;
  m_y1 = m_y2 = 0;

  ESP_LOGI("MonoBiquad", "Coefficients: {%d, %d, %d, %d, %d}",
    m_b0, m_b1, m_b2, m_a1, m_a2);
}

int32_t MonoBiquad::convertCoefficient(float coef) {
  return saturate<int32_t>(coef * (1 << FRAC_BITS_COEF));
}

int16_t MonoBiquad::filter(int16_t x) {
  int32_t x_scaled = x << FRAC_BITS_SAMPLE;

  //Direct Form 1
  int64_t intermediate = static_cast<int64_t>(m_b0)*x
    + static_cast<int64_t>(m_b1)*m_x1 + static_cast<int64_t>(m_b2)*m_x2
    - static_cast<int64_t>(m_a1)*m_y1 - static_cast<int64_t>(m_a2)*m_y2;

  auto y = saturate<int16_t>(intermediate >> (FRAC_BITS_COEF + FRAC_BITS_SAMPLE));

  m_y2 = m_y1;
  m_y1 = saturate<int32_t>(intermediate >> FRAC_BITS_COEF);
  m_x2 = m_x1;
  m_x1 = x_scaled;

  return y;
}


void BiquadFilter::setCoefficients(float b0, float b1, float b2,
  float a0, float a1, float a2) {

  m_leftBiquad.setCoefficients(b0, b1, b2, a0, a1, a2);
  m_rightBiquad.setCoefficients(b0, b1, b2, a0, a1, a2);
}

void BiquadFilter::processSamples(uint8_t* data, uint32_t len) {
  for(uint32_t i = 0; i < len; i += 4) {
    //16-bit samples interleaved right-left
    int16_t right = *reinterpret_cast<int16_t*>(data + i);
    int16_t left = *reinterpret_cast<int16_t*>(data + i + 2);

    right = m_rightBiquad.filter(right);
    left = m_leftBiquad.filter(left);

    *reinterpret_cast<int16_t*>(data + i) = right;
    *reinterpret_cast<int16_t*>(data + i + 2) = left;
  }
}


Biquad::LPF::LPF(float fc, float Q)
  : m_fc{fc}
  , m_Q{Q} {
}

void Biquad::LPF::setSampleRate(int sampleRate) {
  float w0 = 2.f*PI*m_fc/sampleRate;
  float sw = std::sin(w0), cw = std::cos(w0);
  float alpha = sw / (2.f*m_Q);

  float 
    b0 = (1.f - cw)/2.f,
    b1 = 1.f - cw,
    b2 = (1.f - cw)/2.f,
    a0 = 1.f + alpha,
    a1 = -2.f*cw,
    a2 = 1.f - alpha;

  ESP_LOGI("Biquad::LPF", "Coefficients: {%f, %f, %f, %f, %f, %f}",
    b0, b1, b2, a0, a1, a2);

  setCoefficients(b0, b1, b2, a0, a1, a2);
}


}
}
