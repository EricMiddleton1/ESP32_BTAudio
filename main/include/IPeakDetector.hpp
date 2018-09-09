#pragma once

#include <cstdint>

namespace DSP {
  class IPeakDetector {
  public:
    virtual ~IPeakDetector() {}

    virtual uint16_t processSample(int16_t x) = 0;
  };
}
