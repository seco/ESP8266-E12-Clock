void FirmwareUpdate() {
  // Überprüfen der Firmwareversion des programmms aud dem Server
  int FirmwareNeu = 0;
  FirmUpdate.begin(URL_Mit_Version_Info);     // Webseite aufrufen
  int httpCode = FirmUpdate.GET();            // Antwort des Servers einlesen
  if (httpCode == HTTP_CODE_OK)               // Wenn Antwort OK
  {
    String payload = FirmUpdate.getString();        // Webseite einlesen
    FirmwareNeu = payload.toInt();            // Zahl aus Sting bilden
  } 
  FirmUpdate.end();
  
  if (FirmwareNeu > FirmwareVer)              // Firmwareversion mit aktueller vergleichen
  {
    LCDwrite(1, 0, "ESP OverTheAir FLASH");
    LCDwrite(2, 4, "HTTP Update");
    if(DEBUG == 1) {
      Serial.println("Neue Firmware verfuegbar");
      Serial.println("Starte Download");
    }
    ESPhttpUpdate.rebootOnUpdate(false);// reboot abschalten, wir wollen erst Meldungen ausgeben
    t_httpUpdate_return ret = ESPhttpUpdate.update(URL_Mit_Firmware);
    switch (ret)
    {
      case HTTP_UPDATE_FAILED:
        if(DEBUG == 1) {
          Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        }
      break;
      case HTTP_UPDATE_OK:
        if(DEBUG == 1) {
          Serial.println("Update erfolgreich");
          Serial.flush();
          Serial.println("Reset");
          Serial.flush();
        }
        delay(1);
        ESP.reset();
        delay(100);
        break;
    }
  } 
}

