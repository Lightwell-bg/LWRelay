#ifndef GLOBAL_H
#define GLOBAL_H

const String nName = "LWRelay"; //"LWClock_"; 
const String nVersion = "_v1.2";
#define USE_RTC true //USE RTC chip DS3231
#define USE_BME280 false //USE sensor BME280 (5V) http://got.by/40d52x
#define USE_DHT false //USE sensor DHT
#define USE_DS18B20 false //USE sensor DS18B20
#define USE_CPORTAL false //Captive_Portal
#define PIN_RELLAY D6
#define PIN_LED1 D5
#define PIN_BUTTON D0 //D3


#include <ESP8266WiFi.h>  
//#include <ESP8266mDNS.h> //mDNS   
#include <ESP8266WebServer.h> 
#include <ESP8266NetBIOS.h>
#include <ESP8266HTTPClient.h>  
#include <ESP8266SSDP.h> 
#include <Updater.h>//for update firmware and SPIFFS
//#include <WiFiUdp.h>
#if USE_CPORTAL == true
  #include <DNSServer.h> // Captive_Portal
#endif
//#include <FS.h> 
#include <LittleFS.h>
#include <ArduinoJson.h>        //https://github.com/bblanchon/ArduinoJson.git 
#include <TimeLib.h>            //https://github.com/PaulStoffregen/Time Michael Margolis
#include <Timezone.h>           //https://github.com/JChristensen/Timezone
#include "OneButton.h"
#include "StreamString.h" //need FOR OUTPUT ERROR when update firmware
#include <PubSubClient.h> //"https://github.com/knolleary/pubsubclient.git"
#include "NetCrtESP.h"
#include "ESPTimeFunc.h"
//#include "CRC816.h"
#if USE_RTC == true
  #include "RTClib.h" //https://github.com/adafruit/RTClib
  RTC_DS3231 rtc;
#endif
#if USE_BME280 == true // I2C D1, D2
  #include "BlueDot_BME280.h"                         //https://github.com/BlueDot-Arduino/BlueDot_BME280
  BlueDot_BME280 bme280 = BlueDot_BME280();  
#endif //BME280
#if USE_DHT == true
  #include <Adafruit_Sensor.h>                           // https://github.com/adafruit/Adafruit_Sensor
  #include "DHT.h"                                      //https://github.com/adafruit/DHT-sensor-library
  //#define DHTTYPE DHT21   // DHT 21 (AM2301)
  #define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
  #define TEMPHUM_PIN 13    // D7 выберите входной контакт,к которому подключен датчик
  DHT dht(TEMPHUM_PIN, DHTTYPE);
#endif //DHT
#if USE_DS18B20 == true 
  #include <OneWire.h>
  #include <DallasTemperature.h>
  #define TEMPHUM_PIN 13    // D7 
  OneWire oneWire(TEMPHUM_PIN); //DS18b20
  DallasTemperature sensorTemp(&oneWire);  
#endif 

//For dispalay name of device
#if USE_RTC == true
  const String urts = " +RTC";
#else  
  const String urts = " noRTC";
#endif
#if USE_DHT == true
  String cVersion = nName + nVersion + urts + " DHT22";
#elif USE_BME280 == true 
  String cVersion = nName + nVersion + urts + " BME280";
#elif USE_DS18B20 == true 
  String cVersion = nName + nVersion + urts + " DS18B20";
#else 
  String cVersion = nName + nVersion + urts + " no sensor";  
#endif //BME280 

ESP8266WebServer HTTP; 
#if USE_CPORTAL == true
  const uint8_t DNS_PORT = 53;
  DNSServer dnsServer;
#endif
File fsUploadFile; 
NetCrtESP myWIFI;
ESPTimeFunc myESPTime(USE_RTC);
//WiFiUDP Udp;
WiFiClient ESPclient;
PubSubClient mqttClient(ESPclient);
OneButton buttonON(PIN_BUTTON, true);

String filePath = "/myconfig.json"; //File for config
String jsonConfig = "{}";
// wifi
String ssid = "***";
String password = "***";
String ssidAP = "LWRELAY03";   // SSID AP точки доступа
String passwordAP = ""; // пароль точки доступа
String SSDP_Name = "LWRELAY03"; // Имя SSDP
String apIP = "192.168.4.1";
//UDP
uint16_t localUdpPort = 4210;
//char incomingPacket[255];
//char replyPacket[] = "Hi there! Got the message :-)";

