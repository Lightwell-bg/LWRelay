void init_SSDP(void);
String getContentType(String filename);
bool handleFileRead(String path);
void handle_ConfigJSON();
void handle_Set_Ssid();
void handle_Restart();
void handle_resetConfig();
void handle_time_zone();
void handle_ntp_server();
void handle_Time();
void handle_TimeNew();
void handle_Language();
void handle_sensorSet();
void handle_MQTTSet();
void handle_MQTTOn();
void handle_tspeakSet();
void handle_tspeakOn();
void handle_setwcount();
void handle_saveEPROMOn();
void handle_setworktime();
void handle_setautoff();
void handle_setonoff();
void handleNotFound();
void handle_SensorOn();

void init_HTTPServer(void) {
    HTTP.serveStatic("/", LittleFS, "/index.html");
    //HTTP.serveStatic("/generate_204", LittleFS, "/index.html");  //Android captive portal. Maybe not needed. Might be handled by notFound handler.
    //HTTP.serveStatic("/fwlink", LittleFS, "/index.html");  //Microsoft captive portal. Maybe not needed. Might be handled by notFound handler.
    HTTP.on("/configs.json", handle_ConfigJSON); // create configs.json for sending to WEB interface
    HTTP.on("/lang", handle_Language);
    HTTP.on("/ssid", handle_Set_Ssid);     // Set WiFi config and SSDP
    HTTP.on("/Time", handle_Time);     // Sync Time
    HTTP.on("/TimeZone", handle_time_zone);    // Set time zone
    HTTP.on("/setntpserver", handle_ntp_server); // Set sync NTP
    HTTP.on("/TimeNew", handle_TimeNew);     // Set new time 
    HTTP.on("/restart", handle_Restart);   // reset ESP
    HTTP.on("/resetConfig", handle_resetConfig); 
    HTTP.on("/sensorSet", handle_sensorSet);
    HTTP.on("/sensorOn", handle_SensorOn);
    HTTP.on("/mqttSet", handle_MQTTSet);
    HTTP.on("/mqttOn", handle_MQTTOn);
    HTTP.on("/tspeakSet", handle_tspeakSet);
    HTTP.on("/tspeakOn", handle_tspeakOn);
    HTTP.on("/setwcount", handle_setwcount);     // Set count & param 
    HTTP.on("/saveOn", handle_saveEPROMOn);
    HTTP.on("/setworktime", handle_setworktime);     // Set  setworktime
    HTTP.on("/setautoff", handle_setautoff);
    HTTP.on("/setonoff", handle_setonoff);      
#if USE_CPORTAL == true    
    HTTP.onNotFound([]() {handleNotFound();});  // Captive_Portal ********************************  
#else
    HTTP.onNotFound([]() {
        if (!handleFileRead(HTTP.uri())) HTTP.send(404, "text/plain", "FileNotFound");
    });
#endif
    HTTP.begin();  
}

#if USE_CPORTAL == true
// Captive_Portal ********************************
bool captivePortal() {
  if (! HTTP.hostHeader().equals(WiFi.softAPIP().toString())) {
    HTTP.sendHeader(F("Location"), String(F("http://")) + WiFi.softAPIP().toString(), true);
    HTTP.send_P(302, "text/plain", NULL, 0);
    return true;
  }
  return false;
}

void handleNotFound() {
    if (WiFi.status() != WL_CONNECTED) {
        if (captivePortal()) return;
    }
    if (! handleFileRead(HTTP.uri()))
        HTTP.send_P(404, "text/plain", "FileNotFound");
}
#endif

