void callback(char* topic, byte* payload, unsigned int length);
void sendMQTTRelay(bool relayStatus);
void sendMQTTRelayMQTTControl(bool cntrMQTT);
//void saveRestSched(bool command);

void init_mqtt() {
  mqttClient.setServer(mqtt_server.c_str(), mqtt_port);
  mqttClient.setCallback(callback);  
}

void callback(char* topic, byte* payload, unsigned int length) { 
  Serial.print(F("Message arrived "));
  String mqtt_sub_relay_mqttcontr = mqtt_sub_relay + "/mqqtcontr";
  payload[length] = '\0';
  String strTopic = String(topic);
  String strPayload = String((char*)payload);
  Serial.print(strTopic);  Serial.print(" => "); Serial.println(strPayload);
  if(strTopic == mqtt_sub_relay) {
    Serial.println(strPayload);
    if ((char)payload[0] == '1') {
        statusRelayON = 1;
        controlMQTT = true;
        //checkSched = 0;
        //timeSchedOFF = millis();
    }
    else {
        statusRelayON = 0;
        controlMQTT = true;
        //checkSched = 0;    
    }
    sendMQTTRelay(statusRelayON);
    sendMQTTRelayMQTTControl(controlMQTT);
  }  
  if(strTopic == mqtt_sub_relay_mqttcontr) {
      if ((char)payload[0] == '1') {
        controlMQTT = true;
        //checkSched = 0;
      }
      else {
        controlMQTT = false;
        //checkSched = 1; 
      }
      sendMQTTRelayMQTTControl(controlMQTT);
      saveConfig();
  }    
}

boolean mqttReconnect(uint16_t period) {
    static unsigned int lastReconnectAttempt = 0;
    if (!mqttClient.connected()) {
        if (millis() - lastReconnectAttempt > period) {
            //Serial.print("Attempting MQTT connection...");
            if (mqttClient.connect(mqtt_name.c_str(), mqtt_user.c_str(), mqtt_pass.c_str())) {
                Serial.println("connected....");
                // Once connected, publish an announcement...
                mqttClient.publish("HelloTopic", "Hello");//*************************************************************
                // ... and resubscribe
                mqttClient.subscribe(mqtt_sub_relay.c_str());
                String mqtt_sub_relay_mqttcontr = mqtt_sub_relay + "/mqqtcontr";
                mqttClient.subscribe(mqtt_sub_relay_mqttcontr.c_str());
                lastReconnectAttempt = millis();
                return true;
            }
            else {
                Serial.print("failed, rc=");
                Serial.print(mqttClient.state());
                Serial.println(" try again in " + String(ReconnectTime/1000) + " seconds");
                lastReconnectAttempt = millis();
                return false;
            }
        }
    }
    else return true;
}

/*void saveRestSched(bool command) {
  if (!command) {
    if (mySchedule[0].onWork) savedStatSched |= 0b0001; else savedStatSched &= 0b1110;
    if (mySchedule[1].onWork) savedStatSched |= 0b0010; else savedStatSched &= 0b1101;
    if (mySchedule[2].onWork) savedStatSched |= 0b0100; else savedStatSched &= 0b1011;
    if (mySchedule[3].onWork) savedStatSched |= 0b1000; else savedStatSched &= 0b0111;
    for(uint8_t i=0; i<4; i++) {mySchedule[i].onWork = false;}
    if(savedStatSched != 0) saveConfig();
    sendMQTTRelaySched(false);
  }
  else {
    if((savedStatSched & 0b0001) > 0) mySchedule[0].onWork = true;
    if((savedStatSched & 0b0010) > 0) mySchedule[1].onWork = true;
    if((savedStatSched & 0b0100) > 0) mySchedule[2].onWork = true;
    if((savedStatSched & 0b1000) > 0) mySchedule[3].onWork = true;
    //savedStatSched = 0;
    digitalWrite(PIN_LED1, statusRelayON);//for indicate if oldStatusRelay == statusRelayON
    sendMQTTRelaySched(true);
  }  
}*/

void sendMQTTRelay(bool relayStatus) {
  String mqtt_pub_relay = mqtt_sub_relay + "/stat";
  String relayStatusSend = "";
  if (relayStatus) relayStatusSend = "1";
    else relayStatusSend = "0";
  relayStatusSend.toCharArray(mqttData, (relayStatusSend.length()+1));
  mqttClient.publish(mqtt_pub_relay.c_str(), mqttData, true);
  Serial.print("Send MQTT relay status ");  Serial.println((relayStatus ? "ON" : "OFF"));
}

void sendMQTTRelayMQTTControl(bool cntrMQTT) {
  String mqtt_sub_relay_mqttcontr_stat = mqtt_sub_relay + "/mqqtcontr/stat";
  String statusMQTTControl = "";
  if (cntrMQTT) statusMQTTControl = "1";
    else statusMQTTControl = "0";
  statusMQTTControl.toCharArray(mqttData, (statusMQTTControl.length()+1));
  mqttClient.publish(mqtt_sub_relay_mqttcontr_stat.c_str(), mqttData, true);
  Serial.print("Send MQTT statusMQTTControl ");  Serial.println((cntrMQTT ? "ON" : "OFF"));
}

