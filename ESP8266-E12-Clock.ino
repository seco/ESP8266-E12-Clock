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


