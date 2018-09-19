#pragma once

extern "C" {
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "driver/i2s.h"
}

#include "I2SPort.hpp"

#include "types.hpp"

namespace I2SInterface {
    class I2SSource {
    public:
        //Buffer size in samples
        I2SSource(I2SPort& i2sPort, int i2sDataPin, int bufferSize);
        virtual ~I2SSource();

        //This method may block until all samples can be copied into the queue
        void writeSamples(const Audio::SampleBuffer& leftSamples,
            const Audio::SampleBuffer& rightSamples);
    
    private:
        static int packSamples(std::vector<uint8_t>& buffer,
            const Audio::SampleBuffer& left, const Audio::SampleBuffer& right,
            int offset);
        
        I2SSourceHandle m_i2sHandle;
        std::vector<uint8_t> m_i2sBuffer;
    };
}
