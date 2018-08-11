// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

extern "C" {
  #include "freertos/FreeRTOS.h"
  #include "freertos/task.h"
  #include "nvs.h"
  #include "nvs_flash.h"
  #include "esp_system.h"
  #include "esp_log.h"

  #include "esp_bt.h"
  #include "bt_app_core.h"
  #include "bt_app_av.h"
  #include "esp_bt_main.h"
  #include "esp_bt_device.h"
  #include "esp_gap_bt_api.h"
  #include "esp_a2dp_api.h"
  #include "esp_avrc_api.h"
  #include "driver/i2s.h"
}

#include <memory>

#include "SignalChain.hpp"
#include "Gain.hpp"
#include "BiquadFilter.hpp"
#include "types.hpp"

/* event for handler "bt_av_hdl_stack_up */
enum {
    BT_APP_EVT_STACK_UP = 0,
};

/* handler for bluetooth stack enabled events */
static void bt_av_hdl_stack_evt(uint16_t event, void *p_param);

static void task_info(void* arg);

extern "C" void app_main();

std::unique_ptr<DSP::SignalChain> signalChainLeft, signalChainRight;

void app_main()
{
  signalChainLeft = std::make_unique<DSP::SignalChain>();
  signalChainRight = std::make_unique<DSP::SignalChain>();

  float fc = 1000.f;

  signalChainLeft->addFilter(std::make_unique<DSP::Filter::Gain>(-6.f));
  //signalChainLeft->addFilter(std::make_unique<DSP::Filter::Biquad::HPF>(50.f, 1.f));
  //signalChainLeft->addFilter(std::make_unique<DSP::Filter::Biquad::LowShelf>(
    //120.f, 0.7071f, 3.f));
  //signalChainLeft->addFilter(std::make_unique<DSP::Filter::Biquad::LPF>(fc, 0.7071f));
  
  signalChainRight->addFilter(std::make_unique<DSP::Filter::Gain>(-6.f));
  //signalChainRight->addFilter(std::make_unique<DSP::Filter::Biquad::HPF>(fc, 0.7071f));

  /*
  signalChainLeft->addFilter(std::make_unique<DSP::Filter::Biquad::PeakingEQ>
    (160.f, 1.5f, -3.f));
  signalChainRight->addFilter(std::make_unique<DSP::Filter::Biquad::PeakingEQ>
    (160.f, 1.5f, -3.f));

  signalChainLeft->addFilter(std::make_unique<DSP::Filter::Biquad::PeakingEQ>
    (1250.f, 2.f, -2.f));
  signalChainRight->addFilter(std::make_unique<DSP::Filter::Biquad::PeakingEQ>
    (1250.f, 2.f, -2.f));

  signalChainLeft->addFilter(std::make_unique<DSP::Filter::Biquad::PeakingEQ>
    (1800.f, 3.f, -4.f));
  signalChainRight->addFilter(std::make_unique<DSP::Filter::Biquad::PeakingEQ>
    (1800.f, 3.f, -4.f));

  signalChainLeft->addFilter(std::make_unique<DSP::Filter::Biquad::PeakingEQ>
    (6000.f, 3.f, -1.9f));
  signalChainRight->addFilter(std::make_unique<DSP::Filter::Biquad::PeakingEQ>
    (6000.f, 3.f, -1.9f));

  signalChainLeft->addFilter(std::make_unique<DSP::Filter::Biquad::PeakingEQ>
    (9500.f, 1.f, -6.2f));
  signalChainRight->addFilter(std::make_unique<DSP::Filter::Biquad::PeakingEQ>
    (9500.f, 1.f, -6.2f));

  signalChainLeft->addFilter(std::make_unique<DSP::Filter::Biquad::PeakingEQ>
    (17000.f, 1.f, -6.3f));
  signalChainRight->addFilter(std::make_unique<DSP::Filter::Biquad::PeakingEQ>
    (17000.f, 1.f, -6.3f));

  signalChainLeft->addFilter(std::make_unique<DSP::Filter::Biquad::HighShelf>
    (100.f, 0.7071f, -2.5f));
  signalChainRight->addFilter(std::make_unique<DSP::Filter::Biquad::HighShelf>
    (100.f, 0.7071f, -2.5f));
  */

/*
  //2nd Order LP filter
  signalChainLeft->addFilter(std::make_unique<DSP::Filter::Biquad::LPF>
    (2500.f, 0.7071f));
  //Additional DSP correction
  signalChainLeft->addFilter(std::make_unique<DSP::Filter::Biquad::PeakingEQ>
    (160.f, 1.5f, -3.f));
  signalChainLeft->addFilter(std::make_unique<DSP::Filter::Biquad::PeakingEQ>
    (1250.f, 2.f, -2.f));
  signalChainLeft->addFilter(std::make_unique<DSP::Filter::Biquad::PeakingEQ>
    (1800.f, 3.f, -4.f));
  signalChainLeft->addFilter(std::make_unique<DSP::Filter::Biquad::HighShelf>
    (100.f, 0.7071f, -2.5f));

  //2nd Order HP filter
  signalChainRight->addFilter(std::make_unique<DSP::Filter::Biquad::HPF>
    (2500.f, 0.7071f));
  //Additional DSP correction
  signalChainRight->addFilter(std::make_unique<DSP::Filter::Biquad::PeakingEQ>
    (1250.f, 2.f, -2.f));
  signalChainRight->addFilter(std::make_unique<DSP::Filter::Biquad::PeakingEQ>
    (1800.f, 3.f, -4.f));
  signalChainRight->addFilter(std::make_unique<DSP::Filter::Biquad::PeakingEQ>
    (6000.f, 3.f, -1.9f));
  signalChainRight->addFilter(std::make_unique<DSP::Filter::Biquad::PeakingEQ>
    (9500.f, 1.f, -6.2f));
  signalChainRight->addFilter(std::make_unique<DSP::Filter::Biquad::PeakingEQ>
    (17000.f, 1.f, -6.3f));
  //Gain correction (from 1R resistor + asymptote of 100hz high shelf)
  //(0.8 amplitude = -1.93820) + (-2.5) dB
  signalChainRight->addFilter(std::make_unique<DSP::Filter::Gain>(-4.4382f));
*/
  set_signalChain(signalChainLeft.get(), signalChainRight.get());
  
  set_stereo_mode(DSP::StereoMode::Mono);

    /* Initialize NVS — it is used to store PHY calibration data */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    i2s_config_t i2s_config{
      //mode
      static_cast<i2s_mode_t>(I2S_MODE_MASTER | I2S_MODE_TX),
      //sample_rate
      44100,
      //bits_per_sample
      I2S_BITS_PER_SAMPLE_16BIT,
      //channel_format
      I2S_CHANNEL_FMT_RIGHT_LEFT,
      //communication_format
      static_cast<i2s_comm_format_t>(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
      //intr_alloc_flags
      ESP_INTR_FLAG_LEVEL1,
      //dma_buf_count (previously 6)
      4,
      //dma_buf_len (previously 60)
      //4096 is maximum observed single a2dp data packet length
      1024,
      //use_apll
      0,
      //fixed_mclk
      0
    };

    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    
    i2s_pin_config_t pin_config = {
        //bck_io_num
        CONFIG_I2S_BCK_PIN,
        //ws_io_num
        CONFIG_I2S_LRCK_PIN,
        //data_out_num
        CONFIG_I2S_DATA_PIN,
        //data_in_num
        -1                                                       //Not used
    };

    i2s_set_pin(I2S_NUM_0, &pin_config);

    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_BLE));

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    if (esp_bt_controller_init(&bt_cfg) != ESP_OK) {
        ESP_LOGE(BT_AV_TAG, "%s initialize controller failed\n", __func__);
        return;
    }

    if (esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT) != ESP_OK) {
        ESP_LOGE(BT_AV_TAG, "%s enable controller failed\n", __func__);
        return;
    }

    if (esp_bluedroid_init() != ESP_OK) {
        ESP_LOGE(BT_AV_TAG, "%s initialize bluedroid failed\n", __func__);
        return;
    }

    if (esp_bluedroid_enable() != ESP_OK) {
        ESP_LOGE(BT_AV_TAG, "%s enable bluedroid failed\n", __func__);
        return;
    }

    /* create application task */
    bt_app_task_start_up();

    /* Bluetooth device name, connection mode and profile set up */
    bt_app_work_dispatch(bt_av_hdl_stack_evt, BT_APP_EVT_STACK_UP, NULL, 0, NULL);

    xTaskCreate(task_info, "task_info", 4096, NULL, 5, NULL);
}


