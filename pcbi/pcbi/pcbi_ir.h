
/* Class to control the pcbi panel with common RGB infrared remotes like this(https://www.ebay.de/itm/401420892888)
 * Based on IRrecvDumpV2 from David Conran and Ken Shirriff.  
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


#define IR_PIN                2


class RGBRemoteController{
  public:
    RGBRemoteController ();
    bool checkIRRemote (void);
    bool isIdle (void);

  private:
    
    void translateIRCode (void);
    
    IRrecv *irrecv;
    decode_results results;  // Somewhere to store the results 
    

};






#endif
