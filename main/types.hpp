#pragma once

#include <vector>
#include <functional>

namespace DSP {
  enum class Channel {
    Right = 0,
    Left = 1
  };

  enum class StereoMode {
    Mono = 0,
    Stereo = 1
  };

  using SampleBuffer = std::vector<int16_t>;
  using AudioCallback = std::function<void(const SampleBuffer&,
    const SampleBuffer&)>;
}
