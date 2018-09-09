#include "WM8731.hpp"

extern "C" {
    #include "driver/i2c.h"
    #include "esp_log.h"
}

WM8731::WM8731() {

}

void WM8731::start() {
    i2cInit();

    configureDevice();
}

void WM8731::i2cInit() {
    i2c_config_t config;
    config.mode = I2C_MODE_MASTER;
    config.sda_io_num = GPIO_NUM_21; //SDA = GPIO21
    config.sda_pullup_en = GPIO_PULLUP_DISABLE;
    config.scl_io_num = GPIO_NUM_22; //SCL = GPIO22
    config.scl_pullup_en = GPIO_PULLUP_DISABLE;
    config.master.clk_speed = 50000; //Standard speed I2C
    
    //Configure I2C0
    i2c_param_config(I2C_NUM_0, &config);

    //Install driver on I2C0
    i2c_driver_install(I2C_NUM_0, config.mode, 0, 0, 0);
}

void WM8731::configureDevice() {
    int err = ESP_OK;

    /* Left Line In - 0x00
     * Volume: -9dB (10001)
     * Mute: off (0)
     * Copy left data to right: true (1)
     *
     * Value = 0b10xx10001 = 0x111
     */
    err = writeByte(DEVICE_ADDR, 0x00, 0x111);
    ESP_LOGI("INFO", "Setting Left Line In register (%d)", err);

    /* Left Headphone out - 0x02 (0b0000010)
     * Volume: MUTE (0000000)
     * Zero Cross Detect: enable (1)
     * Copy left data to right: true (1)
     *
     * Value = 0b110000000 = 0x180
     */
    err = writeByte(DEVICE_ADDR, 0x02, 0x180);
    ESP_LOGI("INFO", "Setting Left Headphone Out register (%d)", err);

    /* Analog Audio Path Control - 0x04 (0000100)
     * Bypass Switch (Line-in to Line-out): Disable (0)
     * DAC Select: Select (1)
     * Side Tone Switch: Disable (0)
     *
     * Value = 0b010000 = 0x10
     */
    err = writeByte(DEVICE_ADDR, 0x04, 0x10);
    ESP_LOGI("INFO", "Setting Analog Audio Path Control register (%d)", err);

    /* Digital Audio Path Control - 0x05 (0000101)
     * ADC High Pass Filter: Enable (1)
     * De-emphasis Control: Disable (00)
     * DAC Soft Mute Control: Disable (0)
     * Store DC offset when HP filter disabled: Store (1)
     *
     * Value = 0b10001 = 0x11
     */
    err = writeByte(DEVICE_ADDR, 0x05, 0x11);
    ESP_LOGI("INFO", "Setting Digital Audio Path Control register (%d)", err);

    /* Power Down Control - 0x06 (0b0000110)
     * Line-In: On (0)
     * Mic input and bias: Off (1)
     * ADC: On (0)
     * DAC: On (0)
     * Line-Out: On (0)
     * Oscillator: Off (1) //On (0)
     * CLKOUT: Off (1)
     * Device Power: On (0)
     *
     * Value = 0b01100010 = 0x62 //0b01000010 = 0x42
     */
    err = writeByte(DEVICE_ADDR, 0x06, 0x62);
    ESP_LOGI("INFO", "Setting Power Down Control register (%d)", err);

    /* Digital Audio Interface Format - 0x07 (0b0000111)
     * Format: I2S (0b10)
     * Input Audio Data Bit Length: 16 bits (0b00)
     * L/R Phase: Default (0)
     * L/R Swap: False (0)
     * Master: False (0)
     * Bit Clock Invert: False (0)
     *
     * Value = 0b00000010 = 0x02
     */
    err = writeByte(DEVICE_ADDR, 0x07, 0x02);
    ESP_LOGI("INFO", "Setting Digital Audio Interface Format register (%d)", err);

    /* Sampling Control - 0x08 (0b0001000)
     * Mode Select: Normal (0) //USB (1)
     * Base Over-Sampling Rate: 0 //1
     * Sample Rate Control: 0b1000
     * Core Clock Divider Select: 0
     * CLKOUT divider select: 0
     *
     * Value = 0b00100000 = 0x20 //0b00100011 = 0x23
     */
    err = writeByte(DEVICE_ADDR, 0x08, 0x20);
    ESP_LOGI("INFO", "Setting Sampling Control register (%d)", err);

    /* Active Control - 0x09 (0b0001001)
     * Activate Interface: True (1)
     *
     * Value = 0b1 = 0x01
     */
    err = writeByte(DEVICE_ADDR, 0x09, 0x01);
    ESP_LOGI("INFO", "Setting Active Control register (%d)", err);
}

int WM8731::writeByte(uint8_t deviceAddr, uint8_t regAddr, uint16_t value) {
    uint8_t data0 = (regAddr << 1) | (value >> 8); //addr[6:0], value[8]
    uint8_t data1 = value & 0xFF; // value[7:0]

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    
    i2c_master_write_byte(cmd, (deviceAddr << 1) | 0x00, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, data0, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, data1, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    int err = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    return err;
}