void handle_ConfigJSON() { 
    struct tm tnow = myESPTime.getTimeStruct();
    String root = "{}";  
    DynamicJsonDocument jsonDoc(5096);//4096
    DeserializationError error =  deserializeJson(jsonDoc, root);
    if (error) {
        Serial.print(F("deserializeJson() failed with code "));
        Serial.println(error.c_str());
        return;
    }     
    JsonObject json = jsonDoc.as<JsonObject>(); 
    json["cVersion"] = cVersion;   
    json["SSDP"] = myWIFI.getNameSSDP();
    json["ssidAP"] = myWIFI.getNameAPSSID();
    json["passwordAP"] = myWIFI.getPassAPSSID();
    json["ssid"] = myWIFI.getNameSSID();
    json["password"] = myWIFI.getPassSSID();
    json["ip"] = myWIFI.getDevStatusIP();
    json["time"] = myESPTime.getTimeStr(true);
    json["date"] = myESPTime.getDateStr();
    json["time_h"] = String(tnow.tm_hour);
    json["time_m"] = String(tnow.tm_min);
    json["time_s"] = String(tnow.tm_sec); 
    json["date_day"] = tnow.tm_mday;
    json["date_month"] = tnow.tm_mon + 1;
    json["date_year"] = tnow.tm_year + 1900;
    json["timezone"] = timezone;
    json["isDayLightSaving"] = (isDayLightSaving?"checked":"");
    json["lang"] = (lang==0?"RU":lang==1?"BG":lang==2?"EN":"EN");
    json["ntpserver"] = sNtpServerName; 
    json["use_sync"] = typeSync;
    uint32_t valTotalWorkMinute = totalWorkMinute;
    if (statusRelayON) valTotalWorkMinute += (millis() - curWorkMs) / 60000;
    String strMin = valTotalWorkMinute % 60 < 10 ? "0" + String(valTotalWorkMinute % 60) : String(valTotalWorkMinute % 60);
    json["totalWorkMinute"] = valTotalWorkMinute;
    json["valWorkHour"] = String(valTotalWorkMinute / 60) + ":" + strMin;
    json["resourceMinute"] = resourceMinute;
    json["resourcehours"] = resourceMinute / 60;
    if(controlMQTT) json["lampison"]=(statusRelayON?"<b><font color=\"green\">Relay ON</font></b><BR><b><font color=\"red\">SCHEDULER OFF</font></b>":"<b><font color=\"red\">Relay OFF</font></b><BR><b><font color=\"red\">SCHEDULER OFF</font></b>");
        else json["lampison"]=(statusRelayON?"<b><font color=\"green\">Relay ON</font></b>":"<b><font color=\"red\">Relay OFF</font></b>");
    json["writeEPPROM"] = (writeEPPROM?"checked":"");
    json["auto_off"] = (auto_off?"checked":"");
    json["perDelayDirON"] = perDelayDirON/60000;
    json["onWork1"]=(mySchedule[0].onWork?"checked":""); json["onWork2"]=(mySchedule[1].onWork?"checked":"");
    json["workFrom1"] = mySchedule[0].workFrom; json["workTo1"] = mySchedule[0].workTo; 
    json["workFrom2"] = mySchedule[1].workFrom; json["workTo2"] = mySchedule[1].workTo;
    json["sched1_stat"] = mySchedule[0].sched_stat; json["sched2_stat"] = mySchedule[1].sched_stat;
    json["onWork3"]=(mySchedule[2].onWork?"checked":"");
    json["workFrom3"] = mySchedule[2].workFrom; json["workTo3"] = mySchedule[2].workTo;
    json["sched3_stat"] = mySchedule[2].sched_stat;
    json["onWork4"]=(mySchedule[3].onWork?"checked":"");
    json["workFrom4"] = mySchedule[3].workFrom; json["workTo4"] = mySchedule[3].workTo;
    json["sched4_stat"] = mySchedule[3].sched_stat;
    json["onrelay"]=((statusRelayON && !checkSched)?"checked":"");
#if USE_BME280 == true
    json["temp_now"] = getTempBME280(dataCorrectTemp, 2);
    json["hum_now"] = getHumBME280(dataCorrectHum, 2) + "%  Press " + getPressBME280(hpa, 2) + (hpa ? " hPa" : " mm");
#endif
#if USE_DHT == true
    json["temp_now"] = getTempDHT(dataCorrectTemp, 1);
    json["hum_now"] = getHumDHT(dataCorrectHum, 0)+ "%";
#endif
#if USE_DS18B20 == true
    json["temp_now"] = getTempDS(dataCorrectTemp, 1); 
    json["hum_now"] = "n/a";
#endif
#if (USE_BME280 == false && USE_DHT == false && USE_DS18B20 == false)
    json["temp_now"] = "n/a";
    json["hum_now"] = "n/a %  Press n/a" + String(hpa ? " hPa" : " mm");
#endif
    json["datacorrect"] = dataCorrectTemp;
    json["hpa"]=(hpa?"1":"0");
    json["sensorContrOn"] = (sensorContrOn?"checked":"");
    json["sensorContrType"] = sensorContrType;
    json["sensorTempLevel"] = sensorTempLevel;
    json["sensorTempGister"] = sensorTempGister;
    json["mqttOn"]=(mqttOn?"checked":"");
    json["mqtt_server"] = mqtt_server;
    json["mqtt_port"] = mqtt_port;
    json["mqtt_user"] = mqtt_user;
    json["mqtt_pass"] = mqtt_pass;
    json["mqtt_name"] = mqtt_name;
    json["mqtt_pub_temp"] = mqtt_pub_temp;
    json["mqtt_pub_hum"] = mqtt_pub_hum;
    json["mqtt_pub_press"] = mqtt_pub_press;
    json["mqtt_sub_relay"] = mqtt_sub_relay;
    json["tspeakOn"] = (tspeakOn?"checked":"");
    json["tspeak_server"] = tspeak_server;
    json["tspeak_channal"] = tspeak_channal;
    json["tspeak_wapi"] = tspeak_wapi; 
    root = "";
    serializeJson(json, root);
    HTTP.send(200, "text/json", root);
}

