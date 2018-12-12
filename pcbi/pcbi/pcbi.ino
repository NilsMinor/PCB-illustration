
#include "pcbi_config.h"

#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Adafruit_NeoPixel.h>
#include <TimeLib.h>
#include <WidgetRTC.h>


// LED strip related configs
#define PIXEL_PIN   12     // Wemos D6 
#define PIXEL_COUNT 200    // Pixels Count

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRBW + NEO_KHZ800);



// global variables
float brightness = 0.5;
bool on_off = false;


void all_on (void);
void all_off (void);
void all_color (uint8_t r, uint8_t g, uint8_t b);
void all_wwhite (uint8_t w);
void all_leds (uint8_t r, uint8_t g, uint8_t b, uint8_t w);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Blynk.begin(auth, wlan_name, wlan_pw);

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  Serial.print("Connecting");
  while (Blynk.connect() == false) { 
    
    Serial.print(".");
    delay(100);
  }
  
  Serial.println();
  Blynk.syncAll();
}

BLYNK_WRITE(V0) {
  on_off = (float)param.asInt();
}

BLYNK_WRITE(V2) {
  brightness = (float)param.asInt()/100;
}

void loop() {
  if (on_off) 
    all_leds (0,0,0,255);
  else 
    all_off ();
    
 
 
    delay(10);
  Blynk.run();

}

void all_off (void) {
   all_leds (0,0,0,0);
}




// set all leds with one command
void all_leds (uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
  for (int i=0;i!=PIXEL_COUNT;i++) {
      strip.setPixelColor(i, strip.Color(r*brightness, g*brightness, b*brightness,w*brightness));
  }
  strip.show();
}
