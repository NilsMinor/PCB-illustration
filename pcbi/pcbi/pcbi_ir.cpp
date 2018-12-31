

#include "pcbi_ir.h"

                    //    ON         OFF      L_UP     L_DOWN      RED     GREEN      BLUE      WHITE
uint64_t ir_one [] = { 0xFFB04F, 0xFFF807, 0xFF906F, 0xFFB847, 0xFF9867, 0xFFD827, 0xFF8877, 0xFFA857  };

RGBRemoteController::RGBRemoteController () {
  irrecv = new IRrecv (IR_PIN);
  irrecv->enableIRIn();  // Start the receiver
}


bool RGBRemoteController::checkIRRemote (void) {
  if (irrecv->decode(&results)) {
    // print() & println() can't handle printing long longs. (uint64_t)
    ir_codes_pos code = resolveIRCode();
    Serial.print ("IR Code nr: ");
    Serial.println (code);

    switch (code) {
      case ir_on:   cbOn (); break;
      case ir_off:  cbOff (); break;
      case ir_lup:  cbLup (); break;
      case ir_ldn:  cbLdn (); break;
      default : /*not implemented*/ break;
    }

    irrecv->resume();  // Receive the next value
  }
}


// Translate code 
ir_codes_pos RGBRemoteController::resolveIRCode (void) {
  for (int code = 0; code != IR_CODES; code++) {
    // compare codes with the stored codes of the remote 
    if (results.value == ir_one[code]) {

      return (ir_codes_pos) code;
    }
  }
  return ir_error;
}

void RGBRemoteController::setCallbackOn  (void (*_cbOn)(void))  { cbOn    = _cbOn; }
void RGBRemoteController::setCallbackOff (void (*_cbOff)(void)) { cbOff   = _cbOff; }
void RGBRemoteController::setCallbackLup (void (*_cbLup)(void)) { cbLup   = _cbLup; }
void RGBRemoteController::setCallbackLdn (void (*_cbLdn)(void)) { cbLdn   = _cbLdn; }
