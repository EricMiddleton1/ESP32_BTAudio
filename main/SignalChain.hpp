#pragma once

#include <vector>
#include <memory>

#include "IFilter.hpp"
#include "types.hpp"

namespace DSP {

  class SignalChain {
  public:
    SignalChain();

    void addFilter(std::unique_ptr<IFilter>&& filter);

    void setSampleRate(int sampleRate);

    void processSamples(SampleBuffer& samples);

    uint32_t avgProcTime() const;
    uint32_t maxProcTime() const;
    uint32_t avgBufferSize() const;
  private:
    const int DEFAULT_SAMPLE_RATE = 44100;

    std::vector<std::unique_ptr<IFilter>> m_filters;
    int m_sampleRate;

    uint32_t m_avgProcTime;
    uint32_t m_maxProcTime;
    uint32_t m_avgBufferSize;
  };
}
