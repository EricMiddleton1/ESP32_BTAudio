#include "I2SBuffer.hpp"

#include <cstring>

namespace DSP {

I2SBuffer::I2SBuffer(int startSize)
  : m_buffer(startSize)
  , m_sampleCount{0} {
}

uint32_t I2SBuffer::size() const {
  return m_sampleCount;
}

SampleBuffer I2SBuffer::getSamples(Channel ch) {
  return {*this, ch};
}

void I2SBuffer::set(const uint8_t* buffer, uint32_t size) {
  if(m_buffer.size() < size) {
    m_buffer.resize(size);
  }

  std::memcpy(m_buffer.data(), buffer, size);
  m_sampleCount = size/4;
}

const uint8_t* I2SBuffer::get() const {
  return m_buffer.data();
}

int16_t& I2SBuffer::getSample(Channel ch, uint32_t i) {
  //2-byte samples interleaved right-left
  uint32_t offset = 4*i + 2*static_cast<uint32_t>(ch);

  return *reinterpret_cast<int16_t*>(m_buffer.data() + offset);
}

}
