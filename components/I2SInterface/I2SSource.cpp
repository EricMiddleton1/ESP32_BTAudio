#include "I2SSource.hpp"

#include <algorithm>
#include <cstring>

extern "C" {
    #include "esp_log.h"
}

namespace I2SInterface {
    I2SSource::I2SSource(I2SPort& i2sPort, int i2sDataPin, int bufferSize)
        :   m_i2sHandle{i2sPort, i2sDataPin}
        ,   m_i2sBuffer(2*sizeof(int16_t)*bufferSize) {
    }

    I2SSource::~I2SSource() {

    }

    void I2SSource::writeSamples(const Audio::SampleBuffer& leftSamples,
        const Audio::SampleBuffer& rightSamples) {
        
        for(int offset = 0; offset < leftSamples.size(); ) {
            auto sampleCount = packSamples(m_i2sBuffer,
                leftSamples,
                rightSamples,
                offset);
            
            size_t bytesWritten;
            auto err = i2s_write(m_i2sHandle.port().number(),
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

    int I2SSource::packSamples(std::vector<uint8_t>& buffer,
        const Audio::SampleBuffer& left, const Audio::SampleBuffer& right, int offset) {
        
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
