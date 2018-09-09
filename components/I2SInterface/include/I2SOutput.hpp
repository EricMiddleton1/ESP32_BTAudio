#pragma once

extern "C" {
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "driver/i2s.h"
}

#include "types.hpp"

namespace I2SInterface {
    class I2SOutput {
    public:
        //Buffer size in samples
        I2SOutput(i2s_port_t, int bufferSize);
        virtual ~I2SOutput();

        //This method may block until all samples can be copied into the queue
        void writeSamples(const Audio::SampleBuffer& leftSamples,
            const Audio::SampleBuffer& rightSamples);
    
    private:
        static int packSamples(std::vector<uint8_t>& buffer,
            const Audio::SampleBuffer& left, const Audio::SampleBuffer& right,
            int offset);
        
        i2s_port_t m_port;
        std::vector<uint8_t> m_i2sBuffer;
    };
}
