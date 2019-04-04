
/* pcbi-sensors provides access to dht temperture and humidity sensor
 *
 */

#ifndef _PCBI_SENSORS_H_
#define _PCBI_SENSORS_H_


#include "DHT.h"                // DHT22 temperature and humidity sensor


// DHT 22 setting
#define DHTPIN 13               // Communication Pin
#define DHTTYPE DHT22           // DHT 22  (AM2302), AM2321
#define DHT22_INTERVALL_S    30 // Update intervall in seconds

class PCBISensors {
  public:
    PCBISensors ( );            // constructor
    float getTemperature (void); // return last temperature measurement
    float getHumidity (void);    // return last humidity measurement
    bool update (void);         // update data, must be called in the main loop

  private:
    DHT *dht;
    float temperature;
    float humidity;
    bool measureSensors (void);
};

#endif
