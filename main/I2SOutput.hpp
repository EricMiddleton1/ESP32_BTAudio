#pragma once

extern "C" {
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "driver/i2s.h"
}

#include "types.hpp"

namespace DSP {
    class I2SOutput {
    public:
        //Buffer size in samples
        I2SOutput(i2s_port_t, int bufferSize);
        virtual ~I2SOutput();

        //This method may block until all samples can be copied into the queue
        void writeSamples(const SampleBuffer& leftSamples,
            const SampleBuffer& rightSamples);
    
    private:
        static int packSamples(std::vector<uint8_t>& buffer,
            const SampleBuffer& left, const SampleBuffer& right, int offset);
        
        i2s_port_t m_port;
        std::vector<uint8_t> m_i2sBuffer;
    };
}