uint8_t lang = 2; //0-RU, 1-BG, 2 -EN, 
//Time
bool useRTC = USE_RTC; //get time RTC
int8_t timezone = 2;               // часовой пояс GTM
uint8_t typeSync = 0; //type of synchronuzation 0 - manual, 1 - NTP, 2 - GPS, 
bool isDayLightSaving = true; //Summer time use
String sNtpServerName = "us.pool.ntp.org";
const char sNtpServerName2[] = "pool.ntp.org";
const char sNtpServerName3[] = "time.nist.gov";
TimeChangeRule localCEST = {"CEST", Last, Sun, Mar, 2, 180};     // LOCAL Summer Time
TimeChangeRule localCET = {"CET ", Last, Sun, Oct, 3, 120};       // LOCAL Standard Time
Timezone localCE(localCEST, localCET);
bool updateRTCfromNTP = false;
//Count of work hours
uint8_t addrCount = 100;
uint32_t totalWorkMinute = 0; //total work time
uint32_t curWorkMs = 0; //current work time
const uint32_t PERIOD_SAVE_WTIME = 1000*60*60;
uint32_t resourceMinute = 540000; //work resource
bool writeEPPROM = false;

//***************
const bool lowLewelActiv = false; //**** if relay is controled LOW level
const uint32_t PERIOD_CHECK_SCHED = 1000*2;
bool statusRelayON = false, checkSched = true; //for schedule and constant ON
//bool RelayIsON = true; //realy state
const uint32_t PERIOD_CHECK_BTN = 1000*3;
bool auto_off = true;//auto off after period
uint32_t timeSchedOFF = 0, perDelayDirON = 1000*60*15;

//Schedule
const uint8_t day_byte[] = {0b00000001, 0b00000010, 0b00000100, 0b00001000, 0b00010000, 0b00100000, 0b01000000}; //1 - Sunday, 2 - Monday .... etc.
typedef struct {
  bool onWork;
  float workFrom;   
  float workTo;     
  uint8_t sched_stat;     
} sScheduleSet;
sScheduleSet mySchedule[] = {{1,23.30,6.30,127},{0,10.30,11,126},{0,15,15.30,127},{0,20,20.30,127}};

//sensor
bool hpa = false; //Pressure hPa or mm for MQTT
int8_t dataCorrectTemp = 0; //correction temp
int8_t dataCorrectHum = 0; //correction hum
bool sensorContrOn = false; 
uint8_t sensorContrType = 0; //0 - heart 1 - cool
float sensorTempLevel = 20.2;
float sensorTempGister = 0.5;
//mqtt
bool mqttOn = false;
char mqttData[80]; //array for send to  MQTT
String mqtt_server = "hairdresser.cloudmqtt.com"; // Имя сервера MQTT
int mqtt_port = 18695; // Порт для подключения к серверу MQTT
String mqtt_user = "***"; // Логи от сервер
String mqtt_pass = "***"; // Пароль от сервера 
String mqtt_name = nName; 
unsigned long ReconnectTime = 5000;  //const unsigned long MQTT_CONNECT = 1000*60*3;
String mqtt_pub_temp = mqtt_name + "/temp"; 
String mqtt_pub_hum = mqtt_name + "/hum";
String mqtt_pub_press = mqtt_name + "/press";
String mqtt_sub_relay = mqtt_name + "/relay";
const unsigned long MQTT_SEND_INT = 1000*60*15; //MQTT send interval
bool controlMQTT = false; //control only from MQTT (ON/OFF mqtt_sub_relay + "/mqqtcontr")
//unsigned long lastTimePHT = 0; const unsigned long PER_GET_THP = 1000*60*3; //get sensor data Period 
//thingspeak.com
const unsigned long TSPEAK_SEND_INT = 1000*60*11; //TSPEAK send interval
bool tspeakOn = false;
String tspeak_server = "api.thingspeak.com";
unsigned long tspeak_channal = 963393;
String tspeak_wapi = "***";  

#endif