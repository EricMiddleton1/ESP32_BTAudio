#pragma once

extern "C" {
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "driver/i2s.h"
}

#include "types.hpp"

namespace AudioInterface {
    class I2SInput {
    public:
        I2SInput(i2s_port_t port, int bufferSize,
            Audio::AudioCallback&& cb);
        virtual ~I2SInput();

        void start();
        void stop();

        bool running() const;

    private:
        void inputTask();
        static void extractSamples(const std::vector<uint8_t>& buffer,
            Audio::SampleBuffer& left, Audio::SampleBuffer& right);

        i2s_port_t m_port;
        std::vector<uint8_t> m_i2sBuffer;
        Audio::SampleBuffer m_leftSamples, m_rightSamples;
        Audio::AudioCallback m_cb;

        TaskHandle_t m_taskHandle;

        bool m_running;
    };
}
