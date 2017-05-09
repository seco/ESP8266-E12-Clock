void printTime(time_t t) {
    sPrintI00(hour(t));
    sPrintDigits(minute(t));
    sPrintDigits(second(t));
    Serial.print(' ');
    switch (weekday(t))               // depending on which button was pushed, we perform an action  
    { 
      case 1:Serial.print("So");break;
      case 2:Serial.print("Mo");break;
      case 3:Serial.print("Di");break;
      case 4:Serial.print("Mi");break;
      case 5:Serial.print("Do");break;
      case 6:Serial.print("Fr");break;
      case 7:Serial.print("Sa");break;
    }
    Serial.print(' ');
    sPrintI00(day(t));
    Serial.print(' ');
    switch (month(t))               // depending on which button was pushed, we perform an action  
    { 
      case 1:Serial.print("Januar");break;
      case 2:Serial.print("Februar");break;
      case 3:Serial.print("März");break;
      case 4:Serial.print("April");break;
      case 5:Serial.print("Mai");break;
      case 6:Serial.print("Juni");break;
      case 7:Serial.print("Juli");break;
      case 8:Serial.print("August");break;
      case 9:Serial.print("September");break;
      case 10:Serial.print("Oktober");break;
      case 11:Serial.print("November");break;
      case 12:Serial.print("Dezember");break;
    }
    Serial.print(' ');
    Serial.print(year(t));
    Serial.println(' ');
}
 
void sPrintI00(int val) {
    if (val < 10) Serial.print('0');
    Serial.print(val, DEC);
    return;
}
 
void sPrintDigits(int val) {
    Serial.print(':');
    if (val < 10) Serial.print('0');
    Serial.print(val, DEC);
}

void printUrlArg() {
 //Alle Parameter seriell ausgeben
 String message="";
 for (uint8_t i=0; i<server.args(); i++){
 message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
 }
 server.send(200, "text/plain", message);
 Serial.println("");
 Serial.println("HTTP Daten");
 Serial.println(message);
}

