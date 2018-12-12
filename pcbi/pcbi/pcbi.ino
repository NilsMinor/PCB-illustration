
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



void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Blynk.begin(auth, wlan_name, wlan_pw);

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  Serial.print("Connecting");
  while (Blynk.connect() == false) {// Wait until connected
    
    Serial.print(".");
    delay(100);
  }
  
  Serial.println();
  Blynk.syncAll();
  //lcd.clear();
  //lcd.print(0, 1, "Welcome Nils!");

}

void loop() {
  // put your main code here, to run repeatedly:

}
