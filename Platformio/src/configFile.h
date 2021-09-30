bool saveConfig();

bool loadConfig() {
    //if(!SPIFFS.exists(filePath)) {
    if(!LittleFS.exists(filePath)) {
        Serial.println(F("Failed to open config file"));
        saveConfig();   
        return false;
    }
    //File configFile = SPIFFS.open(filePath, "r"); 
    File configFile = LittleFS.open(filePath, "r");  
    size_t size = configFile.size();   
    if (size > 2048) {  
        Serial.print(F("Config file size is too large: ")); Serial.println(size);
        configFile.close();
        return false;
    }
    jsonConfig = configFile.readString(); 
    configFile.close();
    DynamicJsonDocument jsonDoc(5096); //4096
    DeserializationError error = deserializeJson(jsonDoc, jsonConfig);
    if (error) {
      Serial.print(F("loadConfig() deserializeJson() failed with code "));
      Serial.println(error.c_str());
      return false;
    }   
    JsonObject root = jsonDoc.as<JsonObject>();  
    timezone = root["timezone"].as<int>();               
    isDayLightSaving = root["isDayLightSaving"];
    sNtpServerName = root["ntp_server"].as<String>(); 
    typeSync = root["typeSync"];   
    lang = root["lang"].as<int>();
    resourceMinute = root["resourceMinute"].as<uint32_t>();
    writeEPPROM = root["writeEPPROM"];
    auto_off = root["auto_off"];
    perDelayDirON = root["perDelayDirON"];
    mySchedule[0].onWork = root["onWork1"]; mySchedule[1].onWork = root["onWork2"];
    mySchedule[0].workFrom = root["workFrom1"]; mySchedule[0].workTo = root["workTo1"];
    mySchedule[1].workFrom = root["workFrom2"]; mySchedule[1].workTo = root["workTo2"];
    mySchedule[0].sched_stat = root["sched1_stat"]; mySchedule[1].sched_stat = root["sched2_stat"];
    mySchedule[2].onWork = root["onWork3"];
    mySchedule[2].workFrom = root["workFrom3"]; mySchedule[2].workTo = root["workTo3"];
    mySchedule[2].sched_stat = root["sched3_stat"];
    mySchedule[3].onWork = root["onWork4"];
    mySchedule[3].workFrom = root["workFrom4"]; mySchedule[3].workTo = root["workTo4"];
    mySchedule[3].sched_stat = root["sched4_stat"];
    dataCorrectTemp = root["dataCorrectTemp"].as<int>();
    hpa = root["hpa"];
    sensorContrOn = root["sensorContrOn"];
    sensorContrType = root["sensorContrType"];
    sensorTempLevel = root["sensorTempLevel"].as<float>();
    sensorTempGister = root["sensorTempGister"].as<float>();
    mqttOn = root["mqttOn"];
    mqtt_server = root["mqtt_server"].as<String>();
    mqtt_port = root["mqtt_port"].as<int>();
    mqtt_user = root["mqtt_user"].as<String>();
    mqtt_pass = root["mqtt_pass"].as<String>();
    mqtt_name = root["mqtt_name"].as<String>();
    mqtt_pub_temp = root["mqtt_pub_temp"].as<String>();
    mqtt_pub_hum = root["mqtt_pub_hum"].as<String>();
    mqtt_pub_press = root["mqtt_pub_press"].as<String>();
    mqtt_sub_relay = root["mqtt_sub_relay"].as<String>();
    controlMQTT = root["controlMQTT"];
    tspeakOn = root["tspeakOn"];
    tspeak_server = root["tspeak_server"].as<String>();
    tspeak_channal = root["tspeak_channal"].as<int>();
    tspeak_wapi = root["tspeak_wapi"].as<String>(); 
    return true;
}

bool saveConfig() {
    DynamicJsonDocument jsonDoc(5096); //4096
    DeserializationError error = deserializeJson(jsonDoc, jsonConfig);
    if (error) {
        Serial.print(F("saveConfig() deserializeJson() failed with code "));
        Serial.println(error.c_str());
        return false;
    }     
    JsonObject json = jsonDoc.as<JsonObject>();  
    json["timezone"] = timezone;
    json["isDayLightSaving"] = isDayLightSaving;
    json["ntp_server"] = sNtpServerName;
    json["typeSync"] = typeSync;
    json["lang"] = lang;
    json["cVersion"] = cVersion;
    json["resourceMinute"] = resourceMinute;
    json["writeEPPROM"] = writeEPPROM;
    json["auto_off"] = auto_off;
    json["perDelayDirON"] = perDelayDirON;
    json["onWork1"] = mySchedule[0].onWork; json["onWork2"] = mySchedule[1].onWork;
    json["workFrom1"] = mySchedule[0].workFrom; json["workTo1"] = mySchedule[0].workTo; 
    json["workFrom2"] = mySchedule[1].workFrom; json["workTo2"] = mySchedule[1].workTo;
    json["sched1_stat"] = mySchedule[0].sched_stat; json["sched2_stat"] = mySchedule[1].sched_stat;
    json["onWork3"] = mySchedule[2].onWork;
    json["workFrom3"] = mySchedule[2].workFrom; json["workTo3"] = mySchedule[2].workTo;
    json["sched3_stat"] = mySchedule[2].sched_stat;
    json["onWork4"] = mySchedule[3].onWork;
    json["workFrom4"] = mySchedule[3].workFrom; json["workTo4"] = mySchedule[3].workTo;
    json["sched4_stat"] = mySchedule[3].sched_stat;
    json["dataCorrectTemp"] = dataCorrectTemp;
    json["hpa"] = hpa;
    json["sensorContrOn"] = sensorContrOn;
    json["sensorContrType"] = sensorContrType;
    json["sensorTempLevel"] = sensorTempLevel;
    json["sensorTempGister"] = sensorTempGister;
    json["mqttOn"] = mqttOn;
    json["mqtt_server"] = mqtt_server;
    json["mqtt_port"] = mqtt_port;
    json["mqtt_user"] = mqtt_user;
    json["mqtt_pass"] = mqtt_pass;
    json["mqtt_name"] = mqtt_name;
    json["mqtt_pub_temp"] = mqtt_pub_temp;
    json["mqtt_pub_hum"] = mqtt_pub_hum;
    json["mqtt_pub_press"] = mqtt_pub_press;
    json["mqtt_sub_relay"] = mqtt_sub_relay;
    json["controlMQTT"] = controlMQTT; 
    json["tspeakOn"] = tspeakOn;
    json["tspeak_server"] = tspeak_server;
    json["tspeak_channal"] = tspeak_channal;
    json["tspeak_wapi"] = tspeak_wapi; 
    serializeJson(json, jsonConfig);
    //File configFile = SPIFFS.open(filePath, "w");
    File configFile = LittleFS.open(filePath, "w");
    if (!configFile) {
        Serial.println(F("Failed to open config file for writing"));
        configFile.close();
        return false;
    }
    serializeJson(json, configFile);
    configFile.close();
    return true;
}