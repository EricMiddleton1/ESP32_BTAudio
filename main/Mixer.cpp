#include "Mixer.hpp"

namespace DSP {
  namespace Mixer {
    void mix(I2SBuffer& buffer) {
      auto left = buffer.getSamples(Channel::Left);
      auto right = buffer.getSamples(Channel::Right);

      for(uint32_t i = 0; i < left.size(); ++i) {
        auto mixed = (left[i] + right[i]) / 2;
        left[i] = mixed;
        right[i] = mixed;
      }
    }
  }
}
