
#include "pcbi_light.h"
#include <ESP8266WiFi.h>
//#include <BlynkSimpleEsp8266.h>

LightController::LightController ( ) {
  FastLED.addLeds<WS2812B, DATA_PIN, RGB>(ledsRGB, getRGBWsize(PIXEL_COUNT));
  FastLED.show();
  
  red = green = blue = 0;

  brightness = 0.8;
  effect_speed = 5;
  selected_mode = 0;
  pinMode (LED_PWR_PIN, OUTPUT);
  digitalWrite(LED_PWR_PIN, HIGH);
  
  // musiRGBInit ();
}

float LightController::getBrightness (void) {
  return brightness;
}
void  LightController::setBrightness (float br) {
  if (br >= 0 && br <= 1.0) {
    brightness = br;
    FastLED.setBrightness(brightness * 255);
    FastLED.show();
  }
}
void  LightController::upBrightness (void) {
  if (brightness <= 1.0) {
    brightness += BR_STEP;
    FastLED.setBrightness(brightness * 255);
    FastLED.show();
  }
}
void  LightController::downBrightness (void) {
  if (brightness > BR_STEP ) {
    brightness -= BR_STEP;
    FastLED.setBrightness(brightness * 255);
    FastLED.show();
  }
}
uint8_t LightController::getEffectSpeed (void) {
  return effect_speed;
}
void LightController::setEffectSpeed (uint8_t es) {
  if (es <= 10)
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
void LightController::runLEDMode (void) {

   static unsigned long CurrentTime = 0;
   CurrentTime = millis();
   Serial.print ("Elapsed Time: ");
   Serial.println(CurrentTime);
   
  switch (selected_mode) {
 
      case MODE_RAINBOW:  // RGB (rainbow)
           rainbowCycle ( );
        break;

      case MODE_FIRE:     // RGB (fire)
           // Fire(55,120,15);
        break;
    }
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
void LightController::fl_single_led_set (uint8_t led, uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
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

CRGB LightController::wheel(uint8_t WheelPos) {
  CRGB color;
  if (85 > WheelPos) {
   color.r = 0;
   color.g = WheelPos * 3;
   color.b = (255 - WheelPos * 3);
  } 
  else if (170 > WheelPos) {
   color.r = WheelPos * 3;
   color.g = (255 - WheelPos * 3);
   color.b = 0;
  }
  else {
   color.r = (255 - WheelPos * 3);
   color.g = 0;
   color.b = WheelPos * 3;
  }
  return color;
}
