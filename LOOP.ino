void loop() {
  runner.execute();

  ArduinoOTA.handle();

  server.handleClient();

  if(freshboot == true) {
    Temperaturen_holen();
    freshboot = false;
  }
  
  if(Sekunden == 0) { Temperaturen_holen(); }

  Mitternacht();

  Sekunde0();
  
  ZeitToVarWrite();

  LCDRefresh();

  FBCheck();

  Modbus();

  IPSymconCheck();
  
}

