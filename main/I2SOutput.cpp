#include "I2SOutput.hpp"

#include <algorithm>
#include <cstring>

extern "C" {
    #include "esp_log.h"
}

namespace DSP {
    I2SOutput::I2SOutput(i2s_port_t port, int bufferSize)
        :   m_port{port}
        ,   m_i2sBuffer(2*sizeof(int16_t)*bufferSize) {
    }

    I2SOutput::~I2SOutput() {

    }

    void I2SOutput::writeSamples(const SampleBuffer& leftSamples,
        const SampleBuffer& rightSamples) {
        
        for(int offset = 0; offset < leftSamples.size(); ) {
            auto sampleCount = packSamples(m_i2sBuffer,
                leftSamples,
                rightSamples,
                offset);
            
            size_t bytesWritten;
            auto err = i2s_write(m_port,
                m_i2sBuffer.data(),
                2*sizeof(int16_t)*sampleCount,
                &bytesWritten,
                portMAX_DELAY);
            
            if(err != ESP_OK) {
                ESP_LOGE("i2s_write", "Error writing I2S samples (%d)",
                    err);
            }
            
            offset += sampleCount;
        }
    }

    int I2SOutput::packSamples(std::vector<uint8_t>& buffer,
        const SampleBuffer& left, const SampleBuffer& right, int offset) {
        
        int maxSamples = std::min(buffer.size()/(sizeof(int16_t)*2),
            left.size());
        
        for(int i = 0; i < maxSamples; ++i) {
            int offset = 2*sizeof(int16_t)*i;

            std::memcpy(buffer.data() + offset, &right[i], sizeof(int16_t));
            std::memcpy(buffer.data() + offset + sizeof(int16_t), &left[i],
                sizeof(int16_t));
        }

        return maxSamples;
    }
}