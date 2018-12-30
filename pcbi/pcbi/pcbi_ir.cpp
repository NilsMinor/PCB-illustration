

#include "pcbi_ir.h"



RGBRemoteController::RGBRemoteController () {
  irrecv = new IRrecv (IR_PIN);
  irrecv->enableIRIn();  // Start the receiver
}


bool RGBRemoteController::checkIRRemote (void) {
  if (irrecv->decode(&results)) {
    // print() & println() can't handle printing long longs. (uint64_t)
    serialPrintUint64(results.value, HEX);
    Serial.println("");
    irrecv->resume();  // Receive the next value
  }
}



void RGBRemoteController::translateIRCode (void) {
  
}
