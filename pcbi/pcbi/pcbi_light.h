
#ifndef _PCBI_LIGHT_H_
#define _PCBI_LIGHT_H_

// control functions and effects for pcbi light panel
// based on https://www.partsnotincluded.com/neopixels/fastled-rgbw-neopixels-sk6812/ to use fastLED with sk6812 rgbw

#include "Arduino.h"
#include "FastLED.h"
#include "FastLED_RGBW.h"


/*  
 *    MIC           A0    A0
 *    HC ECHO       D0    16
 *    I2C SCL       D1    5
 *    I2C SDA       D2    4
 *    HC TRIG       D3    0
 *    IR_PIN        D4    2
 *    LED PWR       D5    14 
 *    LED DATA      D6    12
 *    DHT PIN       D7    13           
 *                  D8    15 
 *  
 *   HCSR04 hc(0,16);//initialisation class HCSR04 (trig pin , echo pin)
              
 */


//#include "musicRGB.h"

// LED strip related configs
#define DATA_PIN                12                                    // Wemos pin D6 
#define PIXEL_COUNT_BACK        (5*26)                                // Pixels count backlight 5 rows a 26 leds per strip
#define PIXEL_COUNT_FRONT       159                                   // Pixels count front 1 row   
#define PIXEL_COUNT             (PIXEL_COUNT_FRONT+PIXEL_COUNT_BACK)  // Total pixel count
#define BR_STEP                 10                                    // brightness step
#define LED_PWR_PIN             14

#define MAX_MODE                7                                     // max modes
#define MODE_COLOR_SHIFT        3
#define MODE_RAINBOW            4
#define MODE_FIRE               5
#define TWINKLE_SNOW            6

class LightController{
  public:
              LightController ( );
    void      setMOSFET  ( bool state ); 
    uint8_t   getBrightness   (void);
    void      setBrightness   (uint8_t br);
    void      upBrightness    (void);
    void      downBrightness  (void);
    uint16_t  getEffectSpeed  (void);
    void      setEffectSpeed  (uint16_t es);
    uint8_t   getEffectMode   (void);
    void      setEffectMode   (uint8_t mo);
    void      nextEffectMode  (void);
    void      prevEffectMode  (void);
    void      update (void);

    void      fl_all_leds_set (CRGB c);
    void      fl_all_leds_set (uint8_t r, uint8_t g, uint8_t b, uint8_t w);
    void      fl_single_led_set (uint16_t led, CRGB c);
    void      fl_single_led_set (uint16_t led, uint8_t r, uint8_t g, uint8_t b, uint8_t w);
        
    void      bl_all_leds_set (uint8_t r, uint8_t g, uint8_t b, uint8_t w);
    void      bl_single_led_set (uint8_t led, uint8_t r, uint8_t g, uint8_t b, uint8_t w);

    void      connectWLANIndicator (void);
    void      all_off (void);
    void      all_leds_set (uint8_t r, uint8_t g, uint8_t b, uint8_t w);
     
    void      runLEDMode (void);
    void      colorWheelMode (uint8_t value);

    // effects
    void      rainbowCycleFront (void);
    void      musicMode (void);
    void      collorShiftAll (void);
    void      Fire (int Cooling, int Sparking, int SpeedDelay);
    void      TwinkleSnow (void);
    void      addLedColor ( CRGBW crgbw, uint16_t pixel);
    void      subLedColor ( CRGBW crgbw, uint16_t pixel);

  private:
    CRGBW leds[PIXEL_COUNT];
    CRGB *ledsRGB = (CRGB *) &leds[0];
    float   brightness;
    uint16_t effect_speed;
    uint8_t selected_mode;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    CRGB wheel(uint8_t WheelPos);

    void    setPixelHeatColor (int Pixel, byte temperature);
     
  
};










#endif
