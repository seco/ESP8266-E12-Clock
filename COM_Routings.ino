void IPSymconCheck() {
  if (!clientips.connected() and Sekunden == 10) {
    if(DEBUG == 1) {
      Serial.println("Verbindung zum Client wurde getrennt.");
    }
    clientips.stop();
    delay(3000);
    if (clientips.connect(ips, 10002)) {
      clientips.println("VERBUNDEN!");  
      if(DEBUG == 1) {
        Serial.println("VERBUNDEN!");
      }
    } else {
      if(DEBUG == 1) {
        Serial.println("Verbindung zum Client wurde getrennt.");
      }
    }
  }
}

void Temperaturen_holen() {
  //clientips.connect(ips, 10002);
  clientips.print("ESP-CLOCK-SZ---TEMPS\r\n"); 
  delay(1000);
  //clientips.stop();
  if(DEBUG == 1) {
    Serial.println("Temperaturen und Dimmwert werden geholt");
  }
}

