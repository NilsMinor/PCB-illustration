#include "pcbi_sesnors.h"


void PCBISensors::PCBISensors (void) {
  dht = new DHT(DHTPIN, DHTTYPE);

  temperature = humidity = 0;
}


float PCBISensors::getTemperature (void) {
  return temperature;
}
float PCBISensors::getHumidity (void) {
  return humidity;
}
