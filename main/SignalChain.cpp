#include "SignalChain.hpp"

#include <cstring>

namespace DSP {
  
SignalChain::SignalChain()
  : m_sampleRate{DEFAULT_SAMPLE_RATE} {
}

void SignalChain::addFilter(std::unique_ptr<IFilter>&& filter) {
  m_filters.emplace_back(std::move(filter));
  m_filters.back()->setSampleRate(m_sampleRate);
}

void SignalChain::setSampleRate(int sampleRate) {
  m_sampleRate = sampleRate;

  for(auto& filter : m_filters) {
    filter->setSampleRate(m_sampleRate);
  }
}

void SignalChain::processSamples(SampleBuffer& samples) {
  for(uint32_t i = 0; i < samples.size(); ++i) {
    for(auto& filter : m_filters) {
      samples[i] = filter->processSample(samples[i]);
    }
  }
}

}
