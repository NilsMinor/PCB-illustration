
#include "pcbi_light.h"
#include <ESP8266WiFi.h>
//#include <BlynkSimpleEsp8266.h>

LightController::LightController ( ) {
  FastLED.addLeds<WS2812B, DATA_PIN, RGB>(ledsRGB, getRGBWsize(PIXEL_COUNT));
  FastLED.show();

  pinMode (LED_PWR_PIN, OUTPUT);
  
  red = green = blue = 0;

  brightness = 50;
  effect_speed = 5;
  selected_mode = TWINKLE_SNOW;

  setEffectSpeed (100);
  
  setMOSFET (true); 
}

void LightController::setMOSFET  ( bool state ) {
  if ( state ) {
    digitalWrite(LED_PWR_PIN, HIGH);
    Serial.println ("MOSFET an");
  }
  else {
    digitalWrite(LED_PWR_PIN, LOW);
    Serial.println ("MOSFET aus");
  }
}

uint8_t LightController::getBrightness (void) {
  return brightness;
}
void  LightController::setBrightness (uint8_t br) {

  
  if (br >= 0 && br <= 100) {
    brightness = br;
    FastLED.setBrightness( brightness * 255 / 100.0 );
    FastLED.show();
  }

  if (brightness <= 0) {
    setMOSFET (false); 
  }
  else {
    setMOSFET (true); 
  }

  Serial.print ("brightness : ");
  Serial.println( brightness );
}
void  LightController::upBrightness (void) {
  if (brightness <  100) {
    brightness += BR_STEP;
    setBrightness ( brightness );
  }
}
void  LightController::downBrightness (void) {
  if (brightness > 0 ) {
    brightness -= BR_STEP;
    setBrightness ( brightness );
  }
}
uint16_t LightController::getEffectSpeed (void) {
  return effect_speed;
}
void LightController::setEffectSpeed (uint16_t es) {
  effect_speed = es;
}

uint8_t LightController::getEffectMode   (void) {
  return selected_mode;
}
void LightController::setEffectMode   (uint8_t mo) {
  if (mo <= MAX_MODE)
    selected_mode = mo;
}
void LightController::nextEffectMode  (void) {
  selected_mode = ( selected_mode + 1 ) % MAX_MODE;
  Serial.println(selected_mode);
}
void LightController::prevEffectMode  (void) {
  selected_mode = ( selected_mode + MAX_MODE- 1 ) % MAX_MODE;
  Serial.println(selected_mode);
}

void LightController::fl_all_leds_set (uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
  for (int i=0;i!=PIXEL_COUNT_FRONT;i++) {
    leds[i] = CRGBW(r,g,b,w);
  }
  //FastLED.show();
}

void LightController::fl_all_leds_set (CRGB c) {
  for(int i = 0; i < PIXEL_COUNT_FRONT; i++){
    leds[i] = c;
  }
}
void LightController::fl_single_led_set (uint16_t led, CRGB c) {
  if(led <= PIXEL_COUNT_FRONT) {
    leds[led] = c;
  }
}
void LightController::fl_single_led_set (uint16_t led, uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
  if(led <= PIXEL_COUNT_FRONT) {
    leds[led] = CRGBW(r,g,b,w);
  }
}
void LightController::bl_all_leds_set (uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
  for (int i=PIXEL_COUNT_FRONT;i!=PIXEL_COUNT;i++) {
    leds[i] = CRGBW(r,g,b,w);
  }
}
void LightController::bl_single_led_set (uint8_t led, uint8_t r, uint8_t g, uint8_t b, uint8_t w){
  if(led <= PIXEL_COUNT_BACK) {
    leds[led+PIXEL_COUNT_FRONT] = CRGBW(r,g,b,w);
  }
}
void LightController::all_off (void) {
   fl_all_leds_set (0,0,0,0);
   bl_all_leds_set (0,0,0,0);
}

void LightController::all_leds_set (uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
   fl_all_leds_set (r, g, b, w);
   bl_all_leds_set (r, g, b, w);
}
void LightController::update (void) {
  FastLED.show();
}

void LightController::colorWheelMode (uint8_t value) {
  fl_all_leds_set ( wheel(value) );
}
