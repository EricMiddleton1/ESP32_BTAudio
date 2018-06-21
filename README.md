ESP32 Bluetooth Audio DSP
======================

This project is uses an ESP32 as a bluetooth A2DP audio sink, an I2S audio source, and a WiFi AP hosting a web interface. The signal chain from A2DP sink to I2S source allows for software DSP blocks to be inserted and configured at runtime (and eventually controlled through the web interface).

The I2S output is designed for output to a standard I2S stereo DAC (but should probably work with other I2S-based devices). The default I2S connections are shown below, but these can be changed in menuconfig:

| ESP pin   | I2S signal   |
| :-------- | :----------- |
| GPIO22    | LRCK         |
| GPIO25    | DATA         |
| GPIO26    | BCK          |

After the program is started, other bluetooth devices such as smart phones can discover a device named "ICEBOX". Once a connection is established, audio data can be transmitted. This will be visible in the application log including a count of audio data packets and debug information regarding the DSP blocks in the signal chain.
