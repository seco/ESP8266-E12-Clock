void handleRoot() {
 int sec = millis() / 1000;
 int min = sec / 60;
 int hr = min / 60;
 int day = hr / 24;
 String message ="<!DOCTYPE html><html lang=\"de\"><head><meta charset=\"UTF-8\" http-equiv=\"refresh\" content=\"60\"/><title>"+ESP_OTA_HOSTNAME+"</title></head>";
 //message += "<style>body \{ background-color: #cccccc\; font-family: Arial, Helvetica, Sans-Serif; Color: #000088\; \}</style></head>\";
 message += "<body BGCOLOR=\"#cccccc\" LINK=\"#FFFF00\" ALINK=\"#FF0000\" VLINK=\"#FFFFCC\">";
 message += "<FONT FACE=\"Arial, Helvetica, Sans-Serif\" SIZE=\"3\" COLOR=\"#000088\">";
 message += "<h1>Hallo vom "+String(ESP_OTA_HOSTNAME)+" !!!</h1>";

 message += "<h2>Datum und Uhrzeit</h2>";
 message += "<table style=\"text-align: left; width: 400px; height: 100px;\" border=\"1\" cellpadding=\2\" cellspacing=\"2\">";
 message += "<tbody>";
 message += "<tr>";
 message += "<td style=\"height: 30px; width: 300px; vertical-align: middle; text-align: center;\">Wochentag<br>";
 message += "</td>";
 message += "<td style=\"height: 30px; width: 100px; vertical-align: middle; text-align: center;\">";
 switch (WochenTag)
 {
  case 1:message += "Sonntag<br>";break;
  case 2:message += "Montag<br>";break;
  case 3:message += "Dienstag<br>";break;
  case 4:message += "Mittwoch<br>";break;
  case 5:message += "Donnerstag<br>";break;
  case 6:message += "Freitag<br>";break;
  case 7:message += "Samstag<br>";break;
  default:message += "<br>";break;
 }
 message += "</td>";
 message += "</tr>";
 message += "<tr>";
 message += "<td style=\"height: 30px; width: 300px; vertical-align: middle; text-align: center;\">Datum<br>";
 message += "</td>";
 message += "<td style=\"height: 30px; width: 100px; vertical-align: middle; text-align: center;\">";
 if(Tag<10) message += "0";
 message += String(Tag)+".";
 if(Monat<10) message += "0";
 message += String(Monat)+".";
 if(Jahr<10) message += "000";
 message += String(Jahr)+"<br>";
 message += "</td>";
 message += "</tr>";
 message += "<tr>";
 message += "<td style=\"height: 30px; width: 300px; vertical-align: middle; text-align: center;\">Uhrzeit<br>";
 message += "</td>";
 message += "<td style=\"height: 30px; width: 100px; vertical-align: middle; text-align: center;\">";
 if(Stunde<10) message += "0";
 message += String(Stunde)+":";
 if(Minute<10) message += "0";
 message += String(Minute)+":";
 if(Sekunden<10) message += "0";
 message += String(Sekunden)+"<br>";
 message += "</td>";
 message += "</tr>";
 message += "</tbody>";
 message += "</table>";

 message += "<h2>Temperaturen</h2>";
 message += "<table style=\"text-align: left; width: 400px; height: 148px;\" border=\"1\" cellpadding=\"2\" cellspacing=\"2\">";
 message += "<tbody>";
 message += "<tr>";
 message += "<td style=\"height: 30px; width: 300px; vertical-align: middle; text-align: center;\">Außen Temperatur<br>";
 message += "</td>";
 message += "<td style=\"height: 30px; width: 100px; vertical-align: middle; text-align: center;\">";
 message += String(ipsTempA, 2)+" °C<br>";
 message += "</td>";
 message += "</tr>";
 message += "<tr>";
 message += "<td style=\"height: 30px; width: 300px; vertical-align: middle; text-align: center;\">Wohnzimmer Temperatur<br>";
 message += "</td>";
 message += "<td style=\"height: 30px; width: 100px; vertical-align: middle; text-align: center;\">";
 message += String(ipsTempWZ, 2)+" °C<br>";
 message += "</td>";
 message += "</tr>";
 message += "<tr>";
 message += "<td style=\"height: 30px; width: 300px; vertical-align: middle; text-align: center;\">Schlafzimmer Temperatur<br>";
 message += "</td>";
 message += "<td style=\"height: 30px; width: 100px; vertical-align: middle; text-align: center;\">";
 message += String(ipsTempSZ, 2)+" °C<br>";
 message += "</td>";
 message += "</tr>";
 message += "<tr>";
 message += "<td style=\"height: 30px; width: 300px; vertical-align: middle; text-align: center;\">Computerzimmer Temperatur<br>";
 message += "</td>";
 message += "<td style=\"height: 30px; width: 100px; vertical-align: middle; text-align: center;\">";
 message += String(ipsTempCZ, 2)+" °C<br>";
 message += "</td>";
 message += "</tr>";
 message += "<tr>";
 message += "<td style=\"height: 30px; width: 300px; vertical-align: middle; text-align: center;\">Kinderzimmer Temperatur<br>";
 message += "</td>";
 message += "<td style=\"height: 30px; width: 100px; vertical-align: middle; text-align: center;\">";
 message += String(ipsTempKZ, 2)+" °C<br>";
 message += "</td>";
 message += "</tr>";
 message += "</tbody>";
 message += "</table>";

 message += "<h2>Anruf Monitor</h2>";
 message += "<table style=\"text-align: left; width: 400px; height: 100px;\" border=\"1\" cellpadding=\2\" cellspacing=\"2\">";
 message += "<tbody>";
 message += "<tr>";
 message += "<td style=\"height: 30px; width: 200px; vertical-align: middle; text-align: center;\">Anrufzeit<br>";
 message += "</td>";
 message += "<td style=\"height: 30px; width: 200px; vertical-align: middle; text-align: center;\">" + String(lastcalltime);
 message += "</td>";
 message += "</tr>";
 message += "<tr>";
 message += "<td style=\"height: 30px; width: 200px; vertical-align: middle; text-align: center;\">Angerufene Nummer<br>";
 message += "</td>";
 message += "<td style=\"height: 30px; width: 200px; vertical-align: middle; text-align: center;\">" + String(lastcallnumber);
 message += "</td>";
 message += "</tr>";
 message += "<tr>";
 message += "<td style=\"height: 30px; width: 200px; vertical-align: middle; text-align: center;\">Auf der Rufnummer<br>";
 message += "</td>";
 message += "<td style=\"height: 30px; width: 200px; vertical-align: middle; text-align: center;\">" + String(lastcallinnumber);
 message += "</td>";
 message += "</tr>";
 message += "</tbody>";
 message += "</table>";

 message += "<h2>LCD Backlight</h2>";
 message += "<table style=\"text-align: left; width: 400px; height: 30px;\" border=\"1\" cellpadding=\2\" cellspacing=\"2\">";
 message += "<tbody>";
 message += "<tr>";
 message += "<td style=\"height: 30px; width: 200px; vertical-align: middle; text-align: center;\">LCD Backlight Dimwert<br>";
 message += "</td>";
 message += "<td style=\"height: 30px; width: 200px; vertical-align: middle; text-align: center;\">" + String(LCDBackgroundLEDDim);
 message += "</td>";
 message += "</tr>";
 message += "</tbody>";
 message += "</table>";

 message += "<br>";
 message += "<h1>Uptime: "+String(day)+" Tage ";
 if(hr%23<10) message += "0";
 message += String(hr%23)+":";
 if(min%60<10) message += "0";
 message += String(min%60)+":";
 if(sec%60<10) message += "0";
 message += String(sec%60)+"</h1>";
 message += "</body>";
 message += "</html>";
 server.send(200, "text/html", message);
}

