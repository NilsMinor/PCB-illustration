

#ifndef _PCBI_SENSORS_H_
#define _PCBI_SENSORS_H_


#include "DHT.h"                // DHT22 temperature and humidity sensor


// DHT 22 setting
#define DHTPIN 13               // 
#define DHTTYPE DHT22           // DHT 22  (AM2302), AM2321

class PCBISensors {
  public:
    PCBISensors ( );
    void getTemperature (void);
    void getHumidity (void);  

  private:
    DHT *dht;
    float temperature;
    float humidity;
    void measureSensors (void);

};



#endif
