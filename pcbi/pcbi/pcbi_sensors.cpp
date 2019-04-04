#include "pcbi_sensors.h"


PCBISensors::PCBISensors (void) {
  dht = new DHT(DHTPIN, DHTTYPE);

  temperature = humidity = 0;
  dht->begin();
}

bool PCBISensors::update (void) {
   static unsigned long currentTime = 0;
   static unsigned long previousTime = 0;
   
   currentTime = millis();
   
   unsigned long deltaTime = currentTime - previousTime;
   Serial.print ("Elapsed Time: ");
    Serial.println(deltaTime);

   if ( deltaTime > DHT22_INTERVALL_S*1000 ) {
      previousTime = currentTime;
      Serial.println ("update dht22");
      return measureSensors();
   }
   
   return false;
}

bool PCBISensors::measureSensors (void)
{
  // As communication is a bit buggy try to get data more then single shot
  for (uint8_t i=0;i!=10;i++) {

    float h = dht->readHumidity();
    float t = dht->readTemperature();

    if (!isnan(h) && !isnan(t)){
      humidity = h;
      temperature = t;
      return true;
    }
  }
  return false;
}


float PCBISensors::getTemperature (void) {
  return temperature;
}
float PCBISensors::getHumidity (void) {
  return humidity;
}
