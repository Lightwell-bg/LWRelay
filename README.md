##Device for controlling heating/cooling devices up to 3.5 kW. Based on ESP8266 for Home Automation (IoT).

**Built-in Wi-Fi allows you to control devices connected to the controller via any web browser or using an application for a mobile phone / tablet.
Connecting the controller to the Internet will allow you to control the heater from anywhere outside the home via MQTT.
Special application for android devices [https://play.google.com/store/apps/details?id=eu.ledlightwell.lwheater](https://play.google.com/store/apps/details?id=eu.ledlightwell.lwheater)
**Features**
- automatic turning on / of the device at night (reduced rates)
- turning on / off the device according to a specified schedule (4 time intervals)
- Always precise time because of synchronization with precise time clocks (DS3231) or NTP servers on the Internet
- forced switching on / off the device with a button for a specified interval
- forced remote on / off from anywhere in the world via the Internet
- counter of worked hours of the controlled device
- control and management via WEB-interface from PC, tablet or smartphone
- sending temperature, humidity and barometric pressure data to MQTT and Thingspeak.com server
- viewing the current temperature and humidity and their changes in the form of a graph (option)
- firmware update via Wi-Fi
- information display on 3 languages: English, Russian, Bulgarian

**Specifications**
- CPU: Tensilica L106 32-bit processor (ESP8266)
- Connectivity: WiFi 802.11 b/g/n
- Peripherals: BME280 or DHT22 sensor (temperature, humidity and Barometric pressure), UART pins, button, two slots for I2C sensors
- Compatibility: Arduino IDE, PlatformIO, MQTT, and any modern web browser
- Upgradable: The software can be upgraded, either using an USB to UART module and over-the-air (OTA)
- Powered 220V/50Hz
- Maximum power of the controlled device 16 A
- Dimensions: 70 mm x 50 mm and 82 mm x 64 mm (for DIN rail)
## Flashing Firmware to ESP8266
### Tool Downloading
The ESP8266 is a low-cost Wi-Fi microchip produced by Espressif Systems. Espressif Systems also developed a ESP Flash Download Tool to update the firmware of ESP8266.
ESP8266 Firmware can be installed for two reasons: If we want to install the new firmware or to update the ESP8266 Firmware to its latest version.
visit the official link, in order to download the ESP Flash Download Tool, click on Tools section to select the tool [https://www.espressif.com/en/support/download/other-tools](https://www.espressif.com/en/support/download/other-tools "ESP Flash Download Tool")
![](https://github.com/Lightwell-bg/LWRelay/blob/master/images/wsite.png?raw=true)
### Tool Installation
After Downloading the tool, extracts the files and open the “flash_download_tools_v3.6.5” application file.
![](https://github.com/Lightwell-bg/LWRelay/blob/master/images/1.png?raw=true)
After opening the application file, it will display various options like ESP8266 Download Tool, ESP8285 Download Tool, ESP32 Download Tool and ESP32D2WD Download Tool. Now click on “ESP8266 Download Tool” because we are flashing the firmware to ESP8266.
### Tool Settings
As soon as the ESP8266 Download Tool opens, we have to select and upload the two firmware bin files at two different addresses under “SPIDownlaod“. Select SPI SPEED as “40MHz”, SPI MODE as “QIO”, FLASH SIZE as “4Mbit”, the COM port ESP8266 and BAUD as “921600”.
For bin file addresses refer the above table at “Download Latest Version Firmware” section.
![](https://github.com/Lightwell-bg/LWRelay/blob/master/images/ToolSettings.jpg?raw=true)
Now click on START icon to begin the firmware flashing to ESP8266. As soon as the flashing process starts, the tool will display two MAC Addresses as AP and STA and also displays the information about Module in the DETECTED INFO section.
While flashing firmware if we get any error, disconnect the ESP8266 connected USB, reconnect it and again click on START icon to begin the flashing.
If we didn’t get any error, the module will be updated with new firmware and “FINISH” message will be displayed on tool.
After completion of firmware flashing, close the tool, open a web browser with a device page, go to a tab “HELP” and check for new firmware.
![](https://github.com/Lightwell-bg/LWRelay/blob/master/images/help.png?raw=true)
