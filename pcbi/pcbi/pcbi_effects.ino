
/* This file is part of the light system. Effects and and lighting stuff using timer and the Blynk object
 * can not be part of the cpp (source file). For example in order to use Blynk.run() in an effect function
 * it must be part of this file using the extension .ino as the Blynk object is part of it's library. There
 * might be a smarter way but it is working for me and organizes my code in my fashion ^^.
 */

#include "pcbi_light.h"


// Connect to WLAN, wait while not conected
void LightController::connectWLANIndicator (void) {
  
  all_off ();

  // wait for wlan connection
  Serial.print("Connecting");
  for (int i=0;i!=25;i++)
    single_led (i,0,0,255,0);
    
  while (Blynk.connect() == false) { 
    Serial.print(".");
    delay(100); 
  } 
  Serial.println();

  Serial.print("connected");
  all_off ();
}


void LightController::rainbowCycle (void) {
  uint16_t i, j;
  /* 
  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< PIXEL_COUNT_FRONT; i++) {
      strip->setPixelColor(i, Wheel(((i * 256 / PIXEL_COUNT_FRONT) + j) & 255, strip));
      //handleTouch ();
      if (selected_mode != MODE_RAINBOW) return;
      Blynk.run();   
    }
    strip->show();
    Blynk.run(); 
  }
  */
}
