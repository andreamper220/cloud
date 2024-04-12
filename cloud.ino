
//#include <Wire.h>
//#include <Adafruit_GFX.h>
//#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <SoftwareSerial.h>
//#include <MHZ19.h>                                                                    
//#include <Adafruit_BME280.h>                            
//#include <Adafruit_Sensor.h>                            
 
//#define SEALEVELPRESSURE_HPA (1013.25) 
//#define DHTPIN 2
//#define BATTERYPIN 14
//#define DHTTYPE DHT11
//#define SCREEN_WIDTH 128  
//#define SCREEN_HEIGHT 32 
//#define BuzzerPin D8 
//#define GasSensorPin A0

//#define PIN_MQ9  A0
//#define PIN_MQ9_HEATER  8

//Adafruit_BME280 bme;

//MQ9 mq9(A0, PIN_MQ9_HEATER);
//MHZ19 myMHZ19;
//SoftwareSerial co2_serial(, D4);

//Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

//float hum;
//float temp;
//float voltage;
//float pres;

const char* ssid = "cyberpunk";
const char* password = "10011001";

const char* serverName = "http://narodmon.ru/json";

unsigned long lastTime = 0;
unsigned long timerDelay = 5000;

SoftwareSerial _serial(D5, D6);
int _pm1, _pm25, _pm10;
//int ppm_co2;
int gasValue;


void setup(){
  Serial.begin(115200);
  
//  bme.begin();
//  if (!bme.begin(0x76)) {                               // Проверка инициализации датчика
//    Serial.println("Could not find a valid BME280 sensor, check wiring!");
//  }

//  oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);                                                                                                                                              
//  oled.clearDisplay(); 
//  delay(2000);
//
//  oled.setTextSize(1);          
//  oled.setTextColor(WHITE);     
//  oled.setCursor(0, 10);        
//  oled.println("loading..."); 
//  oled.display(); 
//  delay(1000);
  _serial.begin(9600);

//  co2_serial.begin(9600);
//  myMHZ19.begin(co2_serial);
//  myMHZ19.autoCalibration();

//  WiFi.begin(ssid, password);
//  while(WiFi.status() != WL_CONNECTED) {
//    delay(500);  
//  }
}

void loop() {
  if ((millis() - lastTime) > timerDelay) {
    readPm25();
    Serial.printf("PM1.0:%d PM2.5:%d PM10.0:%d\n", _pm1, _pm25, _pm10);
//    ppm_co2 = myMHZ19.getCO2();
//    Serial.print("CO2 = ");
//    Serial.println(ppm_co2);
//    Serial.print("CO = ");
//    Serial.println(analogRead(A0));
//
//    hum = bme.readHumidity();
//    temp = bme.readTemperature();
//    pres = bme.readPressure();
//    Serial.print("Temperature = ");
//    Serial.println(temp);
//    Serial.print("Humidity = ");
//    Serial.println(hum);
//    Serial.print("Pressure = ");
//    Serial.println(pres);  
   
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
//    
    lastTime = millis();
  }
}

void readPm25() {
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
