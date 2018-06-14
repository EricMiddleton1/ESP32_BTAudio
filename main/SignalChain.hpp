#pragma once

#include <vector>
#include <memory>

#include "IFilter.hpp"

namespace DSP {

  class SignalChain {
  public:
    SignalChain();

    void addFilter(std::unique_ptr<IFilter>&& filter);

    void setSampleRate(int sampleRate);

    uint8_t* processSamples(const uint8_t* dataIn, uint32_t length);
  private:
    const int START_BUFFER_SIZE = 4096;
    const int DEFAULT_SAMPLE_RATE = 48000;

    std::vector<std::unique_ptr<IFilter>> m_filters;
    std::vector<uint8_t> m_outBuffer;
    int m_sampleRate;
  };
}
