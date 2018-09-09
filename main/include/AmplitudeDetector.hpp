#pragma once


#include "IPeakDetector.hpp"

namespace DSP {
  class AmplitudeDetector : public IPeakDetector {
  public:
    AmplitudeDetector(float alpha);

    float alpha() const;
    void alpha(float alpha);

    uint16_t processSample(int16_t x) override;

    uint16_t peak() const;

  private:
    const int BIT_DEPTH = 15;

    float m_flt_alpha;
    uint16_t m_fix_alpha, m_fix_invalpha;

    uint16_t m_peak;
  };
}
