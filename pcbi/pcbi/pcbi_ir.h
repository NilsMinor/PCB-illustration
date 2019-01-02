
/* Class to control the pcbi panel with common RGB infrared remotes like this(https://www.ebay.de/itm/401420892888)
 * Based on IRrecvDumpV2 from David Conran and Ken Shirri0xFF.  
 * 
 * My setup uses an TSOP-31238 (https://cdn-reichelt.de/documents/datenblatt/A500/TSOP312SERIES_DATASHEET.pdf) with 38kHz
 *
 */


#ifndef _PCBI_IR_H_
#define _PCBI_IR_H_

#include <IRrecv.h>
#include <IRremoteESP8266.h>
#include <IRutils.h>
#include "Arduino.h"
#include "FastLED.h"    // required for callback

//#include <HCSR04.h>
//#include <movingAvg.h>
// setup
// HCSR04 hc(16,0);//initialisation class HCSR04 (trig pin , echo pin)
// movingAvg avgTemp(10);
// loop
// float temp = hc.dist();
// float avg = avgTemp.reading(temp);

#define IR_PIN                  2
#define IR_COMAND_CODES         8
#define IR_COLOR_CODES          16

typedef void (*Callback)(void);

enum ir_codes_pos {
  ir_error = -1,
  ir_on = 0, 
  ir_off, 
  ir_lup,
  ir_ldn,
  ir_flash,
  ir_strobe,
  ir_fade,
  ir_smooth,
  ir_color
  };

class RGBRemoteController{
  public:
    RGBRemoteController ();
    bool checkIRRemote (void);
    CRGB getCallbackColor (void);

    void setCallbackOn      (void (*_cbOn) (void));
    void setCallbackOff     (void (*_cbOff)(void));
    void setCallbackLup     (void (*_cbLup)(void));
    void setCallbackLdn     (void (*_cbLdn)(void));
    void setCalbackColor    (void (*_cbCol)(void));
    void setCalbackEffectUp (void (*_cbEup)(void));
    void setCalbackEffectDn (void (*_cbEdn)(void));

  private:
    ir_codes_pos resolveIRCode (void);
    ir_codes_pos resolveColor (void);
    
    IRrecv * irrecv;
    decode_results results;  
    CRGB selectedColor;

    void (*cbOn)  (void);
    void (*cbOff) (void);
    void (*cbLup) (void);
    void (*cbLdn) (void);
    void (*cbCol) (void);
    void (*cbEup) (void);
    void (*cbEdn) (void);
};

#endif
