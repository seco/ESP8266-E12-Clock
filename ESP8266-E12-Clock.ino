////////////////////////////////////////////////////////////
////  ESP8266-E12 (NodeMCU) Info Display                ////
////                                                    ////
////  SDA         - PIN D2 \ I2C Bus                    ////
////  SCL         - PIN D1 /                            ////
////  LCD Dim     - D8                                  ////
////                                                    ////
////  One-Wire    - D7                                  ////
////                                                    ////
////  DHT Sensor  - D6                                  ////
////                                                    ////
////  IR Empfang  - D3                                  ////
////                                                    ////
////  IR Senden   - D5                                  ////
////                                                    ////
////  OTA Flash Funktion für Arduino und über WWW       ////
////                                                    ////
////                                                    ////
////                                                    ////
////                              Powered by DJ Terror  ////
////////////////////////////////////////////////////////////
/**** INCLUDES ********************************************/
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <ArduinoOTA.h>
#include <IRremoteESP8266.h>
#include <TimeLib.h>                            //by Paul Stoffregen, not included in the Arduino IDE !!!     https://github.com/PaulStoffregen/Time
#include <Timezone.h>                           //by Jack Christensen, not included in the Arduino IDE !!!    https://github.com/JChristensen/Timezone
#include <TaskScheduler.h>
#include <DHT.h>
#include <OneWire.h>
/**********************************************************/

/**** GLOBAL VARIABLES ************************************/
#define FirmwareVer  2 // Bitte hier Version der Firmware eintragen
#define URL_Mit_Version_Info "http://192.168.1.90/ESP-Clock-Update/ESP-Clock-Update.txt" 
#define URL_Mit_Firmware "http://192.168.1.90/ESP-Clock-Update/ESP-Clock-Update.bin"
HTTPClient FirmUpdate;

ESP8266WebServer server(80);                    // Port des HTTP Servers
WiFiClient Fritzbox;
WiFiClient clientips;
LiquidCrystal_I2C lcd(0x27,20,4);               // Addresse und Art des Displays

int RECV_PIN = D3;                              // PIN für den Infrarot Empfänger
IRrecv irrecv(RECV_PIN);
decode_results results;

int DEBUG = 0;                                  // Sonstiges Debugging wird automatisch eingeschaltet sobald eine debug Funktion aktiv ist.
int HTTPDEBUG = 1;                              // HTTP Funktion debugging
int IRDEBUG = 1;                                // Infrarot Funktion debugging
int OTADEBUG = 1;                               // OverTheAir Flash debugging
int FBDEBUG = 1;                                // Fritzbox Callmonitor debugging
int TIMEDEBUG = 1;                              // Fritzbox Callmonitor debugging
int LCDINFO = 1;                                // Info Anzeigen auf dem Display

bool freshboot = true;

//const char* ESP_OTA_HOSTNAME = "ESP-CLOCK-SZ";  // local OTA Hostname
const char* ESP_OTA_PASSWORD = "PWdjt0815";     // OTA Passwort
const char* ssid = "DJ Terror"; //Ihr Wlan,Netz SSID eintragen
const char* pass = "1701198123101984"; //Ihr Wlan,Netz Passwort eintragen
//IPAddress ip(192,168,1,146); //Feste IP des neuen Servers, frei wählbar
//IPAddress gateway(192,168,1,1); //Gatway (IP Router eintragen)
//IPAddress subnet(255,255,255,0); //Subnet Maske eintragen
IPAddress ips(192,168,1,155); 
IPAddress FRITZBOX_HOSTNAME(192,168,200,1); 

int SERIAL_BAUD_RATE = 115200;                  // Baud rate for serial communication
int ESP_OTA_PASSWORD_ENABLE = 0;                // OTA Password Enable
int timeOutSec = 0;

int LCDTempInnen  = 0;
int LCDTempAussen = 0;

int Stunde        = 0;
int Minute        = 0;
int Sekunden      = 0;
int WochenTag     = 0;
int Tag           = 0;
int Monat         = 0;
int Jahr          = 0;
float ipsTempA    = 0;
float ipsTempWZ   = 0;
float ipsTempSZ   = 0;
float ipsTempCZ   = 0;
float ipsTempKZ   = 0;

