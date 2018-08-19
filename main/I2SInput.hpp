#pragma once

extern "C" {
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "driver/i2s.h"
}

#include "types.hpp"

namespace DSP {
    class I2SInput {
    public:
        I2SInput(i2s_port_t port, int bufferSize,
            AudioCallback&& cb);
        virtual ~I2SInput();

        void start();
        void stop();

        bool running() const;

    private:
        void inputTask();
        static void extractSamples(const std::vector<uint8_t>& buffer,
            SampleBuffer& left, SampleBuffer& right);

        i2s_port_t m_port;
        std::vector<uint8_t> m_i2sBuffer;
        SampleBuffer m_leftSamples, m_rightSamples;
        AudioCallback m_cb;

        TaskHandle_t m_taskHandle;

        bool m_running;
    };
}