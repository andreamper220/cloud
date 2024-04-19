#include <DHT.h>
DHT dht(2, DHT11);
void setup() {
 dht.begin();
 Serial.begin(115200);

}

void loop() {
 float h = dht.readHumidity();
 float t = dht.readTemperature();
 delay (1000);
 Serial.println (h);
 Serial.println (t);

}