int FRITZBOX_PORT = 1012;                       // Fritzbox Callmonitor Port
int RETRY_TIMEOUT = 5000;                       // Fritzbox Wiederverbindungs Zeit
int callin        = 0;
unsigned long callintime;
int callintimeout = 15000;
int CHECKCONNECTION = 10000;                    // Fritzbox Wiederverbindungs Zeit
unsigned long connectioncheck;
String lastcalltime = "";
String lastcallnumber = "";
String lastcallinnumber = "";

int otaflash        = 0;
String ESP_OTA_HOSTNAME = "";

//UDP
WiFiUDP Udp;
unsigned int localPort = 123;
 
//NTP Server
char ntpServerName1[] = "192.168.1.1";        // NTP Server
char ntpServerName2[] = "ntp1.t-online.de";     // NTP Server 1
char ntpServerName3[] = "time.nist.gov";        // NTP Server 2
 
//Timezone
//Central European Time (Frankfurt, Paris)
TimeChangeRule CEST = { "CEST", Last, Sun, Mar, 2, 120 };     //Central European Summer Time
TimeChangeRule CET = { "CET ", Last, Sun, Oct, 3, 60 };       //Central European Standard Time
Timezone CE(CEST, CET);
TimeChangeRule *tcr;        //pointer to the time change rule, use to get the TZ abbrev
time_t utc, local;
const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

//////////   Multitask Sektion
/// t1 = LCD Dimmer
/// t2 = Infrarot Empfänger

#define LCDBackgroundLED D8
int LCDBackgroundLEDDim = 1024;

void lcddim();
void irdecode();

Scheduler runner;

Task t1(0, TASK_FOREVER, &lcddim, &runner, true);
Task t2(0, TASK_FOREVER, &irdecode, &runner, true);

OneWire  ds(D7);  // on pin 10 (a 4.7K resistor is necessary)

#define DHTPIN            D6         // Pin which is connected to the DHT sensor.

// Uncomment the type of sensor in use:
//#define DHTTYPE           DHT11     // DHT 11 
#define DHTTYPE           DHT22     // DHT 22 (AM2302)
//#define DHTTYPE           DHT21     // DHT 21 (AM2301)

// See guide for details on sensor wiring and usage:
//   https://learn.adafruit.com/dht/overview

DHT dht(DHTPIN, DHTTYPE);

//
//*********************************************************/
//* big numbers
//*********************************************************/
// based on http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1265696343

// the 8 arrays that form each segment of the custom numbers
byte C0[8] ={B11111,B11111,B11111,B00000,B00000,B00000,B00000,B00000};
byte C1[8] ={B00000,B00000,B00000,B00000,B00000,B11111,B11111,B11111};
byte C2[8] ={B00000,B00000,B00000,B00000,B00000,B11100,B11100,B11100};
byte C3[8] ={B11111,B11111,B11111,B00000,B00000,B00000,B11111,B11111};
byte C4[8] ={B11100,B11100,B11100,B00000,B00000,B00000,B00000,B00000};
byte C5[8] ={B00011,B00011,B00011,B00011,B00011,B11111,B11111,B11111};
byte C6[8] ={B11100,B11100,B11100,B11100,B11100,B11100,B11100,B11100};
byte C7[8] ={B11100,B11100,B11100,B00000,B00000,B00000,B11100,B11100};

/**********************************************************/

void initDebug() {
  if(HTTPDEBUG == 1 or IRDEBUG == 1 or OTADEBUG == 1 or FBDEBUG == 1 or LCDINFO == 1)
  {
    DEBUG=1;
  }
}

void initSerial() {
  if(DEBUG == 1){
    Serial.begin(SERIAL_BAUD_RATE);
    while (!Serial) {
      ;  // wait for serial port to initialize
    }
    Serial.println("Serial ready");
  }
}