#if (USE_BME280 == true || USE_DHT == true || USE_DS18B20 == true) 
  void sendMQTT(uint32_t Per) {
    static uint32_t ms = 0;
    if (millis() - ms < Per) return;
#if USE_DHT == true
    String tempDHT = getTempDHT(dataCorrectTemp, 2); 
    String humDHT = getHumDHT(dataCorrectHum, 1); 
    tempDHT.toCharArray(mqttData, (tempDHT.length()+1));
    mqttClient.publish(mqtt_pub_temp.c_str(), mqttData, true); //  
    humDHT.toCharArray(mqttData, (humDHT.length()+1));
    mqttClient.publish(mqtt_pub_hum.c_str(), mqttData, true); // 
#endif //DHT
#if USE_BME280 == true
    String tempBME = getTempBME280(dataCorrectTemp, 2); 
    String humBME = getHumBME280(dataCorrectHum, 2);
    String pressBME = getPressBME280(hpa, 2);
    Serial.print("Temperature: "); Serial.print(tempBME); Serial.print("°C"); Serial.print(". Humidity: "); Serial.print(humBME); Serial.print("%");
    Serial.print(". Pressure: ");Serial.print(pressBME); Serial.println((hpa ? " hPa" : " mm"));
    tempBME.toCharArray(mqttData, (tempBME.length()+1));
    mqttClient.publish(mqtt_pub_temp.c_str(), mqttData, true); //  
    humBME.toCharArray(mqttData, (humBME.length()+1));
    mqttClient.publish(mqtt_pub_hum.c_str(), mqttData, true);    
    pressBME.toCharArray(mqttData, (pressBME.length()+1));
    mqttClient.publish(mqtt_pub_press.c_str(), mqttData, true);        
#endif //BME280 
#if USE_DS18B20 == true
    String tempDS = getTempDS(dataCorrectTemp, 2); 
    tempDS.toCharArray(mqttData, (tempDS.length()+1));
    mqttClient.publish(mqtt_pub_temp.c_str(), mqttData, true); //  
#endif
    ms = millis();
  }
#endif

#if (USE_BME280 == true || USE_DHT == true || USE_DS18B20 == true) 
String getResponse();
void HTTPPost(uint8_t numFields, String fieldData[]){
  if (ESPclient.connect(tspeak_server, 80)) {
     // If you have multiple fields, make sure the sting does not exceed 1440 characters.
     //String postData= "api_key=" + writeAPIKey + "&field" + String(numField) + "=" + fieldData;
     String postData= "api_key=" + tspeak_wapi;
     for (uint8_t fieldNumber = 0; fieldNumber < numFields; fieldNumber++) {
          String fieldName = "field" + String(fieldNumber+1);
          postData += "&" + fieldName + "=" + fieldData[fieldNumber];          
      }       
      // POST data via HTTP
      Serial.println("Connecting to ThingSpeak for update...");
      Serial.println();
      ESPclient.println("POST /update HTTP/1.1");
      ESPclient.println("Host: " + String(tspeak_server));
      ESPclient.println("Connection: close");
      ESPclient.println("Content-Type: application/x-www-form-urlencoded");
      ESPclient.println("Content-Length: " + String(postData.length()));
      ESPclient.println();
      ESPclient.println(postData);
      Serial.println(postData);
      String answer = getResponse();
      Serial.println(answer);
  }
  else {
    Serial.println("Connection Failed");
  }
}

String getResponse() {
  String response = "";
  unsigned startTime = millis();
  delay (200);
  while (!ESPclient.available() && ((millis()-startTime) < 5000)) {
        delay (5);
  }
  
  while (ESPclient.available()) { // Get response from server
     char charIn = ESPclient.read(); // Read a char from the buffer.
     response += charIn;     // Append the char to the string response.
  }
  ESPclient.stop();
  return response;
}


void sendTspeak(uint32_t Per) {
    static uint32_t ms = 0;
    if (millis() - ms < Per) return;
#if USE_DHT == true
    String fieldData[2];  
    fieldData[0] = getTempDHT(dataCorrectTemp, 2); 
    fieldData[1] = getHumDHT(dataCorrectHum, 1);
    Serial.print("Temperature: "); Serial.print(fieldData[0]); Serial.print("°C"); Serial.print(". Humidity: "); Serial.print(fieldData[1]); Serial.println("%");     
    HTTPPost(2, fieldData);
#endif //DHT
#if USE_BME280 == true
    String fieldDataBME[3];  
    fieldDataBME[0] = getTempBME280(dataCorrectTemp, 2);  
    fieldDataBME[1] = getHumBME280(dataCorrectHum, 2);
    fieldDataBME[2] = getPressBME280(hpa, 2);
    Serial.print("Temperature: "); Serial.print(fieldDataBME[0]); Serial.print("°C"); Serial.print(". Humidity: "); Serial.print(fieldDataBME[1]); Serial.print("%");
    Serial.print(". Pressure: ");Serial.print(fieldDataBME[2]); Serial.println("hPa");
    HTTPPost(3, fieldDataBME);
#endif //BME280
#if USE_DS18B20 == true
    String fieldData[1];  
    fieldData[0] = getTempDS(dataCorrectTemp, 2); 
    Serial.print("Temperature: "); Serial.print(fieldData[0]); Serial.print("°C");    
    HTTPPost(1, fieldData);
#endif
    ms = millis();
}

#endif