void handle_Language() {               
    if(HTTP.arg("lang") == "RU" && lang !=0) {
        lang = 0;
    }
    else if(HTTP.arg("lang") == "BG" && lang !=1) {
        lang = 1;
    }
    else if(HTTP.arg("lang") == "EN" && lang !=2) {
        lang = 2;
    }
    else {
        lang = 2;
    }
    Serial.print("Set lang: ");    Serial.println(HTTP.arg("lang").c_str());
    saveConfig();
    HTTP.send(200, "text/plain", "OK");
}

void handle_Set_Ssid() {
    ssid = HTTP.arg("ssid").c_str();            
    password = HTTP.arg("password").c_str();   
    ssidAP = HTTP.arg("ssidAP").c_str();        
    passwordAP = HTTP.arg("passwordAP").c_str(); 
    SSDP_Name = HTTP.arg("ssdp").c_str();
    if (passwordAP.length() != 0) {
        if (passwordAP.length() < 8 || passwordAP.length() > 63)  passwordAP = "";  
    }
    //if (strlen(_apPassword) < 8 || strlen(_apPassword) > 63) {
    myWIFI.setConfigWIFI(ssid.c_str(), password.c_str(), SSDP_Name.c_str(), ssidAP.c_str(), passwordAP.c_str()); 
    Serial.println("Set SSID: " + ssid + ", Set password: " + password + ", Set SSID AP: " + ssidAP + ", Set AP password: " + passwordAP + ", SSDP: " + SSDP_Name);
    HTTP.send(200, "text/plain", "OK");   
}

void handle_time_zone() {     
    timezone = HTTP.arg("timezone").toInt(); 
    isDayLightSaving = HTTP.arg("isDayLightSaving").toInt();
    saveConfig();
    if (typeSync == 1) { //NTP
        myESPTime.setTimeParam(false, timezone, isDayLightSaving, sNtpServerName);
        myESPTime.timeSynch();
    }
    Serial.println("NTP Time Zone: " + String(timezone) + ",  isDayLightSaving: " + String(isDayLightSaving));
    HTTP.send(200, "text/plain", "OK");
}

