#include "I2SInput.hpp"

#include <cstring>

extern "C" {
    #include "esp_log.h"
}

namespace AudioInterface {
    I2SInput::I2SInput(i2s_port_t port, int bufferSize,
        Audio::AudioCallback&& cb)
    :   m_port{port}
    ,   m_i2sBuffer(2*sizeof(int16_t)*bufferSize)
    ,   m_leftSamples(bufferSize)
    ,   m_rightSamples(bufferSize)
    ,   m_cb{std::move(cb)}
    ,   m_running{false} {

    }

    I2SInput::~I2SInput() {

    }

    void I2SInput::start() {
        xTaskCreate([](void* instance) {
                reinterpret_cast<I2SInput*>(instance)->inputTask();
            },
            "I2S_Input",
            4096,
            this,
            5, //Priority
            &m_taskHandle
        );

        m_running = true;
    }

    void I2SInput::stop() {
        if(m_running) {
            vTaskDelete(m_taskHandle);
            m_running = false;
        }
    }

    void I2SInput::inputTask() {
        for(;;) {
            size_t bytesRead = 0;

            auto err = i2s_read(m_port,
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
                ESP_LOGE("I2SInput", "i2s_read failed (%d)", err);
            }
        }
    }

    void I2SInput::extractSamples(const std::vector<uint8_t>& buffer,
        Audio::SampleBuffer& left, Audio::SampleBuffer& right) {
        
        for(int i = 0; i < left.size(); ++i) {
            int offset = 4*i;

            std::memcpy(&right[i], buffer.data() + offset, 2);
            std::memcpy(&left[i], buffer.data() + offset + 2, 2);
        }
    }
}
