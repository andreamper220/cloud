#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <SoftwareSerial.h>

#define DHTPIN 2
#define BATTERYPIN 14
#define DHTTYPE DHT11
#define SCREEN_WIDTH 128  
#define SCREEN_HEIGHT 32 

DHT dht(DHTPIN, DHTTYPE);
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

float hum;
float temp;
float voltage;

unsigned long timer;

const char* ssid = "cyberpunk";
const char* password = "10011001";

const char* serverName = "http://narodmon.ru/json";

unsigned long lastTime = 0;
unsigned long timerDelay = 360000;

SoftwareSerial _serial(D5, D6);
int _pm1, _pm25, _pm10;

void setup(){
  dht.begin();

  oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);                                                                                                                                              
  oled.clearDisplay(); 
  delay(2000);

  oled.setTextSize(1);          
  oled.setTextColor(WHITE);     
  oled.setCursor(0, 10);        
  oled.println("loading..."); 
  oled.display(); 
  delay(1000);
  Serial.begin(115200);
  _serial.begin(9600);

//  WiFi.begin(ssid, password);
//  while(WiFi.status() != WL_CONNECTED) {
//    delay(500);  
//  }
}

void loop() {
  if (millis() - timer > 1000) {
    timer = millis();
    voltage = (5.0 * analogRead(BATTERYPIN)) / 1023.0;

    oled.clearDisplay();
    oled.setCursor(0, 10);   
    hum = dht.readHumidity();
    temp = dht.readTemperature();
    
    oled.setCursor(95,7);
    oled.print("H: ");
    oled.print(hum);
    oled.println(" %");

    oled.setCursor(95,16);
    oled.print("T: ");
    oled.print(temp);
    oled.println(" C");

    oled.drawRect(5, 0, 15, 5, WHITE);
    oled.fillRect(100, 0, round((voltage * 15.0) / 5.0), 5, WHITE);
    oled.setCursor(10, 23);
    oled.print("v0.1");
    oled.display(); 
  }
  
  if ((millis() - lastTime) > timerDelay) {
    readSensor();
    
//    if(WiFi.status()== WL_CONNECTED){
//      WiFiClient client;
//      HTTPClient http;
//      
//      http.begin(client, serverName);
//      
//      http.addHeader("Content-Type", "application/json");
//      
//      String json = 
//        String("{\"devices\":[{\"mac\":\"E8DB84ED8FCF\",\"name\":\"mich1\",\"owner\":\"blini000\",\"lat\":52.888295,\"lon\":40.516813,\"alt\":152,\"sensors\":[{\"id\":\"PM1\",\"name\":\"PM2.5_1\",\"value\":") 
//        + _pm25
//        + String(".00,\"unit\":\"\u00B5g\/m3\"}]}]}");
//      int httpResponseCode = http.POST(json);
//      
//      http.end();
//    }
    
    lastTime = millis();
  }
}


void readSensor() {
  int checksum = 0;
  unsigned char pms[32] = {0,};

  Serial.println("Start");
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