void handle_ntp_server() {   
    uint8_t oldtypeSync = typeSync;           
    sNtpServerName = HTTP.arg("ntpserver").c_str(); 
    typeSync = HTTP.arg("use_sync").toInt();
    if (typeSync == 2) {//GPS but no
        typeSync = oldtypeSync;
    }
    if (typeSync == 0) useRTC = true; else useRTC = false;
    myESPTime.setTimeParam(useRTC, timezone, isDayLightSaving, sNtpServerName);
    saveConfig();
    myESPTime.timeSynch();
    Serial.println("sNtpServerName: " + sNtpServerName + ", typeSync: " + typeSync);
    HTTP.send(200, "text/plain", "OK");
}

void handle_Time() {
    myESPTime.timeSynch();
    HTTP.send(200, "text/plain", "OK"); 
}

void handle_TimeNew() {
    struct tm timeman;
    time_t epoch_time;
    uint8_t new_h = HTTP.arg("time_h").toInt(); 
    uint8_t new_m = HTTP.arg("time_m").toInt();
    uint8_t new_s = HTTP.arg("time_s").toInt();
    uint8_t new_day = HTTP.arg("date_day").toInt(); 
    uint8_t new_month = HTTP.arg("date_month").toInt();
    uint16_t new_year = HTTP.arg("date_year").toInt();    
    timeman.tm_hour = new_h; 
    timeman.tm_min = new_m;
    timeman.tm_sec = new_s;
    timeman.tm_mday = new_day;
    timeman.tm_mon = new_month - 1;
    timeman.tm_year = new_year - 1900; 
    epoch_time = mktime(&timeman);
    printf("Setting time: %s", asctime(&timeman)); 
    Serial.print(F("Setting time UNIX: ")); Serial.println(epoch_time);   
    useRTC = true;
    myESPTime.setTimeParam(useRTC, timezone, isDayLightSaving, sNtpServerName);
    //myESPTime.setTimeRTC(year_s, mon_s, mday_s, hour_s, min_s, sec_s);
    myESPTime.setTimeRTC(epoch_time);
    saveConfig();
    myESPTime.timeSynch();
    HTTP.send(200, "text/plain", "OK"); 
}


void handle_Restart() {
  String restart = HTTP.arg("device");       
  if (restart == "ok") {              
    HTTP.send(200, "text/plain", "Reset OK");
    ESP.restart();                               
  }
  else {                                       
    HTTP.send(200, "text/plain", "No Reset"); 
  }
}

void handle_resetConfig() {
  String restart = HTTP.arg("device");
  if(restart == "ok") {
    //SPIFFS.format();
    //SPIFFS.remove("/myconfig.json");
    LittleFS.remove("/myconfig.json");
    Serial.println("ESP erase Config file");
    delay(3000);
    HTTP.send(200, "text/plain", "OK");
    delay(3000);
    ESP.restart();
  }
}

void handle_MQTTOn() {
    HTTP.arg("mqttOn").toInt()==1?mqttOn=true:mqttOn=false;
    saveConfig();                 
    Serial.println("mqttOn: " + mqttOn);
    HTTP.send(200, "text/plain", "OK");   
}

void handle_SensorOn() {
    HTTP.arg("sensorContrOn").toInt()==1?sensorContrOn=true:sensorContrOn=false;
    saveConfig();                 
    Serial.println("sensorContrOn: " + sensorContrOn);
    HTTP.send(200, "text/plain", "OK");   
}

void handle_sensorSet() {
    dataCorrectTemp = HTTP.arg("datacorrect").toInt();
    HTTP.arg("hpa").toInt()==1?hpa=true:hpa=false;
    sensorContrType = HTTP.arg("sensorContrType").toInt();
    sensorTempLevel = HTTP.arg("sensorTempLevel").toFloat();
    sensorTempGister = HTTP.arg("sensorTempGister").toFloat();
    saveConfig();                 
    Serial.println("datacorrectTemp: " + String(dataCorrectTemp) + ", hpa: " + String(hpa));
    HTTP.send(200, "text/plain", "OK"); 
}

