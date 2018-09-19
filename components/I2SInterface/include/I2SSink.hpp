#pragma once

extern "C" {
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "driver/i2s.h"
}

#include "I2SPort.hpp"

#include "types.hpp"

namespace I2SInterface {
    class I2SSink {
    public:
        I2SSink(I2SPort& i2sPort, int i2sDataPin, int bufferSize,
            Audio::AudioCallback&& cb);
        virtual ~I2SSink();

        void start();
        void stop();

        bool running() const;

    private:
        void inputTask();
        static void extractSamples(const std::vector<uint8_t>& buffer,
            Audio::SampleBuffer& left, Audio::SampleBuffer& right);

        I2SSinkHandle m_i2sHandle;
        std::vector<uint8_t> m_i2sBuffer;
        Audio::SampleBuffer m_leftSamples, m_rightSamples;
        Audio::AudioCallback m_cb;

        TaskHandle_t m_taskHandle;

        bool m_running;
    };
}
