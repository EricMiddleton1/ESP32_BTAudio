#include "I2SSink.hpp"

#include <cstring>

extern "C" {
    #include "esp_log.h"
}

namespace I2SInterface {
    I2SSink::I2SSink(I2SPort& i2sPort, int i2sDataPin, int bufferSize,
        Audio::AudioCallback&& cb)
    :   m_i2sHandle{i2sPort, i2sDataPin}
    ,   m_i2sBuffer(2*sizeof(int16_t)*bufferSize)
    ,   m_leftSamples(bufferSize)
    ,   m_rightSamples(bufferSize)
    ,   m_cb{std::move(cb)}
    ,   m_running{false} {

    }

    I2SSink::~I2SSink() {
      stop();
    }

    void I2SSink::start() {
        xTaskCreate([](void* instance) {
                reinterpret_cast<I2SSink*>(instance)->inputTask();
            },
            "I2S_Input",
            4096,
            this,
            5, //Priority
            &m_taskHandle
        );

        m_running = true;
    }

    void I2SSink::stop() {
        if(m_running) {
            vTaskDelete(m_taskHandle);
            m_running = false;
        }
    }

    void I2SSink::inputTask() {
        for(;;) {
            size_t bytesRead = 0;

            auto err = i2s_read(m_i2sHandle.port().number(),
                m_i2sBuffer.data(),
                m_i2sBuffer.size(),
                &bytesRead,
                portMAX_DELAY);
            
            if(err == ESP_OK) {
                extractSamples(m_i2sBuffer, m_leftSamples, m_rightSamples);

                if(m_cb) {
                    m_cb(m_leftSamples, m_rightSamples);
                }
            }
            else {
                ESP_LOGE("I2SSink", "i2s_read failed (%d)", err);
            }
        }
    }

    void I2SSink::extractSamples(const std::vector<uint8_t>& buffer,
        Audio::SampleBuffer& left, Audio::SampleBuffer& right) {
        
        for(int i = 0; i < left.size(); ++i) {
            int offset = 4*i;

            std::memcpy(&right[i], buffer.data() + offset, 2);
            std::memcpy(&left[i], buffer.data() + offset + 2, 2);
        }
    }
}
