#include <Arduino.h>
#include "config.h" 
#include "configFile.h"
#include "fwareupd.h"
#include "epromcount.h"
#include "sensor.h"
#include "httpServer.h"
#include "schedule.h"
//#include "udpserv.h"
#include "mqtt.h"

void setup() {
  Serial.begin(115200);
  pinMode(PIN_RELLAY, OUTPUT);
  pinMode(PIN_LED1, OUTPUT);
  buttonON.setDebounceTicks(50);
  buttonON.attachClick(myClickFunction);
  buttonON.setPressTicks(3000);
  buttonON.attachPress(myPressFunction);
  if (lowLewelActiv) digitalWrite(PIN_RELLAY, !statusRelayON); //on relay
    else digitalWrite(PIN_RELLAY, statusRelayON);
  digitalWrite(PIN_LED1, statusRelayON);
  char host[16];
  if(!LittleFS.begin()) {
  //if(!SPIFFS.begin()) { // Initialize SPIFFS
        Serial.println("An Error has occurred while mounting LittleFS");
        return;
  } 
  loadConfig(); Serial.println(F("FileConfig init"));
  myWIFI.begin(ssid.c_str(), password.c_str(), SSDP_Name.c_str(), ssidAP.c_str(), passwordAP.c_str(), apIP.c_str());
  Serial.println(F("Wi-Fi init"));
  if (typeSync == 0) useRTC = true; else useRTC = false;
  myESPTime.begin(timezone, isDayLightSaving, sNtpServerName, sNtpServerName2, sNtpServerName3, useRTC, updateRTCfromNTP);
  Serial.println(F("Start Time"));
  init_firmWareUpdate(); Serial.println(F("Start init FirmWare update"));
  SSDP_Name = myWIFI.getNameSSDP(); Serial.println(SSDP_Name);
  if (WiFi.status() == WL_CONNECTED)  {
      init_SSDP(); Serial.println(F("Start SSDP"));   //Run SSDP
  }
  else {
#if USE_CPORTAL == true
    dnsServer.setErrorReplyCode(DNSReplyCode::NoError); // Captive_Portal
    dnsServer.start(DNS_PORT, F("*"), WiFi.softAPIP()); // Captive_Portal
#endif    
  }
  init_HTTPServer(); Serial.println(F("HTTPServer init"));
  SSDP_Name.toCharArray(host, SSDP_Name.length()+1);
  NBNS.begin(SSDP_Name.c_str());
  Serial.printf("Ready! Open http://%s in your browser\n", host);
  Serial.println(myWIFI.getDevStatusIP()); 
  //init_udp();
  myWIFI.beginUDP(localUdpPort);
  init_mqtt();
  curWorkMs = millis();
  if(writeEPPROM) totalWorkMinute = loadCountEEPROM(addrCount, 512); //long	int32_t	4 байта
    else totalWorkMinute = 0;
#if (USE_BME280 == true || USE_DHT == true)
  init_sensor(); Serial.println(F("Start Sensor"));
#endif   
}

void loop() {
  static bool oldStatusRelay = false;
  static uint32_t tCount = 0, tCountBtn = 0;
#if USE_CPORTAL == true
    dnsServer.processNextRequest(); // Captive_Portal
#endif    
  HTTP.handleClient();
  //listenUDP();
  myWIFI.returnIPtoUDP();
  buttonON.tick();
  if(mqttOn) {
      mqttReconnect(ReconnectTime);
      mqttClient.loop();
#if (USE_BME280 == true || USE_DHT == true || USE_DS18B20 == true) 
      sendMQTT(MQTT_SEND_INT);
#endif //DHT
  }
#if (USE_BME280 == true || USE_DHT == true || USE_DS18B20 == true) 
  if (tspeakOn) sendTspeak(TSPEAK_SEND_INT);
#endif //DHT
  if(!controlMQTT) {
    if ((millis() - tCount > PERIOD_CHECK_SCHED) && checkSched) {
#if (USE_BME280 == true || USE_DHT == true || USE_DS18B20 == true)
      if(sensorContrOn) {
        if(checkSchedule() && checkTemperature(sensorContrType, dataCorrectTemp)) {
          statusRelayON = true;  
        }
        else statusRelayON = false; 
      }
      else statusRelayON = checkSchedule();
#else
      statusRelayON = checkSchedule();
#endif //Sensor
      tCount = millis();
    }
    if(!checkSched) {
      ledBlank(700);
#if (USE_BME280 == true || USE_DHT == true || USE_DS18B20 == true)
      if(sensorContrOn) {
        statusRelayON = checkTemperature(sensorContrType, dataCorrectTemp, 1000);
      }
#endif //Sensor
      if ((millis() - timeSchedOFF > perDelayDirON) && auto_off) {
        checkSched = true;
      }
    }
  } //if(!controlMQTT)
  if (oldStatusRelay != statusRelayON) {
    Serial.println(statusRelayON?"Lamp is ON":"Lamp is OFF");
    if (lowLewelActiv) digitalWrite(PIN_RELLAY, !statusRelayON); 
        else digitalWrite(PIN_RELLAY, statusRelayON);
    if (statusRelayON) {
      curWorkMs = millis();
    }
    else {
      saveWorkTime();
    }
    digitalWrite(PIN_LED1, statusRelayON);
    if(mqttOn) {
      sendMQTTRelay(statusRelayON);
      sendMQTTRelayMQTTControl(controlMQTT);
    }
    oldStatusRelay = statusRelayON;
  }
  if (statusRelayON) saveWorkTimePer(PERIOD_SAVE_WTIME);
  if(controlMQTT && !statusRelayON) ledBlank(2000);
  yield();
}