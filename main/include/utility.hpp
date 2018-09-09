#pragma once

#include <limits>
#include <algorithm>

namespace DSP {
  const float PI = 3.141592654f;

  template<typename OutType, typename InType>
  OutType saturate(InType in) {
    in = std::max(static_cast<InType>(std::numeric_limits<OutType>::min()), in);
    in = std::min(static_cast<InType>(std::numeric_limits<OutType>::max()), in);

    return static_cast<OutType>(in);
  }
}