void handleNotFound(){
 String message = "File Not Found\n\n";
 message += "URI: ";
 message += server.uri();
 message += "\nMethod: ";
 message += (server.method() == HTTP_GET)?"GET":"POST";
 message += "\nArguments: ";
 message += server.args();
 message += "\n";
 for (uint8_t i=0; i<server.args(); i++){
   message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
 }
 server.send(404, "text/plain", message);
}

/* HTTP Post Datum & Zeit
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// HTTP Post Datum & Zeit
void handleDatumZeit() {
 if(HTTPDEBUG == 1){printUrlArg();}
 String Tagtemp =server.arg("Tag");
 String Monattemp =server.arg("Monat");
 String Jahrtemp =server.arg("Jahr");
 String WochenTagtemp =server.arg("WochenTag");
 String Stundetemp =server.arg("Stunde");
 String Minutetemp =server.arg("Minute");
 int tempTag = Tagtemp.toInt();
 int tempMonat = Monattemp.toInt();
 int tempJahr = Jahrtemp.toInt();
 int tempWochenTag = WochenTagtemp.toInt();
 int tempStunde = Stundetemp.toInt();
 int tempMinute = Minutetemp.toInt();
 if (Tag!=tempTag) {
 Tag=tempTag;
 }
 if (Monat!=tempMonat) {
 Monat=tempMonat;
 }
 if (Jahr!=tempJahr) {
 Jahr=tempJahr;
 }
 if (WochenTag!=tempWochenTag) {
 WochenTag=tempWochenTag;
 }
 if (Stunde!=tempStunde) {
 Stunde=tempStunde;
 }
 if (Minute!=tempMinute) {
 Minute=tempMinute;
 }

 String message="";
 String tempstr= String(Tag); 
 message += "Datum : " + tempstr +".";
 tempstr= String(Monat); 
 message += tempstr +".";
 tempstr= String(Jahr); 
 message += tempstr +"\n";
 tempstr= String(WochenTag, 0); 
 message += "Wochentag : " + tempstr +"\n";
 tempstr= String(Stunde, 0); 
 message += "Uhrzeit : " + tempstr +":";
 tempstr= String(Minute, 0); 
 message += tempstr +"\n";
 server.send(200, "text/plain", message);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// Datum

void handleDatum() {
 if(HTTPDEBUG == 1){printUrlArg();}
 String Tagtemp =server.arg("Tag");
 String Monattemp =server.arg("Monat");
 String Jahrtemp =server.arg("Jahr");
 String WochenTagtemp =server.arg("WochenTag");
 int tempTag = Tagtemp.toInt();
 int tempMonat = Monattemp.toInt();
 int tempJahr = Jahrtemp.toInt();
 int tempWochenTag = WochenTagtemp.toInt();
 if (Tag!=tempTag) {
 Tag=tempTag;
 }
 if (Monat!=tempMonat) {
 Monat=tempMonat;
 }
 if (Jahr!=tempJahr) {
 Jahr=tempJahr;
 }
 if (WochenTag!=tempWochenTag) {
 WochenTag=tempWochenTag;
 }

 String message="";
 String tempstr= String(Tag); 
 message += "Datum : " + tempstr +".";
 tempstr= String(Monat); 
 message += tempstr +".";
 tempstr= String(Jahr); 
 message += tempstr +"\n";
 tempstr= String(WochenTag); 
 message += "Wochentag : " + tempstr +"\n";
 server.send(200, "text/plain", message);
}

void handleDatumTag() {
 if(HTTPDEBUG == 1){printUrlArg();}
 String stemp =server.arg("wert");
 int tempTag = stemp.toInt();
 if (Tag!=tempTag) {
 Tag=tempTag;
 }

 String message="";
 String tempstr= String(Tag); 
 message += "Tag : " + tempstr +"\n";
 server.send(200, "text/plain", message);
}

void handleDatumMonat() {
 if(HTTPDEBUG == 1){printUrlArg();}
 String stemp =server.arg("wert");
 int tempMonat = stemp.toInt();
 if (Tag!=tempMonat) {
 Monat=tempMonat;
 }

 String message="";
 String tempstr= String(Monat); 
 message += "Monat : " + tempstr +"\n";
 server.send(200, "text/plain", message);
}

void handleDatumJahr() {
 if(HTTPDEBUG == 1){printUrlArg();}
 String stemp =server.arg("wert");
 int tempJahr = stemp.toInt();
 if (Jahr!=tempJahr) {
 Jahr=tempJahr;
 }

 String message="";
 String tempstr= String(Jahr); 
 message += "Jahr : " + tempstr +"\n";
 server.send(200, "text/plain", message);
}

void handleDatumWochentag() {
 if(HTTPDEBUG == 1){printUrlArg();}
 String stemp =server.arg("wert");
 int tempWochenTag = stemp.toInt();
 if (WochenTag!=tempWochenTag) {
 WochenTag=tempWochenTag;
 }

 String message="";
 String tempstr= String(WochenTag); 
 message += "Wochentag : " + tempstr +"\n";
 server.send(200, "text/plain", message);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// Zeit

void handleZeit() {
 if(HTTPDEBUG == 1){printUrlArg();}
 String Stundetemp =server.arg("Stunde");
 String Minutetemp =server.arg("Minute");
 int tempStunde = Stundetemp.toInt();
 int tempMinute = Minutetemp.toInt();
 if (Stunde!=tempStunde) {
 Stunde=tempStunde;
 }
 if (Minute!=tempMinute) {
 Minute=tempMinute;
 }

 String message="";
 String tempstr= String(Stunde); 
 message += "Uhrzeit : " + tempstr +":";
 tempstr= String(Minute); 
 message += tempstr +"\n";
 server.send(200, "text/plain", message);
}

void handleZeitStunde() {
 if(HTTPDEBUG == 1){printUrlArg();}
 String stemp =server.arg("wert");
 int tempStunde = stemp.toInt();
 if (Stunde!=tempStunde) {
 Stunde=tempStunde;
 }

 String message="";
 String tempstr= String(Stunde); 
 message += "Stunde : " + tempstr +"\n";
 server.send(200, "text/plain", message);
}

void handleZeitMinute() {
 if(HTTPDEBUG == 1){printUrlArg();}
 String stemp =server.arg("wert");
 int tempMinute = stemp.toInt();
 if (Minute!=tempMinute) {
 Minute=tempMinute;
 }

 String message="";
 String tempstr= String(Minute); 
 message += "Minute : " + tempstr +"\n";
 server.send(200, "text/plain", message);
}
*/

