#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

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
}

void loop(){
  if (millis() - timer > 1000){
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
} 
