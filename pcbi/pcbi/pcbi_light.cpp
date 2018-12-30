
#include "pcbi_light.h"
#include <ESP8266WiFi.h>
//#include <BlynkSimpleEsp8266.h>

LightController::LightController ( ) {
  strip = new Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRBW + NEO_KHZ800);
  red = green = blue = 0;

  brightness = 0.5;
  effect_speed = 5;
  selected_mode = 1;
  strip->begin();

  // musiRGBInit ();

}

float LightController::getBrightness (void) {
  return brightness;
}
void  LightController::setBrightness (float br) {
  if (br >= 0 && br <= 1.0)
    brightness = br;
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
void LightController::nextEffectMode  (uint8_t v) {
  selected_mode = ( selected_mode + v ) % MAX_MODE;
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
            Fire(55,120,15);
        break;
    }
}
void LightController::all_off (void) {
   all_leds (0,0,0,0);
   bl_all_leds (0,0,0,0);
}

// set all leds with one command
void LightController::all_leds (uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
  for (int i=0;i!=PIXEL_COUNT_FRONT;i++) {
      strip->setPixelColor(i, strip->Color(r*brightness, g*brightness, b*brightness,w*brightness));
  }
  strip->show();
}

void LightController::bl_all_leds (uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
  for (int i=PIXEL_COUNT_FRONT;i!=PIXEL_COUNT;i++) {
      strip->setPixelColor(i, strip->Color(r*brightness, g*brightness, b*brightness,w*brightness));
  }
  strip->show();
}

void LightController::single_led (uint8_t pin, uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
  strip->setPixelColor(pin, strip->Color(r*brightness, g*brightness, b*brightness,w*brightness));
  strip->show();
}



void LightController::setPixelHeatColor (int Pixel, byte temperature) {
  // Scale 'heat' down from 0-255 to 0-191
  byte t192 = round((temperature/255.0)*191);
 
  // calculate ramp up from
  byte heatramp = t192 & 0x3F; // 0..63
  heatramp <<= 2; // scale up to 0..252
 
  // figure out which third of the spectrum we're in:
  if( t192 > 0x80) {                     // hottest
    strip->setPixelColor(Pixel, 255, 255, heatramp);
  } else if( t192 > 0x40 ) {             // middle
    strip->setPixelColor(Pixel, 255, heatramp, 0);
  } else {                               // coolest
    strip->setPixelColor(Pixel, heatramp, 0, 0);
  }
}

void LightController::musicMode (void) {
  musicRGBFFT ( );
  musicRGBcolor (strip);
  if (selected_mode != MODE_MUSIC) return;
}

void LightController::Fire(int Cooling, int Sparking, int SpeedDelay) {
  static byte heat[PIXEL_COUNT];
  int cooldown;
  
  // Step 1.  Cool down every cell a little
  for( int i = 0; i < PIXEL_COUNT_FRONT; i++) {
    cooldown = random(0, ((Cooling * 10) / PIXEL_COUNT) + 2);
    
    if(cooldown>heat[i]) {
      heat[i]=0;
    } else {
      heat[i]=heat[i]-cooldown;
    }
  }
  
  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for( int k= PIXEL_COUNT_FRONT - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }
    
  // Step 3.  Randomly ignite new 'sparks' near the bottom
  if( random(255) < Sparking ) {
    int y = random(7);
    heat[y] = heat[y] + random(160,255);
    //heat[y] = random(160,255);
  }

  // Step 4.  Convert heat to LED colors
  for( int j = 0; j < PIXEL_COUNT_FRONT; j++) {
    setPixelHeatColor(j, heat[j] );
  }

  strip->show();
  //delay(SpeedDelay);
}