void handleTemperatur() {
 if(HTTPDEBUG == 1){printUrlArg();}
 String TAtemp =server.arg("TA");
 String TWZtemp =server.arg("TWZ");
 String TSZtemp =server.arg("TSZ");
 String TCZtemp =server.arg("TCZ");
 String TKZtemp =server.arg("TKZ");
 float tempTA = TAtemp.toFloat();
 float tempTWZ = TWZtemp.toFloat();
 float tempTSZ = TSZtemp.toFloat();
 float tempTCZ = TCZtemp.toFloat();
 float tempTKZ = TKZtemp.toFloat();
 if (ipsTempA!=tempTA) {
 ipsTempA=tempTA;
 }
 if (ipsTempWZ!=tempTWZ) {
 ipsTempWZ=tempTWZ;
 }
 if (ipsTempSZ!=tempTSZ) {
 ipsTempSZ=tempTSZ;
 }
 if (ipsTempCZ!=tempTCZ) {
 ipsTempCZ=tempTCZ;
 }
 if (ipsTempKZ!=tempTKZ) {
 ipsTempKZ=tempTKZ;
 }

 String message="Temperaturen\n";
 String tempstr= String(ipsTempA, 2); 
 message += "Aussen         : " + tempstr +"\n";
 tempstr= String(ipsTempWZ, 2); 
 message += "Wohnzimmer     : " + tempstr +"\n";
 tempstr= String(ipsTempSZ, 2); 
 message += "Schlafzimmer   : " + tempstr +"\n";
 tempstr= String(ipsTempCZ, 2); 
 message += "Computerzimmer : " + tempstr +"\n";
 tempstr= String(ipsTempKZ, 2); 
 message += "Kinderzimmer   : " + tempstr +"\n";
 server.send(200, "text/plain", message);
}

void handleLCDDim() {
 if(HTTPDEBUG == 1){printUrlArg();}
 String LCDBackgroundLEDDimtemp =server.arg("LCDDimWert");
 int tempLCDBackgroundLEDDim = LCDBackgroundLEDDimtemp.toInt();
 if (LCDBackgroundLEDDim!=tempLCDBackgroundLEDDim) {
 LCDBackgroundLEDDim=tempLCDBackgroundLEDDim;
 }

 String message="LCD Dim Wert\n";
 String tempstr= String(LCDBackgroundLEDDim); 
 message += "Dim Wert : " + tempstr +"\n";
 server.send(200, "text/plain", message);
}

