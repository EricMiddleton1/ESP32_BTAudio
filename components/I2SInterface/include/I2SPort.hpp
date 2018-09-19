#pragma once


extern "C" {
  #include <freertos/FreeRTOS.h>
  #include <freertos/task.h>
  #include <driver/i2s.h>
}

namespace I2SInterface {
  class I2SPort {
  public:
    I2SPort(i2s_port_t portNum, bool master, int sampleRate, int bckPin, int wsPin);
    ~I2SPort();

    bool start();
    bool stop();
    bool running() const;

    i2s_port_t number() const;
    bool master() const;
    int sampleRate() const;
    int bckPin() const;
    int wsPin() const;
    int sourcePin() const;
    int sinkPin() const;

  protected:
    friend class I2SSourceHandle;
    friend class I2SSinkHandle;

    bool registerSource(int dataPin);
    bool deregisterSource();

    bool registerSink(int dataPin);
    bool deregisterSink();

  private:
    i2s_port_t m_portNum;
    bool m_master;
    int m_sampleRate;
    int m_bckPin, m_wsPin, m_sourcePin, m_sinkPin;

    bool m_running;
  };

  class I2SSourceHandle {
  public:
    I2SSourceHandle(I2SPort& port, int dataPin);
    I2SSourceHandle(I2SSourceHandle&) = delete;
    ~I2SSourceHandle();

    operator bool() const;

    I2SPort& port();
    const I2SPort& port() const;

  private:
    I2SPort& m_port;
    bool m_valid;
  };

  class I2SSinkHandle {
  public:
    I2SSinkHandle(I2SPort& port, int dataPin);
    I2SSinkHandle(I2SSinkHandle&) = delete;
    ~I2SSinkHandle();

    operator bool() const;

    I2SPort& port();
    const I2SPort& port() const;

  private:
    I2SPort& m_port;
    bool m_valid;
  };
}
