# Controller for heating / cooling devices up to 3.5 kW KIT
**Based on ESP8266 for Home Automation (IoT).**
Built-in Wi-Fi allows you to control devices connected to the controller via any web browser or using an application for a mobile phone / tablet.
Connecting the controller to the Internet will allow you to control the heater from anywhere outside the home via MQTT.
Special application for android devices [https://play.google.com/store/apps/details?id=eu.ledlightwell.lwheater](https://play.google.com/store/apps/details?id=eu.ledlightwell.lwheater)

**Features**
- automatic turning on / of the device at night (reduced rates)
- turning on / off the device according to a specified schedule (4 time intervals)
- maintaining the set temperature (heating or cooling)
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

### Required Parts:
|  ID |  Designator |  Name | Quantity  |  Link |
| ------------ | ------------ | ------------ | ------------ | ------------ |
|1|U1|DS3231|1|http://got.by/5e99zr|
|2|U5|HLK-PM01|1|http://got.by/5e99tc|
|3|R7,R8,R6|10k|3||
|4|R9|100|1||
|5|R1|200|1||
|6|R2,R3|4.7k|2||
|7|R5|220|1||
|8|Q2|IRL530NPBF|1|http://got.by/5e9afi|
|9|RELAY1|HF115F/005-2ZS4|1|http://got.by/5e9aag|
|10|H14|Header-Male-2.54_1x3|1|Header2.54mm 1x3P|
|11|LED1|LED-TH-5mm_R|1|333-2SURD/S530-A3|
|12|J3,J2|HDR-M-2.54_1x2|2||
|13|C3|10uF|1|50YXF10MFFC5X11|
|14|D2,D1|1N4148|2|http://got.by/5e9anv|
|15|H7|Header-Male-2.54_1x2|1|826629-2|
|16|U6|DG128-5.0-03P-14-00A(H)|1|http://got.by/5e9b13|
|17|U2|Wemos D1 Mini|1|http://got.by/40d3u7|
|18|B1|BS-2-1|1|BS-2-1|
|19|C1|100nF|1||
|20|U7,U3,U4|DG128-5.0-02P-14-00A(H)|3|http://got.by/5e9b13|
|21|BME280|||http://ali.pub/40d52x|
|22|DHT22|||http://got.by/5e9b6x|

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