void handle_MQTTSet() {
    HTTP.arg("mqttOn").toInt()==1?mqttOn=true:mqttOn=false;
    mqtt_server = HTTP.arg("mqtt_server").c_str();
    mqtt_port = HTTP.arg("mqtt_port").toInt();
    mqtt_user = HTTP.arg("mqtt_user").c_str();
    mqtt_pass = HTTP.arg("mqtt_pass").c_str();
    mqtt_name = HTTP.arg("mqtt_name").c_str();
    mqtt_pub_temp = HTTP.arg("mqtt_pub_temp").c_str();
    mqtt_pub_hum = HTTP.arg("mqtt_pub_hum").c_str();
    mqtt_pub_press = HTTP.arg("mqtt_pub_press").c_str();
    mqtt_sub_relay = HTTP.arg("mqtt_sub_relay").c_str();
    saveConfig();                 
    Serial.println("mqtt_server: " + mqtt_server + ", mqtt_user: " + mqtt_user + ", mqtt_name: " + mqtt_name);
    HTTP.send(200, "text/plain", "OK"); 
}

void handle_tspeakSet() {
  HTTP.arg("tspeakOn").toInt()==1?tspeakOn=true:tspeakOn=false;
  tspeak_server = HTTP.arg("tspeak_server").c_str();
  tspeak_channal = HTTP.arg("tspeak_channal").toInt();
  tspeak_wapi = HTTP.arg("tspeak_wapi").c_str();
  saveConfig();                 
  Serial.println("tspeak_server: " + tspeak_server + ", tspeak_channal: " + tspeak_channal + ", tspeak_wapi: " + tspeak_wapi);
  HTTP.send(200, "text/plain", "OK"); 
}

void handle_tspeakOn() {
  HTTP.arg("tspeakOn").toInt()==1?tspeakOn=true:tspeakOn=false;
  saveConfig();                
  Serial.println("tspeakOn: " + tspeakOn);
  HTTP.send(200, "text/plain", "OK"); 
}  

//*****************
void handle_setwcount() {          
    totalWorkMinute = HTTP.arg("totalWorkMinute").toInt();
    resourceMinute = HTTP.arg("resourceMinute").toInt(); 
    HTTP.arg("writeEPPROM").toInt()==1?writeEPPROM=true:writeEPPROM=false;    
    //if(writeEPPROM) saveCountEEPROM(addrCount, totalWorkMinute);
    saveCountEEPROM(addrCount, totalWorkMinute);    
    delay(100);
    saveConfig();
    Serial.println("resourceMinute: " + String(resourceMinute) + ", totalWorkMinute: " + String(totalWorkMinute));
    HTTP.send(200, "text/plain", "OK");   
}

void handle_saveEPROMOn() {
  HTTP.arg("writeEPPROM").toInt()==1?writeEPPROM=true:writeEPPROM=false;  
  saveConfig();                
  Serial.println("writeEPPROM: " + writeEPPROM);
  HTTP.send(200, "text/plain", "OK"); 
}  

void handle_setautoff() {
    perDelayDirON = HTTP.arg("perDelayDirON").toInt() * 60000; 
    HTTP.arg("auto_off").toInt()==1?auto_off=true:auto_off=false;    
    saveConfig();
    Serial.println("perDelayDirON: " + String(perDelayDirON) + ", auto_off: " + String(auto_off));
    HTTP.send(200, "text/plain", "OK");       
}

