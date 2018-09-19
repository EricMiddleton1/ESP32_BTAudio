#pragma once


extern "C" {
  #include <freertos/FreeRTOS.h>
  #include <freertos/task.h>
  #include <driver/i2s.h>
}

namespace I2S {
  class Port {
  public:
    Port(i2s_port_t portNum, bool master, int sampleRate, int bckPin, int wsPin);
    ~Port();

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
    friend class SourceHandle;
    friend class SinkHandle;

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

  class SourceHandle {
  public:
    SourceHandle(Port& port, int dataPin);
    SourceHandle(SourceHandle&) = delete;
    ~SourceHandle();

    operator bool() const;

    Port& port();
    const Port& port() const;

  private:
    Port& m_port;
    bool m_valid;
  };

  class SinkHandle {
  public:
    SinkHandle(Port& port, int dataPin);
    SinkHandle(SinkHandle&) = delete;
    ~SinkHandle();

    operator bool() const;

    Port& port();
    const Port& port() const;

  private:
    Port& m_port;
    bool m_valid;
  };
}
