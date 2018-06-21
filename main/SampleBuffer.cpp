#include "SampleBuffer.hpp"
#include "I2SBuffer.hpp"


namespace DSP {

SampleBuffer::SampleBuffer(I2SBuffer& buffer, Channel ch)
  : m_buffer{buffer}
  , m_ch{ch} {
}

int16_t& SampleBuffer::operator[](uint32_t i) {
  return m_buffer.getSample(m_ch, i);
}

const int16_t& SampleBuffer::operator[](uint32_t i) const {
  return m_buffer.getSample(m_ch, i);
}

uint32_t SampleBuffer::size() const {
  return m_buffer.size();
}

}
