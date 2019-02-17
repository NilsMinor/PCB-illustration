
/* This file is part of the light system. Effects and and lighting stuff using timer and the Blynk object
 * can not be part of the cpp (source file). For example in order to use Blynk.run() in an effect function
 * it must be part of this file using the extension .ino as the Blynk object is part of it's library. There
 * might be a smarter way but it is working for me and organizes my code in my fashion ^^.
 */

#include "pcbi_light.h"

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

void LightController::runLEDMode (void) {

   static unsigned long currentTime = 0;
   static unsigned long previousTime = 0;
   
   currentTime = millis();
   
   unsigned long deltaTime = currentTime - previousTime;
   

   //Serial.print("Update : ");
   //Serial.println(getEffectSpeed());

   if ( deltaTime > (getEffectSpeed () * 0.9)) {
    Serial.print ("Elapsed Time: ");
    Serial.println(deltaTime);
    
   
   
      switch (selected_mode) {
     
          case MODE_RAINBOW:  // RGB (rainbow)
               rainbowCycleFront ( );
            break;
    
          case MODE_COLOR_SHIFT:
              collorShiftAll ( );
              break;
    
          case MODE_FIRE:     // RGB (fire)
               Fire(55,120,15);
            break;
      }
      previousTime = currentTime;
   }
  
}
// Connect to WLAN, wait while not conected
void LightController::connectWLANIndicator (void) {
  
  all_off ();

  // wait for wlan connection
  Serial.print("Connecting");
  for (int i=0;i!=25;i++)
    fl_single_led_set (i,0,0,255,0);

  lc.update ();   // update leds
    
  while (Blynk.connect() == false) { 
    Serial.print(".");
    delay(100); 
  } 
  Serial.println();

  Serial.print("connected");
  all_off ();
}



void LightController::collorShiftAll(){
  static uint8_t hue;

  uint8_t color = 0;
  for(int i = 0; i < PIXEL_COUNT_FRONT; i++ ){
    leds[i] = CHSV((color * 256 / PIXEL_COUNT_FRONT) + hue, 255, 255);
    handleTouch ();
    Blynk.run();   
  }
  update ();
  hue++;
  color++;
  Blynk.run(); 
}
void LightController::rainbowCycleFront(){
  static uint8_t hue;
 
  for(int i = 0; i < PIXEL_COUNT_FRONT; i++ ){
    leds[i] = CHSV((i * 256 / PIXEL_COUNT_FRONT) + hue, 255, 255);
    handleTouch ();
    Blynk.run();   
  }
  update ();
  hue++;
  Blynk.run(); 
}

void LightController::setPixelHeatColor (int Pixel, byte temperature) {
    // Scale 'heat' down from 0-255 to 0-191
    byte t192 = round((temperature/255.0)*191);
  
    // calculate ramp up from
    byte heatramp = t192 & 0x3F; // 0..63
    heatramp <<= 2; // scale up to 0..252
  
    // figure out which third of the spectrum we're in:
    if( t192 > 0x80) {                     // hottest
      fl_single_led_set(Pixel, 255, 255, heatramp,0);
    } else if( t192 > 0x40 ) {             // middle
      fl_single_led_set(Pixel, 255, heatramp, 0,0);
    } else {                               // coolest
      fl_single_led_set(Pixel, heatramp, 0, 0,0);
    }
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
  
    update ();
  }
