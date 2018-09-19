#include "I2SPort.hpp"

namespace I2SInterface {
  I2SPort::I2SPort(i2s_port_t portNum, bool master, int sampleRate,
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

  I2SPort::~I2SPort() {
    stop();
  }

  bool I2SPort::start() {
    //Compute I2S Mode (master, transmit, receive settings)
    int mode = m_master ? I2S_MODE_MASTER : I2S_MODE_SLAVE;
    if(m_sourcePin != -1) {
      mode |= I2S_MODE_TX;
    }
    if(m_sinkPin != -1) {
      mode |= I2S_MODE_RX;
    }

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
      return false;
    }

    //Configure pins
    err = i2s_set_pin(m_portNum, &pinConfig);
    if(err != ESP_OK) {
      return false;
    }

    //Configure MCLK output on CLK_OUT2
    REG_WRITE(PIN_CTRL, 0xF00);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0RXD_U, FUNC_U0RXD_CLK_OUT2);

    return true;
  }

  bool I2SPort::stop() {
    if(m_running) {
      i2s_driver_uninstall(m_portNum);
      m_running = false;
    }

    return !m_running;
  }

  bool I2SPort::running() const {
    return m_running;
  }

  i2s_port_t I2SPort::number() const {
    return m_portNum;
  }

  bool I2SPort::master() const {
    return m_master;
  }

  int I2SPort::sampleRate() const {
    return m_sampleRate;
  }

  int I2SPort::bckPin() const {
    return m_bckPin;
  }

  int I2SPort::wsPin() const {
    return m_wsPin;
  }

  int I2SPort::sourcePin() const {
    return m_sourcePin;
  }

  int I2SPort::sinkPin() const {
    return m_sinkPin;
  }

  bool I2SPort::registerSource(int dataPin) {
    if(m_sourcePin) {
      //Source already registered
      return false;
    }

    m_sourcePin = dataPin;
    return true;
  }

  bool I2SPort::deregisterSource() {
    if(m_sourcePin == -1) {
      //No registered source
      return false;
    }

    m_sourcePin = -1;
    return true;
  }

  bool I2SPort::registerSink(int dataPin) {
    if(m_sinkPin) {
      //Sink already registered
      return false;
    }

    m_sinkPin = dataPin;
    return true;
  }

  bool I2SPort::deregisterSink() {
    if(m_sinkPin == -1) {
      //No registered sink
      return false;
    }

    m_sinkPin = -1;
    return true;
  }


  I2SSourceHandle::I2SSourceHandle(I2SPort& port, int dataPin)
    : m_port{port}
    , m_valid{m_port.registerSource(dataPin)} {
  }

  I2SSourceHandle::~I2SSourceHandle() {
    if(m_valid) {
      m_port.deregisterSource();
    }
  }

  I2SSourceHandle::operator bool() const {
    return m_valid;
  }

  I2SPort& I2SSourceHandle::port() {
    return m_port;
  }

  const I2SPort& I2SSourceHandle::port() const {
    return m_port;
  }

  I2SSinkHandle::I2SSinkHandle(I2SPort& port, int dataPin)
    : m_port{port}
    , m_valid{m_port.registerSink(dataPin)} {
  }

  I2SSinkHandle::~I2SSinkHandle() {
    if(m_valid) {
      m_port.deregisterSink();
    }
  }

  I2SSinkHandle::operator bool() const {
    return m_valid;
  }

  I2SPort& I2SSinkHandle::port() {
    return m_port;
  }

  const I2SPort& I2SSinkHandle::port() const {
    return m_port;
  }

}
