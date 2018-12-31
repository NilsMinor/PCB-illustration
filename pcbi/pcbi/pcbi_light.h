#ifndef _PCBI_LIGHT_H_
#define _PCBI_LIGHT_H_

// control functions and effects for pcbi light panel
// based on https://www.partsnotincluded.com/neopixels/fastled-rgbw-neopixels-sk6812/ to use fastLED with sk6812 rgbw

#include "Arduino.h"
#include "FastLED.h"
#include "FastLED_RGBW.h"

//#include "musicRGB.h"

// LED strip related configs
#define DATA_PIN                12                                    // Wemos pin D6 
#define PIXEL_COUNT_BACK        (5*26)                                // Pixels count backlight 5 rows a 26 leds per strip
#define PIXEL_COUNT_FRONT       159                                   // Pixels count front 1 row   
#define PIXEL_COUNT             (PIXEL_COUNT_FRONT+PIXEL_COUNT_BACK)  // Total pixel count
#define MAX_MODE                6                                     // max modes

#define MODE_MUSIC              3
#define MODE_RAINBOW            4
#define MODE_FIRE               5

class LightController{
  public:
    LightController ( );
    float   getBrightness   (void);
    void    setBrightness   (float br);
    uint8_t getEffectSpeed  (void);
    void    setEffectSpeed  (uint8_t es);
    uint8_t getEffectMode   (void);
    void    setEffectMode   (uint8_t mo);
    void    nextEffectMode  (uint8_t v);

    void    fl_all_leds_set (uint8_t r, uint8_t g, uint8_t b, uint8_t w);
    void    fl_single_led_set (uint8_t led, uint8_t r, uint8_t g, uint8_t b, uint8_t w);
    void    bl_all_leds_set (uint8_t r, uint8_t g, uint8_t b, uint8_t w);
    void    bl_single_led_set (uint8_t led, uint8_t r, uint8_t g, uint8_t b, uint8_t w);

    void    connectWLANIndicator (void);
    void    all_off (void);
    void    all_leds_set (uint8_t r, uint8_t g, uint8_t b, uint8_t w);
     
    void    runLEDMode (void);

    // effects
    void    rainbowCycle (void);
    void    musicMode (void);

  private:
    CRGBW leds[PIXEL_COUNT];
    CRGB *ledsRGB = (CRGB *) &leds[0];
    float   brightness;
    uint8_t effect_speed;
    uint8_t selected_mode;
    uint8_t red;
    uint8_t green;
    uint8_t blue;

    void    setPixelHeatColor (int Pixel, byte temperature);
  
};










#endif
