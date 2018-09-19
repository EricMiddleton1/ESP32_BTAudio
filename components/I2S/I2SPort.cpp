#include "I2SPort.hpp"

extern "C" {
  #include "esp_log.h"
}

namespace I2S {
  Port::Port(i2s_port_t portNum, bool master, int sampleRate,
    int bckPin, int wsPin)
    : m_portNum{portNum}
    , m_master{master}
    , m_sampleRate{sampleRate}
    , m_bckPin{bckPin}
    , m_wsPin{wsPin}
    , m_sourcePin{-1}
    , m_sinkPin{-1}
    , m_running{false} {
  }

  Port::~Port() {
    stop();
  }

  bool Port::start() {
    if(m_running) {
      //Already started
      return true;
    }

    //Compute I2S Mode (master, transmit, receive settings)
    int mode = m_master ? I2S_MODE_MASTER : I2S_MODE_SLAVE;
    if(m_sourcePin != -1) {
      mode |= I2S_MODE_TX;
    }
    if(m_sinkPin != -1) {
      mode |= I2S_MODE_RX;
    }

    ESP_LOGI("I2S::Port", "I2S Mode flags: %x", mode);
    ESP_LOGI("I2S::Port", "Data Out Pin: %d, Data In Pin: %d", m_sourcePin, m_sinkPin);

    i2s_config_t i2sConfig {
      //Mode
      static_cast<i2s_mode_t>(mode),
      //Sample rate
      m_sampleRate,
      //Bits per sample
      I2S_BITS_PER_SAMPLE_16BIT,
      //Channel format
      I2S_CHANNEL_FMT_RIGHT_LEFT,
      //Communication format
      static_cast<i2s_comm_format_t>(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
      //Interrupt alloc flags
      ESP_INTR_FLAG_LEVEL1,
      //DMA buffer count
      4,
      //DMA buffer length
      1024,
      //Use APLL
      1,
      //Fixed MCLK
      256 * m_sampleRate
    };

    i2s_pin_config_t pinConfig {
      //BCK
      m_bckPin,
      //WS
      m_wsPin,
      //Data out
      m_sourcePin,
      //Data in
      m_sinkPin
    };

    //Install driver
    esp_err_t err = i2s_driver_install(m_portNum, &i2sConfig, 0, NULL);
    if(err != ESP_OK) {
      ESP_LOGE("I2S::Port", "i2s_driver_install failed: %d", err);
      return false;
    }

    //Configure pins
    err = i2s_set_pin(m_portNum, &pinConfig);
    if(err != ESP_OK) {
      ESP_LOGE("I2S::Port", "i2s_set_pin failed: %d", err);
      return false;
    }

    //Configure MCLK output on CLK_OUT2
    REG_WRITE(PIN_CTRL, 0xF00);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0RXD_U, FUNC_U0RXD_CLK_OUT2);

    return true;
  }

  bool Port::stop() {
    if(m_running) {
      i2s_driver_uninstall(m_portNum);
      m_running = false;
    }

    return !m_running;
  }

  bool Port::running() const {
    return m_running;
  }

  i2s_port_t Port::number() const {
    return m_portNum;
  }

  bool Port::master() const {
    return m_master;
  }

  int Port::sampleRate() const {
    return m_sampleRate;
  }

  int Port::bckPin() const {
    return m_bckPin;
  }

  int Port::wsPin() const {
    return m_wsPin;
  }

  int Port::sourcePin() const {
    return m_sourcePin;
  }

  int Port::sinkPin() const {
    return m_sinkPin;
  }

  bool Port::registerSource(int dataPin) {
    if(m_sourcePin != -1) {
      //Source already registered
      return false;
    }

    ESP_LOGI("I2S::Port", "Source registered on pin %d", dataPin);

    m_sourcePin = dataPin;
    return true;
  }

  bool Port::deregisterSource() {
    if(m_sourcePin == -1) {
      //No registered source
      return false;
    }

    ESP_LOGI("I2S::Port", "Source deregistered");

    m_sourcePin = -1;
    return true;
  }

  bool Port::registerSink(int dataPin) {
    if(m_sinkPin != -1) {
      //Sink already registered
      return false;
    }

    ESP_LOGI("I2S::Port", "Sink registered on pin %d", dataPin);

    m_sinkPin = dataPin;
    return true;
  }

  bool Port::deregisterSink() {
    if(m_sinkPin == -1) {
      //No registered sink
      return false;
    }

    ESP_LOGI("I2S::Port", "Sink deregistered");

    m_sinkPin = -1;
    return true;
  }


  SourceHandle::SourceHandle(Port& port, int dataPin)
    : m_port{port}
    , m_valid{m_port.registerSource(dataPin)} {
  }

  SourceHandle::~SourceHandle() {
    if(m_valid) {
      m_port.deregisterSource();
    }
  }

  SourceHandle::operator bool() const {
    return m_valid;
  }

  Port& SourceHandle::port() {
    return m_port;
  }

  const Port& SourceHandle::port() const {
    return m_port;
  }

  SinkHandle::SinkHandle(Port& port, int dataPin)
    : m_port{port}
    , m_valid{m_port.registerSink(dataPin)} {
  }

  SinkHandle::~SinkHandle() {
    if(m_valid) {
      m_port.deregisterSink();
    }
  }

  SinkHandle::operator bool() const {
    return m_valid;
  }

  Port& SinkHandle::port() {
    return m_port;
  }

  const Port& SinkHandle::port() const {
    return m_port;
  }

}
