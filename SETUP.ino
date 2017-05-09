void setup() {
  initDebug();  
  initSerial();
  initLCD();
  initWiFi();
  
  FirmwareUpdate();
  
  clientips.connect(ips, 10002);

  inithttpserver();
  initirserver();
  initArduinoOTA();
  initfbconnect();
  initLCDBegruesungstext();
  
  LCDTempInnen = 1;
  LCDTempAussen = 0;

  ntpabgleich();
  
  runner.startNow();  // set point-in-time for scheduling start
}

