#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <DNSServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <FastLED.h>

#define USESERIAL

#define NUMSERVOS 2

#include "Limits.h"
#include "Config.h"
#include "FirmwareReset.h"
#include "Servos.h"
#include "AdminPage.h"
#include "SetupPage.h"

#define DNS_NAME  SSID_DEFAULT
#define DNS_URL   "http://" DNS_NAME ".local/"

#define HTTP_PORT 80
#define DNS_PORT  53
#define WS_PORT   2205

#define JOYMSG    0
#define LEDMSG    1
#define ADJMSG    2
#define MAXTIMEOUT 10
#define MSFREQ    5


ESP8266WebServer webServer(HTTP_PORT);
DNSServer dnsServer;
WebSocketsServer webSocket = WebSocketsServer(WS_PORT);

byte timeout = MAXTIMEOUT;
bool oldConnected = true;
bool connected = false;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);  Serial.printf("\nWiFi Strandbeest\n");
    
  InitConfig();
  
  //Start the wifi with the required username and password
  WiFi.mode(WIFI_AP);

  LoadConfig();
  //ResetConfig();
  PrintConfig();

  //Check to see if the flag is still set from the previous boot
  if(checkResetFlag()) {
    //Do the firmware reset here
    Serial.printf("Reset Firmware\n");

    //Set the ssid to default value and turn off the password
    WiFi.softAP("APConfig", "", 1, false, 1);
  } 
  else {
    WiFi.softAP(config.ssid, config.pass, 1, false, 1);
  }

  //Start the file system
  SPIFFS.begin();  

  //Serve a very basic page on the root url
  webServer.serveStatic("/", SPIFFS, "/index.html");
  webServer.serveStatic("/servo", SPIFFS, "/servo.html");

  // Set up the admin page
  webServer.on("/admin", std::bind(serveAdmin, &webServer));

  // Set up the setup page
  webServer.on("/setup", std::bind(serveSetup, &webServer));

  // Redirect all unknown traffic back to the homepage
  webServer.onNotFound([](){
    webServer.sendHeader("Location", DNS_URL);
    webServer.send(303);
  });

  webServer.begin();

  webSocket.onEvent(webSocketEvent);
  webSocket.begin();

  MDNS.begin(DNS_NAME);
  MDNS.addService("http", "tcp", HTTP_PORT);
  dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());

  InitServos();

  while(!booted) yield();
}




void loop() {
  dnsServer.processNextRequest();
  webServer.handleClient();  
  webSocket.loop();

  EVERY_N_MILLISECONDS(MSFREQ) { 
    for(byte i=0; i< NUMSERVOS; i++) 
      servos[i].update();
  }

  // Keep a counter to check for communication timeouts
  EVERY_N_MILLISECONDS(500) { 
    connected = (timeout < MAXTIMEOUT);
    digitalWrite(LED_BUILTIN, !connected);

    // Update items when the connected state changes
    if(oldConnected != connected) {
      for(byte i=0; i< NUMSERVOS; i++) 
        servos[i].connect(connected);  
    }
    
    if(connected) timeout += 1;    
    oldConnected = connected;
  }
}

void webSocketEvent(unsigned char clientNo, WStype_t type, unsigned char *data, unsigned int len)
{
  uint8_t msgtype, id;
  int8_t x, y;
  uint16_t ms;
  String message = "Packet: ";
  int16_t leftWheel, rightWheel;
  
  StaticJsonBuffer<200> jsonBuffer;
  
  switch(type)
  {
  case WStype_CONNECTED:
    break;

  case WStype_DISCONNECTED:
    break;
    
  case WStype_BIN: 
//    for(int i = 0; i < len; i += 1) 
//      message += String(data[i], HEX) + ",";
//    Serial.println(message);  
  
    msgtype = data[0];
    
    switch (msgtype) {
      case JOYMSG:
        x = data[1];
        y = data[2];

        leftWheel =  (y*7.0) + (x*7.0);
        rightWheel = (y*7.0) - (x*7.0);
       
        //Serial.printf("x:%d, y:%d, l:%d, r:%d\n", x, y, leftWheel, rightWheel);

        servos[LFT].targetValue =  -leftWheel;
        servos[RGT].targetValue =  -rightWheel;
        break;

      case ADJMSG:
        id = data[1];
        ms = (uint16_t)(data[2] << 8) + (uint16_t)data[3];
        //Serial.printf("id: %d, ms: %d\n", id, ms);
        servos[id].writeMicroseconds(ms);
        break;
    }

    timeout = 0;
    break;

  case WStype_TEXT:
    Serial.printf("<-%s\n", data);
    JsonObject& root = jsonBuffer.parseObject(data);
    
    if (root.success()) {
      const char* msgtype = root["type"];
 
      if(strcmp(msgtype, "getinfo") == 0) 
        sendServoData();
        
      if(strcmp(msgtype, "setinfo") == 0) {
        uint8_t id = root["id"];
        config.limits[id].min = servos[id].limits.min = root["min"];
        config.limits[id].zero = servos[id].limits.zero = root["zero"];
        config.limits[id].max = servos[id].limits.max = root["max"];

        EEPROM_writeAnything(0,  config.limits);    
        EEPROM.commit();
  
        Serial.printf("Updated Servo %d Settings\n", id);
      }
    }
    break;    
  }
}

void sendServoData() {
  StaticJsonBuffer<300> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  JsonArray& servosObj = root.createNestedArray("servos");

  String jsonString;
  
  root["numservos"] = NUMSERVOS;

  for(byte id = 0; id < NUMSERVOS; id++) {
    JsonObject& servoObj = servosObj.createNestedObject();
    servoObj["id"] = id;
    servoObj["min"] = config.limits[id].min;
    servoObj["zero"] = config.limits[id].zero;
    servoObj["max"] = config.limits[id].max;
  }

  root.printTo(jsonString);
  webSocket.sendTXT(0, jsonString.c_str(), jsonString.length());
  Serial.printf("->%s %d\n", jsonString.c_str(), jsonString.length());
}


