
#include "pcbi_config.h"

/* 
 * Blynk connections :
 * 
 *  Pin    description         value
 *  V0     on-off switch       0 - 1
 *  V1     brightness          0 - 100
 *  V2     effect selection    0 - ?
 *  V3     effect speed        0 - 9
 *  V10    temeprature value   XX.X °C
 *  V11    humidity value      XX.X %
 *  
 *  
 */

#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Adafruit_NeoPixel.h>
#include <TimeLib.h>
#include <WidgetRTC.h>

#include "Seeed_CY8C401XX.h"    // Grove touch sensor
#include "DHT.h"                // DHT22 temperature and humidity sensor

// DHT 22 setting
#define DHTPIN 13                // what digital pin we're connected to
#define DHTTYPE DHT22           // DHT 22  (AM2302), AM2321


// LED strip related configs
#define PIXEL_PIN   12          // Wemos D6 
#define PIXEL_COUNT 200         // Pixels Count

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRBW + NEO_KHZ800);

CY8C touch;
DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;


// global variables
float brightness = 0.5;
bool on_off = false;


void all_on (void);
void all_off (void);
void all_color (uint8_t r, uint8_t g, uint8_t b);
void all_wwhite (uint8_t w);
void single_led (uint8_t pin, uint8_t r, uint8_t g, uint8_t b, uint8_t w);
void all_leds (uint8_t r, uint8_t g, uint8_t b, uint8_t w);
void sendTemperatureHumidity(void);
void handle_touch (void);
void connectWLANIndicator (void);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  touch.init();
  Blynk.begin(auth, wlan_name, wlan_pw);

  strip.begin();
  dht.begin();

  connectWLANIndicator ();

  
  timer.setInterval(1000L, sendTemperatureHumidity); // Setup a function to be called every second

  Blynk.syncAll();  
}

//----------------------------------------------------------------------------------------------------


BLYNK_WRITE(V0) {
  on_off = (float)param.asInt();
}

BLYNK_WRITE(V1) {
  brightness = (float)param.asInt()/100;
}

//----------------------------------------------------------------------------------------------------

void loop() {
  
  if (on_off) 
    all_leds (0,0,0,255);
  else 
    all_off ();
    
    //handle_touch ();

    //delay(10);
    Blynk.run();
    timer.run();
}

void all_off (void) {
   all_leds (0,0,0,0);
}

// Connect to WLAN, wait while not conected
void connectWLANIndicator (void) {
  
  all_off ();

  // wait for wlan connection
  Serial.print("Connecting");
  single_led (10,0,0,200,0);
  while (Blynk.connect() == false) { 
    Serial.print(".");
    delay(100); 
  } 
  Serial.println();

  Serial.print("connected");
  all_off ();
}


// handle touch events
// needed to change variable types (eg u8 to uint8_t) for esp8266
void handle_touch (void) {
    uint8_t value=0;
    
    touch.get_touch_button_value(&value);
    if(value&0x01) {
        Serial.println("button 1 is pressed");

        // toggle on of state
        if (on_off) on_off = false;
        else        on_off = true;
    }
    if(value&0x2){
        // use for event setting
        Serial.println("button 1 is pressed");
    }

    touch.get_touch_slider_value(&value);
    brightness = (double) value / 100.0;      
    Serial.print("slider value is");
    Serial.println(value,HEX);
    Serial.println(" ");

    // notify blynk
}
void sendTemperatureHumidity(void) {
  
  /*float h = dht.readHumidity();
  float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  
  Blynk.virtualWrite(V5, t);
  Blynk.virtualWrite(V6, h);*/

  Serial.println("Test DHT sensor!");

  Blynk.virtualWrite(V5, 1.23);
  Blynk.virtualWrite(V6, 45.67);
}


// set all leds with one command
void all_leds (uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
  for (int i=0;i!=PIXEL_COUNT;i++) {
      strip.setPixelColor(i, strip.Color(r*brightness, g*brightness, b*brightness,w*brightness));
  }
  strip.show();
}

void single_led (uint8_t pin, uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
  strip.setPixelColor(pin, strip.Color(r*brightness, g*brightness, b*brightness,w*brightness));
  strip.show();
}
