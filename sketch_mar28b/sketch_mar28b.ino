
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <SoftwareSerial.h>

const char* ssid = "cyberpunk";
const char* password = "10011001";

const char* serverName = "http://narodmon.ru/json";

unsigned long lastTime = 0;
unsigned long timerDelay = 300000;

SoftwareSerial _serial(D5, D6);
int _pm1, _pm25, _pm10;

void setup() {
  Serial.begin(115200);
  _serial.begin(9600);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if ((lastTime == 0) || ((millis() - lastTime) > timerDelay)) {
    readSensor();
    Serial.printf("PM1.0:%d PM2.5:%d PM10.0:%d\n", _pm1, _pm25, _pm10);

    if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;
      
      http.begin(client, serverName);
      
      http.addHeader("Content-Type", "application/json");

      String json = 
        String("{\"devices\":[{\"mac\":\"E8DB84ED8FCF\",\"name\":\"mich1\",\"owner\":\"blini000\",\"lat\":52.888295,\"lon\":40.516813,\"alt\":152,\"sensors\":[{\"id\":\"PM1\",\"name\":\"PM2.5_1\",\"value\":") 
        + _pm25 
        + String(".00,\"unit\":\"\u00B5g\/m3\"}]}]}");
      
      int httpResponseCode = http.POST(json);
      if (httpResponseCode > 0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
        
      http.end();
    } else {
      Serial.println("WiFi Disconnected");
    }
    
    lastTime = millis();
  }
}

void readSensor() {
  int checksum = 0;
  unsigned char pms[32] = {0,};

  while( _serial.available() && 
      _serial.read() != 0x42 &&
      _serial.peek() != 0x4D ) {
  }    
  if( _serial.available() >= 31 ){
    pms[0] = 0x42;
    checksum += pms[0];
    for(int j=1; j<32 ; j++){
      pms[j] = _serial.read();
      if(j < 30)
        checksum += pms[j];
    }
    _serial.flush();
    if( pms[30] != (unsigned char)(checksum>>8) 
      || pms[31]!= (unsigned char)(checksum) ){
      Serial.println("Checksum error");
      return;
    }
    if( pms[0]!=0x42 || pms[1]!=0x4d ) {
      Serial.println("Packet error");
      return;
    }
    _pm1  = makeWord(pms[10],pms[11]);
    _pm25 = makeWord(pms[12],pms[13]);
    _pm10 = makeWord(pms[14],pms[15]);
  }   
}
