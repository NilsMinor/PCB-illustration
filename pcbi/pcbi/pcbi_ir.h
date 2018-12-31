
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

#define IR_PIN          2
#define IR_CODES        8

enum ir_codes_pos {
  ir_error = -1,
  ir_on = 0, 
  ir_off, 
  ir_lup,
  ir_ldown,
  ir_red,
  ir_green,
  ir_blue,
  ir_white
  };


class RGBRemoteController{
  public:
    RGBRemoteController ();
    bool checkIRRemote (void);

  private:
    
    ir_codes_pos translateIRCode (void);
    
    IRrecv *irrecv;
    decode_results results;  // Somewhere to store the results 
    

};






#endif
