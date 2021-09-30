/*void init_udp() {
    Udp.begin(localUdpPort);
    Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);
}

void listenUDP() {
    uint8_t packetSize = Udp.parsePacket();
    char host[16];
    if (packetSize) {// получаем входящие UDP-пакеты:
        Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
        uint8_t len = Udp.read(incomingPacket, 255);
        if (len > 0) {
            incomingPacket[len] = '\0';
        }
        Serial.printf("UDP packet contents: %s\n", incomingPacket);
        // отправляем ответ на IP-адрес и порт, с которых пришел пакет: 
        SSDP_Name = myWIFI.getNameSSDP();
        SSDP_Name.toCharArray(host, SSDP_Name.length()+1);
        Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
        Udp.write(host);
        Udp.endPacket();
    }
}*/