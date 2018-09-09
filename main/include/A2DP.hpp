#pragma once

extern "C" {
  #include "esp_log.h"
}

namespace Bluetooth {
  class A2DP {
  public:
    A2DP();

    void start();
    void stop();

    void setName(const std::string& name);
    const std::string& getName() const;
  private:
    std::string m_Name;
  };
}
