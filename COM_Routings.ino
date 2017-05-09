/**** METHOD: Modbus ****************************/
void Modbus() {
  //Modbus starten
  Mb.Run();
  
  //Analog inputs 0-1023
  //Mb.R[0] = TemperatureId0*100;
  //Mb.R[2] = TemperatureId1*100;
  //digitalWrite(led, Mb.C[4]);
  ipsmbhours = Mb.R[5];
  ipsmbminutes= Mb.R[6];
  ipsmbWochenTag= Mb.R[7];
  ipsmbTag= Mb.R[8];
  ipsmbMonat= Mb.R[9];
  ipsmbJahr= Mb.R[10];
  
  //Mb.R[20] = days;
  //Mb.R[21] = hours;
  //Mb.R[22] = minutes;
  //Mb.R[23] = seconds;

  ipsmbTempA = Mb.R[30]/100;
  ipsmbTempWZ = Mb.R[31]/100;
  ipsmbTempSZ = Mb.R[32]/100;
  ipsmbTempCZ = Mb.R[33]/100;
  ipsmbTempKZ = Mb.R[34]/100;

  //Serial.println(ipsmbhours);
}
/***********************************************/

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

