#if (USE_BME280 == true || USE_DHT == true || USE_DS18B20 == true) 

void init_sensor() {
#if USE_DHT == true
  dht.begin(); 
#endif //DHT
#if USE_BME280 == true // I2C D1, D2
  bme280.parameter.communication = 0;                  //Choose communication protocol
  bme280.parameter.I2CAddress = 0x76;                  //Choose I2C Address 0x77
  //0b00:     In sleep mode no measurements are performed, but power consumption is at a minimum
  //0b01:     In forced mode a single measured is performed and the device returns automatically to sleep mode
  //0b11:     In normal mode the sensor measures continually (default value)
  bme280.parameter.sensorMode = 0b11; 
  bme280.parameter.IIRfilter = 0b100;                    //Setup for IIR Filter
  bme280.parameter.humidOversampling = 0b101;            //Setup Humidity Oversampling
  bme280.parameter.tempOversampling = 0b101;             //Setup Temperature Ovesampling
  bme280.parameter.pressOversampling = 0b101;            //Setup Pressure Oversampling 
  bme280.parameter.pressureSeaLevel = 1013.25;           //default value of 1013.25 hPa
  bme280.parameter.tempOutsideCelsius = 15;              //default value of 15°C
  uint8_t tries = 10;
  while (--tries && bme280.init() != 0x60) {
    Serial.println(F("Ops! BME280 could not be found! Please check your connections."));
    delay(500);
  }
  if (tries > 0) {
    Serial.println(F("BME280 detected!"));
  }
#endif //BME280 
#if USE_DS18B20 == true
  sensorTemp.begin();
#endif 
}

#if USE_BME280 == true // I2C D1, D2
String getTempBME280(int16_t dataCor, uint8_t pr) { //correct depending brightness and sign after point
  float tempBME = bme280.readTempC() + dataCor;
  Serial.print("Temperature: "); Serial.print(tempBME); Serial.println("°C"); 
  return String(tempBME, pr);
}

String getHumBME280(int16_t dataCor, uint8_t pr) { //correct depending brightness and sign after point
  float humBME = bme280.readHumidity() + dataCor;
  Serial.print("Humidity: "); Serial.print(humBME); Serial.println("%");
  return String(humBME, pr);  
}

String getPressBME280(bool hpa, uint8_t pr) { //unit of press and sign after point
  float pressBME = bme280.readPressure();
  Serial.print(F("Pressure "));Serial.print(pressBME);Serial.println(" hPa");
  Serial.print(F("Pressure "));Serial.print(pressBME*0.750062);Serial.println(" mm");
  Serial.print(F("Altitude in Meters:\t\t")); Serial.println(bme280.readAltitudeMeter());
  if (hpa) return String(pressBME, pr);
  else     return String(pressBME*0.750062, pr);  
}
#endif //BME280 

#if USE_DHT == true
String getTempDHT(int16_t dataCor = 0, uint8_t pr = 1) {
  float h = dht.readHumidity(); 
  float t = dht.readTemperature(); 
  if (isnan(t) || isnan(h))  {
    Serial.println("Failed to read Temp from DHT");
    return "";
  }
  else {
    // Compute heat index in Celsius (isFahreheit = false)
    float hic = dht.computeHeatIndex(t, h, false) + dataCor;
    String tempDHT = String(hic, pr); 
    Serial.print("Temperature: "); Serial.print(tempDHT); Serial.println("°C");     
    return tempDHT;      
  }
}

String getHumDHT(int16_t dataCor = 0, uint8_t pr = 0) {
  float h = dht.readHumidity() + dataCor; 
  if (isnan(h))  {
    Serial.println("Failed to read Hum from DHT");
    return "";
  }
  else {
    // Compute heat index in Celsius (isFahreheit = false)
    String humDHT = String(h, pr); 
    Serial.print("Humidity: "); Serial.print(humDHT); Serial.println("%");     
    return humDHT;      
  }
}
#endif //DHT
#if USE_DS18B20 == true
String getTempDS(int8_t dataCor = 0, uint8_t pr = 1) {
  sensorTemp.requestTemperatures();
  float tempDS = sensorTemp.getTempCByIndex(0) + dataCor;
  Serial.print("Temperature: "); Serial.print(tempDS); Serial.println("°C");
  return String(tempDS, pr); 
}
#endif //USE_DS18B20

bool checkTemperature(uint8_t typeControl, int8_t dataCor = 0, uint32_t Per = 0) {
  static uint32_t ms = 0;
  static bool devOn = false;
  float tempCur;
  if (millis() - ms < Per) return devOn;
#if USE_DS18B20 == true
  sensorTemp.requestTemperatures();
  tempCur = sensorTemp.getTempCByIndex(0) + dataCor;
#endif //USE_DS18B20
#if USE_BME280 == true
  tempCur = bme280.readTempC() + dataCor;
#endif 
#if USE_DHT == true
  tempCur = dht.computeHeatIndex(t, h, false) + dataCor;
#endif 
  if(typeControl == 0) { //heart
    if (tempCur > (sensorTempLevel + (sensorTempGister/2)))  devOn = false;
    if (tempCur < (sensorTempLevel - (sensorTempGister/2)))  devOn = true;
  }
  else {
    if (tempCur > (sensorTempLevel + (sensorTempGister/2)))  devOn = true;
    if (tempCur < (sensorTempLevel - (sensorTempGister/2)))  devOn = false;
  }
  ms = millis();
  return devOn;
}

#endif

void myClickFunction() {
  if (!statusRelayON && checkSched) { //01
    statusRelayON = true;
    checkSched = false;
  }
  else if(statusRelayON && !checkSched) { //10
    //statusRelayON = true;
    checkSched = true;
  }
  else if (statusRelayON && checkSched) { //11 = 01
    statusRelayON = true;
    checkSched = false;
  }
  else if (!statusRelayON && !checkSched) { //00 NOT REALY
    statusRelayON = true;
    checkSched = true;
  }
  Serial.print(F("Buttom ")); Serial.println(statusRelayON? "ON" : "OFF");
  if (!checkSched) timeSchedOFF = millis();
  digitalWrite(PIN_LED1, statusRelayON);//for indicate if oldStatusRelay == statusRelayON
} // myClickFunction

void myPressFunction() {
  ESP.restart();
}

void ledBlank(uint32_t Per) {
  static uint32_t ms = 0;
  static bool ledStat = 1;
  if (millis() - ms < Per) return;
  digitalWrite(PIN_LED1, ledStat);
  ledStat = !ledStat;
  ms = millis();
}