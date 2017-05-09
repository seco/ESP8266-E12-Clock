/**** METHOD: NTP - Client ****************************/
void Sekunde0() {
  if (Sekunden == 0) {
    
  }
}

void Mitternacht() {
  if(Stunde == 0 and Minute == 0 and Sekunden == 0) {
    ntpabgleich();
  }
}

void ntpabgleich() {
  Udp.begin(localPort);
  if (!getNtpTime(ntpServerName1)) { if (!getNtpTime(ntpServerName2)) { getNtpTime(ntpServerName3); } }
  Udp.stop();
}

void ZeitToVarWrite() {
  local = CE.toLocal(now(), &tcr);
  if(TIMEDEBUG == 0){printTime(local);}
  if(DEBUG == 1 and TIMEDEBUG == 0){if(Sekunden == 0){printTime(local);}}

  Stunde = hour(local);
  Minute = minute(local);
  Sekunden = second(local);
  WochenTag = weekday(local);
  Tag = day(local);
  Monat = month(local);
  Jahr = year(local);
}

bool getNtpTime(char* ntpServerName) {
    if(TIMEDEBUG == 1){Serial.print(F("NTP request..."));}

/*
    if (timeStatus() == timeSet) {
        Serial.println(F("not necessary"));
        return true;
    }
*/
 
    IPAddress ntpServerIP; // NTP server's ip address
 
    while (Udp.parsePacket() > 0); // discard any previously received packets
    if(TIMEDEBUG == 1){Serial.println(F("Transmit NTP Request"));}
    // get a random server from the pool
    WiFi.hostByName(ntpServerName, ntpServerIP);
    if(TIMEDEBUG == 1)
    {
      Serial.print(ntpServerName);
      Serial.print(": ");
      Serial.println(ntpServerIP);
    }
    sendNTPpacket(ntpServerIP);
    uint32_t beginWait = millis();
    while (millis() - beginWait < 1500) {
        int size = Udp.parsePacket();
        if (size >= NTP_PACKET_SIZE) {
            if(TIMEDEBUG == 1){Serial.println(F("Receive NTP Response"));}
            Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
            unsigned long secsSince1900;
            // convert four bytes starting at location 40 to a long integer
            secsSince1900 = (unsigned long)packetBuffer[40] << 24;
            secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
            secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
            secsSince1900 |= (unsigned long)packetBuffer[43];
            setTime(secsSince1900 - 2208988800UL);
            //setTime(23, 55, 0, 30, 3, 2016); //simulate time for test
            return true;
        }
    }
    if(TIMEDEBUG == 1){Serial.println(F("FATAL ERROR : No NTP Response."));}
    return false; // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address) {
    // set all bytes in the buffer to 0
    memset(packetBuffer, 0, NTP_PACKET_SIZE);
    // Initialize values needed to form NTP request
    // (see URL above for details on the packets)
    packetBuffer[0] = 0b11100011;   // LI, Version, Mode
    packetBuffer[1] = 0;     // Stratum, or type of clock
    packetBuffer[2] = 6;     // Polling Interval
    packetBuffer[3] = 0xEC;  // Peer Clock Precision
                             // 8 bytes of zero for Root Delay & Root Dispersion
    packetBuffer[12] = 49;
    packetBuffer[13] = 0x4E;
    packetBuffer[14] = 49;
    packetBuffer[15] = 52;
    // all NTP fields have been given values, now
    // you can send a packet requesting a timestamp:
    Udp.beginPacket(address, 123); //NTP requests are to port 123
    Udp.write(packetBuffer, NTP_PACKET_SIZE);
    Udp.endPacket();
}
 
//Function to return the compile date and time as a time_t value
time_t compileTime(void) {
#define FUDGE 25        //fudge factor to allow for compile time (seconds, YMMV)
    char *compDate = __DATE__, *compTime = __TIME__, *months = "JanFebMarAprMayJunJulAugSepOctNovDec";
    char chMon[3], *m;
    int d, y;
    tmElements_t tm;
    time_t t;
    strncpy(chMon, compDate, 3);
    chMon[3] = '\0';
    m = strstr(months, chMon);
    tm.Month = ((m - months) / 3 + 1);
    tm.Day = atoi(compDate + 4);
    tm.Year = atoi(compDate + 7) - 1970;
    tm.Hour = atoi(compTime);
    tm.Minute = atoi(compTime + 3);
    tm.Second = atoi(compTime + 6);
    t = makeTime(tm);
    return t + FUDGE;        //add fudge factor to allow for compile time
}

