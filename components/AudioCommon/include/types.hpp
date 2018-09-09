#pragma once

#include <vector>
#include <functional>

namespace Audio {
  enum class StereoMode {
    Mono = 0,
    Stereo = 1
  };

  using SampleBuffer = std::vector<int16_t>;
  using AudioCallback = std::function<void(const SampleBuffer&,
    const SampleBuffer&)>;
}
