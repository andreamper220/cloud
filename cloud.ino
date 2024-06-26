#include <Adafruit_BME280.h>
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266HTTPClient.h>
#include <SoftwareSerial.h>
#include <WiFiClient.h>
#include <Wire.h>

/**
 * OLED Settings
 */
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display = Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

/**
 * Wi-Fi Settings
 */
const char* ssid = "cyberpunk";
const char* password = "10011001";
const char* serverName = "http://narodmon.ru/json";

/**
 * Common settings
 */
const String mac = "E8DB84ED8FCA";
const float latitude = 52.888295;
const float longitude = 40.516813;
const int altitude = 291;
const String placeName = "center";
const String placeOwner = "junicode";

/**
 * Timers
 */
unsigned long sendDataMillis = 0;
unsigned long sendDataInterval = 300000;
unsigned long showDataMillis = 0;
unsigned long showDataInterval = 5000;

/**
 * Sensors
 */
// BME280
Adafruit_BME280 bme;
float temperature, humidity, pressure;
// PMS7003
SoftwareSerial serialPms7003(D5, D6);
int pm1, pm25, pm10;
// MQ-9
int co;

void setup() {
  // BME280
  bme.begin(0x76);
  // PMS7003
  serialPms7003.begin(9600);
  // OLED
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); 
  display.clearDisplay();
  display.setTextSize(1); 
  display.setTextColor(SSD1306_WHITE); 
  display.setCursor(30, 10);
  display.println("Loading...");
  display.display();
  delay(5000);
  display.clearDisplay();
  // Wi-Fi
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void loop() {
  if ((showDataMillis == 0) || ((millis() - showDataMillis) > showDataInterval)) {    
    // BME280
    temperature = bme.readTemperature();
    humidity = bme.readHumidity();
    pressure = bme.readPressure() * 0.0075;
    // PMS7003
    readPms7003();
    // MQ-9
    co = analogRead(A0) / 2;
    // OLED
    display.setCursor(0, 0);
    display.print("T:");
    display.print(temperature);
    display.println("C");
    display.print("H:");
    display.print(humidity);
    display.println("%");
    display.print("P:");
    display.print(pressure);
    display.println("mmHg");
    display.print("PM2.5:");
    display.print(pm25);
    display.println("ppm");
    display.print("CO:");
    display.print(co);
    display.println("ppm");
    display.println("");
    display.println("v 1.0.0");
    display.display();
    display.clearDisplay();

    showDataMillis = millis();
  }

  if ((sendDataMillis == 0) || ((millis() - sendDataMillis) > sendDataInterval)) {
    // Wi-Fi
    if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;
      
      http.begin(client, serverName);
      http.addHeader("Content-Type", "application/json");

      String json = 
        String("{\"devices\":[{\"mac\":\"" 
        + mac
        + String("\",\"name\":\"")
        + placeName 
        + "\",\"owner\":\""
        + placeOwner
        + "\",\"lat\":"
        + String(latitude)
        + ",\"lon\":"
        + String(longitude)
        + ",\"alt\":"
        + altitude 
        + ",\"sensors\":[{\"id\":\"PM1\",\"name\":\"PM1_1\",\"value\":")
        + pm1
        + String(".00,\"unit\":\"ug\/m3\"},{\"id\":\"PM2_5\",\"name\":\"PM25_1\",\"value\":")
        + pm25
        + String(".00,\"unit\":\"ug\/m3\"},{\"id\":\"PM10\",\"name\":\"PM10_1\",\"value\":")
        + pm10
        + String(".00,\"unit\":\"ug\/m3\"},{\"id\":\"CO1\",\"name\":\"CO_1\",\"value\":")
        + co
        + String(".00,\"unit\":\"ppm\"},{\"id\":\"TEMP1\",\"name\":\"TEMP_1\",\"value\":")
        + String(temperature)
        + String(",\"unit\":\"C\"},{\"id\":\"HUM1\",\"name\":\"HUM_1\",\"value\":")
        + String(humidity)
        + String(",\"unit\":\"%\"},{\"id\":\"PRES1\",\"name\":\"PRES_1\",\"value\":")
        + String(pressure)
        + String(",\"unit\":\"mmHg\"}]}]}");

      http.POST(json);

      http.end();
    }
    
    sendDataMillis = millis();
  }
}

void readPms7003() {
  int checksum = 0;
  unsigned char pms[32] = {0,};

  while( serialPms7003.available() && 
      serialPms7003.read() != 0x42 &&
      serialPms7003.peek() != 0x4D ) {
  }    
  if( serialPms7003.available() >= 31 ){
    pms[0] = 0x42;
    checksum += pms[0];
    for(int j=1; j<32 ; j++){
      pms[j] = serialPms7003.read();
      if(j < 30)
        checksum += pms[j];
    }
    serialPms7003.flush();
    if( pms[30] != (unsigned char)(checksum>>8) 
      || pms[31]!= (unsigned char)(checksum) ){
      Serial.println("Checksum error");
      return;
    }
    if( pms[0]!=0x42 || pms[1]!=0x4d ) {
      Serial.println("Packet error");
      return;
    }

    pm1  = makeWord(pms[10],pms[11]);
    pm25 = makeWord(pms[12],pms[13]);
    pm10 = makeWord(pms[14],pms[15]);
  }   
}
