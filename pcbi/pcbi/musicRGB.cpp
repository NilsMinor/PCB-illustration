
#include "musicRGB.h"
#include "arduinoFFT.h"



unsigned int sampling_period_us;
unsigned long microseconds;
float values[] = {0,0,0,0,0,0,0};
double vReal[SAMPLES];
double vImag[SAMPLES];
unsigned long newTime, oldTime;


arduinoFFT FFT = arduinoFFT();

void musiRGBInit (void) {
  sampling_period_us = round(1000000 * (1.0 / SAMPLING_FREQUENCY));
}


void musicRGBFFT (void) {
  for (int i = 0; i < SAMPLES; i++) {
    newTime = micros()-oldTime;
    oldTime = newTime;
    vReal[i] = analogRead(A0); // A conversion takes about 1mS on an ESP8266
    vImag[i] = 0;
    while (micros() < (newTime + sampling_period_us)) { /* do nothing to wait */ }
  }
  FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
  FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);
  for (int i = 2; i < (SAMPLES/2); i++){ // Don't use sample 0 and only first SAMPLES/2 are usable. Each array eleement represents a frequency and its value the amplitude.
    if (vReal[i] > amplitude) { // Add a crude noise filter, 4 x amplitude or more
      if (i<=5 )             values[0] = (int)vReal[i]/amplitude; // 125Hz
      if (i >5   && i<=12 )  values[1] = (int)vReal[i]/amplitude; // 250Hz
      if (i >12  && i<=32 )  values[2] = (int)vReal[i]/amplitude; // 500Hz
      if (i >32  && i<=62 )  values[3] = (int)vReal[i]/amplitude; // 1000Hz
      if (i >62  && i<=105 ) values[4] = (int)vReal[i]/amplitude; // 2000Hz
      if (i >105 && i<=120 ) values[5] = (int)vReal[i]/amplitude; // 4000Hz
      if (i >120 && i<=146 ) values[6] = (int)vReal[i]/amplitude; // 8000Hz
    }
  }
}

void musicRGBcolor (Adafruit_NeoPixel * strip) {
  int ledgr = -1;

  // find max
  float max_amp = values[0];
  for (byte i=1; i!= SLOTS; i++) {
    if (values[i] > max_amp)
      max_amp = values[i];
  }

 //Serial.println (max_amp);
  
  int color[7][3] = {  
   {255, 0, 0},   
   {255, 0, 66},    
   {185, 0, 221},      
   {62, 0, 188},    
   {8, 201, 0},    
   {196, 134, 0}     
  };
  
  //Serial.println ("-------------------------");
  for (int leds=0; leds!=strip->numPixels(); leds++) {
    
    strip->setPixelColor(leds, strip->Color(color[ledgr][0] * ((float)values[ledgr]/max_amp),
                                            color[ledgr][1] * ((float)values[ledgr]/max_amp),
                                            color[ledgr][2] * ((float)values[ledgr]/max_amp)));

    if (leds%20 == 0 && leds > 10) {
      ledgr++;
      //Serial.print (ledgr);
      //Serial.print (" = ");
      //Serial.println (values[ledgr]/max_amp);
    } 

     
  }
  strip->show();


  //for (int i=0; i!=5; i++)
  //   Serial.println (values[i]);

}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos, Adafruit_NeoPixel * strip) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip->Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip->Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  
  WheelPos -= 170;
  return strip->Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
