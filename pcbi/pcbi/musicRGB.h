#ifndef _MUSIC_RGB_H_
#define _MUSIC_RGB_H_

#include "Arduino.h"
#include <Adafruit_NeoPixel.h>


#define SAMPLES 256              //Must be a power of 2
#define SAMPLING_FREQUENCY 10000 //Hz, must be 10000 or less due to ADC conversion time. Determines maximum frequency that can be analysed by the FFT.
#define amplitude 25
#define SLOTS   7
#define MIC_PIN     0           //           


void musiRGBInit (void);
void musicRGBFFT (void);
void musicRGBcolor (Adafruit_NeoPixel * strip);
uint32_t Wheel(byte WheelPos, Adafruit_NeoPixel * strip);

#endif
