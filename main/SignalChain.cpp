#include "SignalChain.hpp"

#include <cstring>

namespace DSP {
  
SignalChain::SignalChain()
  : m_outBuffer(START_BUFFER_SIZE)
  , m_sampleRate{DEFAULT_SAMPLE_RATE} {
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

uint8_t* SignalChain::processSamples(const uint8_t* dataIn, uint32_t length) {
  if(m_outBuffer.size() < length) {
    m_outBuffer.resize(length);
  }

  std::memcpy(m_outBuffer.data(), dataIn, length);

  for(auto& filter : m_filters) {
    filter->processSamples(m_outBuffer.data(), length);
  }

  return m_outBuffer.data();
}

}