void handle_setworktime() {
    if (HTTP.arg("onWork1").toInt()==1) mySchedule[0].onWork = true; else mySchedule[0].onWork = false;
    if (HTTP.arg("onWork2").toInt()==1) mySchedule[1].onWork = true; else mySchedule[1].onWork = false;
    if (HTTP.arg("onWork3").toInt()==1) mySchedule[2].onWork = true; else mySchedule[2].onWork = false;
    if (HTTP.arg("onWork4").toInt()==1) mySchedule[3].onWork = true; else mySchedule[3].onWork = false;
    mySchedule[0].workFrom = HTTP.arg("workFrom1").toFloat(); mySchedule[0].workTo = HTTP.arg("workTo1").toFloat();
    mySchedule[1].workFrom = HTTP.arg("workFrom2").toFloat(); mySchedule[1].workTo = HTTP.arg("workTo2").toFloat();
    mySchedule[2].workFrom = HTTP.arg("workFrom3").toFloat(); mySchedule[2].workTo = HTTP.arg("workTo3").toFloat();
    mySchedule[3].workFrom = HTTP.arg("workFrom4").toFloat(); mySchedule[3].workTo = HTTP.arg("workTo4").toFloat();
    mySchedule[0].sched_stat = HTTP.arg("sched1_stat").toInt();
    mySchedule[1].sched_stat = HTTP.arg("sched2_stat").toInt();
    mySchedule[2].sched_stat = HTTP.arg("sched3_stat").toInt();
    mySchedule[3].sched_stat = HTTP.arg("sched4_stat").toInt();
    saveConfig();
    HTTP.send(200, "text/plain", "OK");
    digitalWrite(PIN_LED1, statusRelayON);//for indicate if oldStatusRelay == statusRelayON
    Serial.print("onWork1 "); Serial.println(mySchedule[1].onWork); 
}

void handle_setonoff() {
    if(HTTP.arg("onrelay").toInt()==1) {
        statusRelayON = 1;
        checkSched = 0;
        timeSchedOFF = millis();
    }
    else {
        statusRelayON = 0;
        checkSched = 1;    
    }
    Serial.print(F("HTTP ")); Serial.println(statusRelayON? "ON" : "OFF");
    HTTP.send(200, "text/plain", "OK");  
}

bool handleFileRead(String path) {
    if (path.endsWith("/")) path += "index.html";
    String contentType = getContentType(path);
    String pathWithGz = path + ".gz";
    //if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) {
    if (LittleFS.exists(pathWithGz) || LittleFS.exists(path)) {
        //if (SPIFFS.exists(pathWithGz))
        if (LittleFS.exists(pathWithGz))
        path += ".gz";
        //File file = SPIFFS.open(path, "r");
        File file = LittleFS.open(path, "r");
        size_t sent = HTTP.streamFile(file, contentType);
        file.close();
        return true;
    }
    return false;
}

String getContentType(String filename) {
    if (HTTP.hasArg("download")) return "application/octet-stream";
    else if (filename.endsWith(".htm")) return "text/html";
    else if (filename.endsWith(".html")) return "text/html";
    else if (filename.endsWith(".json")) return "application/json";
    else if (filename.endsWith(".css")) return "text/css";
    else if (filename.endsWith(".js")) return "application/javascript";
    else if (filename.endsWith(".png")) return "image/png";
    else if (filename.endsWith(".gif")) return "image/gif";
    else if (filename.endsWith(".jpg")) return "image/jpeg";
    else if (filename.endsWith(".ico")) return "image/x-icon";
    else if (filename.endsWith(".xml")) return "text/xml";
    else if (filename.endsWith(".pdf")) return "application/x-pdf";
    else if (filename.endsWith(".zip")) return "application/x-zip";
    else if (filename.endsWith(".gz")) return "application/x-gzip";
    return "text/plain";
}

void init_SSDP(void) {
    HTTP.on("/description.xml", HTTP_GET, []() {
        SSDP.schema(HTTP.client());
    });
    SSDP.setDeviceType("upnp:rootdevice");
    SSDP.setSchemaURL("description.xml");
    SSDP.setHTTPPort(80);
    SSDP.setName(SSDP_Name);
    SSDP.setSerialNumber("001788102201");
    SSDP.setURL("/");
    SSDP.setModelName(nName);
    SSDP.setModelNumber(nVersion);
    SSDP.setModelURL("http://" + (WiFi.status() != WL_CONNECTED ? WiFi.softAPIP() : WiFi.localIP()));
    SSDP.setManufacturer("LIGHTWELL");
    SSDP.setManufacturerURL("https://led-lightwell.eu");
    SSDP.begin();
}