void initWiFi() {
  if(DEBUG == 1){Serial.println(" \r\nWLAN INIT \r\n") ;}
  WiFi.begin(ssid, pass);
  LCDwrite(1, 5, "WLAN INIT");
  lcd.setCursor(4,2);
  while (WiFi.status() != WL_CONNECTED) {
    lcd.print(".");
    if (timeOutSec == 50) { ESP.reset(); }
    if(DEBUG == 1){Serial.print(".");}
    delay(500);
    timeOutSec++;
  }
  if(DEBUG == 1)
  {
    Serial.println("");
    Serial.print("Verbunden mit ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } 
  lcd.clear();
  if(LCDINFO == 1)
  {
    LCDwrite(0, 0, "Verbunden mit");
    LCDwrite(1, 0, ssid);
    LCDwrite(2, 0, "IP address:");
    lcd.setCursor(0,3);
    lcd.print(WiFi.localIP());
    delay(2000);
    lcd.clear();
  }
}

void initLCD() {
  pinMode(D8, OUTPUT);
  digitalWrite(LCDBackgroundLED, 1);
  lcd.begin(20,4);
  lcd.init();
  lcd.backlight();
}

void inithttpserver() {
  server.on("/",handleRoot) ;
  //server.on("/DatumZeit/", handleDatumZeit);
  //server.on("/DatumZeit/Zeit/", handleZeit);
  //server.on("/DatumZeit/Zeit/Stunde/", handleZeitStunde);
  //server.on("/DatumZeit/Zeit/Minute/", handleZeitMinute);
  //server.on("/DatumZeit/Datum/", handleDatum);
  //server.on("/DatumZeit/Datum/Tag/", handleDatumTag);
  //server.on("/DatumZeit/Datum/Monat/", handleDatumMonat);
  //server.on("/DatumZeit/Datum/Jahr/", handleDatumJahr);
  //server.on("/DatumZeit/Datum/Wochentag/", handleDatumWochentag);
  server.on("/LCDDim/", handleLCDDim);
  server.on("/Temperatur/", handleTemperatur);
  server.onNotFound(handleNotFound);
  server.begin();
  if(HTTPDEBUG == 1){Serial.println("HTTP Server wurde gestartet!");}
  if(LCDINFO == 1)
  {
    LCDwrite(1, 4, "HTTP Server");
    LCDwrite(2, 5, "gestartet");
    delay(2000);
    lcd.clear();
  }
}

void initirserver() {
  irrecv.enableIRIn(); // Start the receiver
  if(IRDEBUG == 1){Serial.println("IR Empfang Eingeschaltet");}
  if(LCDINFO == 1)
  {
    LCDwrite(1, 4, "IR Empfang");
    LCDwrite(2, 5, "gestartet");
    delay(2000);
    lcd.clear();
  }
}

void initArduinoOTA() {
  /////////////////////////////////////////////////////////
  /// OTA Funktion
  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  if(OTADEBUG == 1){Serial.print("ESP-ChipID ist : "); Serial.println(ESP.getChipId());}
  if (ESP.getChipId() == 14118255) { if(OTADEBUG == 1){Serial.print("Registriert als : ");Serial.println("ESP-Clock-SZ");} ArduinoOTA.setHostname("ESP-CLOCK-SZ"); ESP_OTA_HOSTNAME = "ESP-CLOCK-SZ";}
  if (ESP.getChipId() == 1463276) { if(OTADEBUG == 1){Serial.print("Registriert als : ");Serial.println("ESP-Clock-WZ");} ArduinoOTA.setHostname("ESP-CLOCK-WZ"); ESP_OTA_HOSTNAME = "ESP-CLOCK-WZ";}
  //ArduinoOTA.setHostname(ESP_OTA_HOSTNAME);

  // No authentication by default
  if(ESP_OTA_PASSWORD_ENABLE == 1)
  {
    if(OTADEBUG == 1){Serial.println("OTA Passwort Ein");}
    ArduinoOTA.setPassword(ESP_OTA_PASSWORD);
  }
  ArduinoOTA.onStart([]() {
    otaflash = 1;
    runner.disableAll();
    analogWrite(LCDBackgroundLED, 0);
    lcd.clear();
    digitalWrite(LCDBackgroundLED, 1);
    clientips.stop();
    Fritzbox.stop();
    server.stop();
    LCDwrite(1, 0, "ESP OverTheAir FLASH");
    if(OTADEBUG == 1){Serial.println("OTA Start");}
  });
  ArduinoOTA.onEnd([]() {
    lcd.clear();
    if(OTADEBUG == 1){Serial.println("\n OTA End");}
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    if(OTADEBUG == 1){Serial.printf("Progress: %u%%\r", (progress / (total / 100)));}
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR)  if(OTADEBUG == 1){Serial.println("Auth Failed");}
    else if (error == OTA_BEGIN_ERROR) if(OTADEBUG == 1){Serial.println("Begin Failed");}
    else if (error == OTA_CONNECT_ERROR) if(OTADEBUG == 1){Serial.println("Connect Failed");}
    else if (error == OTA_RECEIVE_ERROR) if(OTADEBUG == 1){Serial.println("Receive Failed");}
    else if (error == OTA_END_ERROR) lcd.clear();runner.enableAll(); if(OTADEBUG == 1){Serial.println("End Failed");}
  });
  ArduinoOTA.begin();
  if(OTADEBUG == 1){Serial.println("OverTheAir Server Eingeschaltet");}
  if(LCDINFO == 1)
  {
    LCDwrite(0, 1, "OverTheAir Server");
    LCDwrite(1, 5, "gestartet");
    LCDwrite(3, 4, ESP_OTA_HOSTNAME);
    delay(2000);
    lcd.clear();
  }
}

void initfbconnect() {
  if(LCDINFO == 1)
  {
    LCDwrite(1, 2, "Fritzbox Connect");
    lcd.setCursor(3,2);
  }
  if(FBDEBUG == 1){Serial.print("Fritzbox Connect : ") ;}
  Fritzbox.connect(FRITZBOX_HOSTNAME,FRITZBOX_PORT);
  if(Fritzbox.status() == 4)
  {
    if(FBDEBUG == 1){Serial.println("Verbunden") ;}
    if(LCDINFO == 1){LCDwrite(2, 5, "Verbunden");}
  }
  else
  {
    if(FBDEBUG == 1){Serial.println("nicht Verbunden") ;}
    if(LCDINFO == 1){LCDwrite(2, 2, "nicht Verbunden");}
  }
  if(LCDINFO == 1)
  {
    delay(2000);
    lcd.clear();
  }
}

void initLCDBegruesungstext() {
  if(DEBUG == 1){Serial.println("LCD Begruesungstext");}
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("IP-Symcon IR Gateway");
  lcd.setCursor(6,1);
  lcd.print("ESP8266");
  lcd.setCursor(6,2);
  lcd.print("nodeMCU");
  lcd.setCursor(4,3);
  lcd.print("by DJ Terror");
  delay(2000);
  lcd.createChar(0,C0);
  lcd.createChar(1,C1);
  lcd.createChar(2,C2);
  lcd.createChar(3,C3);
  lcd.createChar(4,C4);
  lcd.createChar(5,C5);
  lcd.createChar(6,C6);
  lcd.createChar(7,C7);
  lcd.clear();
}

void resetEthernet() {
  //client.stop();
  //delay(1000);
  //Ethernet.begin(mac);
  //delay(1000);
}

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

void FBCheck() {
  /// Fritzbox Status überprüfen und wenn nicht neu verbinden
  if((millis() - connectioncheck) > CHECKCONNECTION and Fritzbox.status() != 4 and otaflash == 0)
  {
    if(FBDEBUG == 1){Serial.println("Fritzbox neu Verbinden") ;}
    Fritzbox.connect(FRITZBOX_HOSTNAME,FRITZBOX_PORT);
  }

  // Check connection (refresh)
  if((millis() - connectioncheck) > CHECKCONNECTION and otaflash == 0)
  {
    if(FBDEBUG == 1){
      //Serial.println(F("Checking connection..."));
    }
    connectioncheck = millis();
    // Send dummy data to "refresh" connection state
    Fritzbox.write("x");
  }
  int size;

  if((size = Fritzbox.available()) > 0)
  { 
    uint8_t* msg = (uint8_t*)malloc(size);
    size = Fritzbox.read(msg,size);
    msg[size-1] = '\0';

    if(DEBUG == 1)
    {
      Serial.print(F("->Msg: "));
      Serial.println((char*)msg);
    }
    uint8_t* copymsgforsplit = (uint8_t*)malloc(size);
    memcpy(copymsgforsplit, msg, size);

    // Analyze incoming msg
    int i = 0;
    String DatumZeit,Type,ConnectionID;
    char *pch, *ptr;
    char type[11];
    pch = strtok_r ((char*)copymsgforsplit,";", &ptr);
          
    while (pch != NULL)
    {

      if(FBDEBUG == 1)
      {
        Serial.print(F("    ->Splitted part "));
        Serial.print(i);
        Serial.print(F(": "));
        Serial.println(pch);
      }
      switch(i)
      {
      case 0:           // Date and Time
        DatumZeit = pch;
      break;
      case 1:           // TYPE
        strcpy(type, pch);
      break;
      case 2:           // ConnectionID
      break;
      case 3:           // Anrufende Nummer     
        if(strcmp(type, "RING")  == 0)
        {
          callin = 1;
          LCDwrite(2, 0, "                    ");
          LCDwrite(3, 0, "                    ");
        
          if(strstr((char*)msg, ";;")) // Unknown caller?
          {
            LCDwrite(2, 1, "Eingehender Anruf");
            LCDwrite(3, 1, "Unbekanter Anrufer");
            lastcallnumber = "Unbekanter Anrufer";
          }
          else
          {
            LCDwrite(2, 1, "Eingehender Anruf");
            LCDwrite(3, 4, pch);
            lastcallnumber = pch;
          }
          callintime = millis();
        }
      break;
      case 4:           // Eigene Nummer
        if(strcmp(type, "RING")  == 0)
        {
          lastcalltime = DatumZeit;
          lastcallinnumber = pch;
        }
      break;
      case 5:
      break;
      default: 
      break;
      }
       i++;
      pch = strtok_r (NULL, ";", &ptr); // Split next part
    }
    free(msg);
    free(copymsgforsplit);
  }
  if(millis() - callintime > callintimeout and callin == 1) 
  {
    callin = 0;
    LCDwrite(2, 0, "                    ");
    LCDwrite(3, 0, "                    ");

  }
}

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

void irdecode() {
  if (irrecv.decode(&results)) {
    if(IRDEBUG == 1){Serial.println("");}
    if(IRDEBUG == 1){Serial.println("IR Empfang");}
    if(IRDEBUG == 1){Serial.print("Code Hex : ");}
    if(IRDEBUG == 1){Serial.print(results.value, HEX);}
    if(IRDEBUG == 1){Serial.print("    Code Dec: ");}
    if(IRDEBUG == 1){Serial.println(results.value, DEC);}
  switch(results.value, DEC)
  {
  default: 
    ProgrammUnknow();
    break;
  }
    irrecv.resume(); // Receive the next value
  }
 //delay(500); 
}

void ProgrammUnknow() {
  //clientips.connect(ips, 10002);
    clientips.print(results.value, DEC); 
//    clientips.println("\r\n"); 
  //clientips.stop();
  delay(1500);
}

void LCDRefresh() {
  LCDPrintTime();
  LCDPrintTemp();
}


void LCDwrite(int zeile, int spalte, String Text) {
  lcd.setCursor(spalte,zeile);
  lcd.print(Text);
}

void LCDPrintTemp() {
  if(second(local) >= 0 and second(local) <= 4 and LCDTempInnen == 1 and callin == 0 or second(local) >= 10 and second(local) <= 14 and LCDTempInnen == 1 and callin == 0 or second(local) >= 20 and second(local) <= 24 and LCDTempInnen == 1 and callin == 0 or second(local) >= 30 and second(local) <= 34 and LCDTempInnen == 1 and callin == 0 or second(local) >= 40 and second(local) <= 44 and LCDTempInnen == 1 and callin == 0 or second(local) >= 50 and second(local) <= 54 and LCDTempInnen == 1 and callin == 0)
  {
    lcd.setCursor(0,2);
    lcd.print("                    ");
    lcd.setCursor(0,3);
    lcd.print("                    ");
    lcd.setCursor(0,2);
    lcd.print("WZ ");
    lcd.print(ipsTempWZ);
    lcd.setCursor(10,2);
    lcd.print("SZ ");
    lcd.print(ipsTempSZ);
    lcd.setCursor(0,3);
    lcd.print("CZ ");
    lcd.print(ipsTempCZ);
    lcd.setCursor(10,3);
    lcd.print("KZ ");
    lcd.print(ipsTempKZ);
    LCDTempInnen = 0;
    LCDTempAussen = 1;
  }

  if(second(local) >= 5 and second(local) <= 9 and LCDTempAussen == 1 and callin == 0 or second(local) >= 15 and second(local) <= 19 and LCDTempAussen == 1 and callin == 0 or second(local) >= 25 and second(local) <= 29 and LCDTempAussen == 1 and callin == 0 or second(local) >= 35 and second(local) <= 39 and LCDTempAussen == 1 and callin == 0 or second(local) >= 45 and second(local) <= 49 and LCDTempAussen == 1 and callin == 0 or second(local) >= 55 and second(local) <= 59 and LCDTempAussen == 1 and callin == 0)
  {
    lcd.setCursor(0,2);
    lcd.print("                    ");
    lcd.setCursor(0,3);
    lcd.print("                    ");
    lcd.setCursor(0,2);
    lcd.print("Aussen ");
    lcd.print(ipsTempA);
    LCDTempInnen = 1;
    LCDTempAussen = 0;
  }
}

void printBigDigits(byte digits, int x){
  // utility function for digital clock display: prints preceding colon and leading 0
  bignum(digits/10, x);
  bignum(digits%10, x+3);
}

void LCDPrintTime(){
  lcd.setCursor(6,0);
  lcd.print("-");
  lcd.setCursor(6,1);
  lcd.print("-");
  printBigDigits(hour(local), 0);
  printBigDigits(minute(local), 7);
  LCDWochenTag(weekday(local));
  LCDDatum(day(local), month(local), year(local));
}

void LCDDatum(int Tag, int Monat, int Jahr) {
  lcd.setCursor(15,1);
  if(Tag < 10)
    lcd.print("0");
  lcd.print(Tag,DEC);
  lcd.print(".");
  if(Monat < 10)
    lcd.print("0");
  lcd.print(Monat,DEC);
  lcd.setCursor(16,0);
  lcd.print(Jahr,DEC);
}

void LCDWochenTag(int digit) {
  switch (digit)               // depending on which button was pushed, we perform an action  
  { 
  case 1:lcd.setCursor(13,0);lcd.print("So");break;
  case 2:lcd.setCursor(13,0);lcd.print("Mo");break;
  case 3:lcd.setCursor(13,0);lcd.print("Di");break;
  case 4:lcd.setCursor(13,0);lcd.print("Mi");break;
  case 5:lcd.setCursor(13,0);lcd.print("Do");break;
  case 6:lcd.setCursor(13,0);lcd.print("Fr");break;
  case 7:lcd.setCursor(13,0);lcd.print("Sa");break;
  }
}

void bignum(int digit, int x) {
  switch (digit)               // depending on which button was pushed, we perform an action  
  { 
  case 0:{custom0(x);break;}
  case 1:{custom1(x);break;}
  case 2:{custom2(x);break;}
  case 3:{custom3(x);break;}
  case 4:{custom4(x);break;}
  case 5:{custom5(x);break;}
  case 6:{custom6(x);break;}
  case 7:{custom7(x);break;}
  case 8:{custom8(x);break;}
  case 9:{custom9(x);break;}
  }
}

void custom1(int x){lcd.setCursor(x,0);lcd.write(0);lcd.write(255);lcd.write(32);lcd.setCursor(x, 1);lcd.write(1);lcd.write(255);lcd.write(2);}
void custom2(int x){lcd.setCursor(x,0);lcd.write(0);lcd.write(3);lcd.write(6);lcd.setCursor(x, 1);lcd.write(255);lcd.write(1);lcd.write(2);}
void custom3(int x){lcd.setCursor(x,0);lcd.write(3);lcd.write(3);lcd.write(6);lcd.setCursor(x, 1);lcd.write(1);lcd.write(5);lcd.write(6);}
void custom4(int x){lcd.setCursor(x,0);lcd.write(255);lcd.write(1);lcd.write(2);lcd.setCursor(x, 1);lcd.write(32);lcd.write(255);lcd.write(32);}
void custom5(int x){lcd.setCursor(x,0);lcd.write(255);lcd.write(3);lcd.write(7);lcd.setCursor(x, 1);lcd.write(1);lcd.write(5);lcd.write(6);}
void custom6(int x){lcd.setCursor(x,0);lcd.write(255);lcd.write(3);lcd.write(7);lcd.setCursor(x, 1);lcd.write(255);lcd.write(5);lcd.write(6);}
void custom7(int x){lcd.setCursor(x,0);lcd.write(0);lcd.write(0);lcd.write(6);lcd.setCursor(x, 1);lcd.write(32);lcd.write(255);lcd.write(32);}
void custom8(int x){lcd.setCursor(x,0);lcd.write(255);lcd.write(3);lcd.write(6);lcd.setCursor(x, 1);lcd.write(255);lcd.write(5);lcd.write(6);}
void custom9(int x){lcd.setCursor(x,0);lcd.write(255);lcd.write(3);lcd.write(6);lcd.setCursor(x, 1);lcd.write(1);lcd.write(5);lcd.write(6);}
void custom0(int x){lcd.setCursor(x,0);lcd.write(255);lcd.write(0);lcd.write(6);lcd.setCursor(x, 1);lcd.write(255);lcd.write(5);lcd.write(6);}

void lcdon() {
  lcd.backlight();
}

void lcddim() {
  analogWrite(LCDBackgroundLED, LCDBackgroundLEDDim);
}

void lcdoff() {
  lcd.noBacklight();
}

void lcdstartdim() {

}

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

  IPSymconCheck();
  
}

