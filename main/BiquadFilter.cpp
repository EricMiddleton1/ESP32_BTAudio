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
  int32_t x_scaled = static_cast<int32_t>(x) << FRAC_BITS_SAMPLE;

  //Direct Form 1
  int64_t intermediate = static_cast<int64_t>(m_b0)*x_scaled
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


Biquad::ManualFilter::ManualFilter(float b0, float b1, float b2, float a0, float a1,
  float a2)
  : m_b0{b0}
  , m_b1{b1}
  , m_b2{b2}
  , m_a0{a0}
  , m_a1{a1}
  , m_a2{a2} {
}

void Biquad::ManualFilter::setSampleRate(int sampleRate) {
  setCoefficients(m_b0, m_b1, m_b2, m_a0, m_a1, m_a2);
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

  //From Audio-EQ-Cookbook
  ESP_LOGI("Biquad::LPF", "Coefficients: {%f, %f, %f, %f, %f, %f}",
    b0, b1, b2, a0, a1, a2);

  setCoefficients(b0, b1, b2, a0, a1, a2);
}

Biquad::HPF::HPF(float fc, float Q)
  : m_fc{fc}
  , m_Q{Q} {
}

void Biquad::HPF::setSampleRate(int sampleRate) {
  float w0 = 2.f*PI*m_fc/sampleRate;
  float sw = std::sin(w0), cw = std::cos(w0);
  float alpha = sw / (2.f*m_Q);

  //From Audio-EQ-Cookbook
  float 
    b0 = (1.f + cw)/2.f,
    b1 = -(1.f + cw),
    b2 = (1.f + cw)/2.f,
    a0 = 1.f + alpha,
    a1 = -2.f*cw,
    a2 = 1.f - alpha;

  ESP_LOGI("Biquad::HPF", "Coefficients: {%f, %f, %f, %f, %f, %f}",
    b0, b1, b2, a0, a1, a2);

  setCoefficients(b0, b1, b2, a0, a1, a2);
}

Biquad::PeakingEQ::PeakingEQ(float f0, float Q, float A)
  : m_f0{f0}
  , m_Q{Q}
  , m_A{std::pow(10.f, A/40.f)} {
}

void Biquad::PeakingEQ::setSampleRate(int sampleRate) {
  float w0 = 2.f*PI*m_f0/sampleRate;
  float sw = std::sin(w0), cw = std::cos(w0);
  float alpha = sw / (2.f*m_Q);

  //From Audio-EQ-Cookbook
  float 
    b0 = 1.f + alpha*m_A,
    b1 = -2.f * cw,
    b2 = 1.f - alpha*m_A,
    a0 = 1.f + alpha/m_A,
    a1 = -2.f * cw,
    a2 = 1.f - alpha/m_A;

  ESP_LOGI("Biquad::PeakingEQ", "Coefficients: {%f, %f, %f, %f, %f, %f}",
    b0, b1, b2, a0, a1, a2);

  setCoefficients(b0, b1, b2, a0, a1, a2);
}


Biquad::LowShelf::LowShelf(float f0, float Q, float A)
  : m_f0{f0}
  , m_Q{Q}
  , m_A{std::pow(10.f, A/20.f)} {
}

void Biquad::LowShelf::setSampleRate(int sampleRate) {
  float w0 = 2.f*PI*m_f0/sampleRate;
  float sw = std::sin(w0), cw = std::cos(w0);
  float alpha = sw / (2.f*m_Q);
  float factor = 2.f*std::sqrt(m_A)*alpha;

  //From Audio-EQ-Cookbook
  float 
    b0 = m_A*     ( (m_A+1.f)   -   (m_A-1.f)*cw + factor ),
    b1 = 2.f*m_A* ( (m_A-1.f)   -   (m_A+1.f)*cw          ),
    b2 = m_A*     ( (m_A+1.f)   -   (m_A-1.f)*cw - factor ),
    a0 =            (m_A+1.f)   +   (m_A-1.f)*cw + factor  ,
    a1 = -2.f*    ( (m_A-1.f)   +   (m_A+1.f)*cw          ),
    a2 =            (m_A+1.f)   +   (m_A-1.f)*cw - factor  ;

  ESP_LOGI("Biquad::LowShelf", "Coefficients: {%f, %f, %f, %f, %f, %f}",
    b0, b1, b2, a0, a1, a2);

  setCoefficients(b0, b1, b2, a0, a1, a2);
}


Biquad::HighShelf::HighShelf(float f0, float Q, float A)
  : m_f0{f0}
  , m_Q{Q}
  , m_A{std::pow(10.f, A/20.f)} {
}

void Biquad::HighShelf::setSampleRate(int sampleRate) {
  float w0 = 2.f*PI*m_f0/sampleRate;
  float sw = std::sin(w0), cw = std::cos(w0);
  float alpha = sw / (2.f*m_Q);
  float factor = 2.f*std::sqrt(m_A)*alpha;

  //From Audio-EQ-Cookbook
  float 
    b0 = m_A*     ( (m_A+1.f)   +   (m_A-1.f)*cw + factor ),
    b1 = -2.f*m_A*( (m_A-1.f)   +   (m_A+1.f)*cw          ),
    b2 = m_A*     ( (m_A+1.f)   +   (m_A-1.f)*cw - factor ),
    a0 =            (m_A+1.f)   -   (m_A-1.f)*cw + factor  ,
    a1 = 2.f*     ( (m_A-1.f)   -   (m_A+1.f)*cw          ),
    a2 =            (m_A+1.f)   -   (m_A-1.f)*cw - factor  ;

  ESP_LOGI("Biquad::HighShelf", "Coefficients: {%f, %f, %f, %f, %f, %f}",
    b0, b1, b2, a0, a1, a2);

  setCoefficients(b0, b1, b2, a0, a1, a2);
}

}
}
