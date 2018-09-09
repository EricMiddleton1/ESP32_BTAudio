#include "BiquadFilter.hpp"

#include <cmath>

#include "utility.hpp"

namespace DSP {
namespace Filter {

BiquadFilter::BiquadFilter()
  : m_x1{0}
  , m_x2{0}
  , m_y1{0}
  , m_y2{0} {

  setCoefficients(0.f, 0.f, 0.f, 1.f, 0.f, 0.f);
}

void BiquadFilter::setCoefficients(float b0, float b1, float b2,
  float a0, float a1, float a2) {
  
  m_b0 = convertCoefficient(b0/a0);
  m_b1 = convertCoefficient(b1/a0);
  m_b2 = convertCoefficient(b2/a0);
  m_a1 = convertCoefficient(a1/a0);
  m_a2 = convertCoefficient(a2/a0);

  m_x1 = m_x2 = 0;
  m_y1 = m_y2 = 0;
}

int32_t BiquadFilter::convertCoefficient(float coef) {
  return saturate<int32_t>(coef * (1 << FRAC_BITS_COEF));
}

void BiquadFilter::processSamples(Audio::SampleBuffer& samples) {
  for(int i = 0; i < samples.size(); ++i) {
    int32_t x_scaled = static_cast<int32_t>(samples[i]) << FRAC_BITS_SAMPLE;

    //Direct Form 1
    int64_t intermediate = static_cast<int64_t>(m_b0)*x_scaled
      + static_cast<int64_t>(m_b1)*m_x1 + static_cast<int64_t>(m_b2)*m_x2
      - static_cast<int64_t>(m_a1)*m_y1 - static_cast<int64_t>(m_a2)*m_y2;

    samples[i] = saturate<int16_t>(intermediate >> (FRAC_BITS_COEF + FRAC_BITS_SAMPLE));

    m_y2 = m_y1;
    m_y1 = saturate<int32_t>(intermediate >> FRAC_BITS_COEF);
    m_x2 = m_x1;
    m_x1 = x_scaled;
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

  //From Audio-EQ-Cookbook
  float 
    b0 = (1.f - cw)/2.f,
    b1 = 1.f - cw,
    b2 = (1.f - cw)/2.f,
    a0 = 1.f + alpha,
    a1 = -2.f*cw,
    a2 = 1.f - alpha;

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

  setCoefficients(b0, b1, b2, a0, a1, a2);
}

}
}