static void bt_av_hdl_stack_evt(uint16_t event, void *p_param)
{
    ESP_LOGD(BT_AV_TAG, "%s evt %d", __func__, event);
    switch (event) {
    case BT_APP_EVT_STACK_UP: {
        /* set up device name */
        char *dev_name = "ICEEBOX";
        esp_bt_dev_set_device_name(dev_name);

        /* initialize A2DP sink */
        esp_a2d_register_callback(&bt_app_a2d_cb);
        esp_a2d_sink_register_data_callback(bt_app_a2d_data_cb);
        esp_a2d_sink_init();

        /* initialize AVRCP controller */
        esp_avrc_ct_init();
        esp_avrc_ct_register_callback(bt_app_rc_ct_cb);

        /* set discoverable and connectable mode, wait to be connected */
        esp_bt_gap_set_scan_mode(ESP_BT_SCAN_MODE_CONNECTABLE_DISCOVERABLE);
        break;
    }
    default:
        ESP_LOGE(BT_AV_TAG, "%s unhandled evt %d", __func__, event);
        break;
    }
}

static void task_info(void* arg) {
  for(;;) {
    //ESP_LOGI("INFO", "Free Heap: %u bytes",
      //heap_caps_get_free_size(MALLOC_CAP_8BIT));

    ESP_LOGI("", "Left SignalChain (%uus, %uus), Right SignalChain "
      "(%uus, %uus), %ub", signalChainLeft->avgProcTime(), signalChainLeft->maxProcTime(),
      signalChainRight->avgProcTime(), signalChainRight->maxProcTime(),
      signalChainLeft->avgBufferSize());

    vTaskDelay(100 / portTICK_RATE_MS);
  }
}
