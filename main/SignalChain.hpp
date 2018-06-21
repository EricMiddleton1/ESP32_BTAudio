#pragma once

#include <vector>
#include <memory>

#include "IFilter.hpp"
#include "SampleBuffer.hpp"

namespace DSP {

  class SignalChain {
  public:
    SignalChain();

    void addFilter(std::unique_ptr<IFilter>&& filter);

    void setSampleRate(int sampleRate);

    void processSamples(SampleBuffer& samples);
  private:
    const int DEFAULT_SAMPLE_RATE = 48000;

    std::vector<std::unique_ptr<IFilter>> m_filters;
    int m_sampleRate;
  };
}
