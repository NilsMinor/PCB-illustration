

#include "pcbi_ir.h"



                    //    ON        OFF      L_UP     L_DOWN     FLASH     STROBE     FADE     SMOOTH
uint64_t ir_one [] = { 0xFFB04F, 0xFFF807, 0xFF906F, 0xFFB847 , 0xFFB24D, 0xFF00FF, 0xFF58A7, 0xFF30CF};

                    //   RED      GREEN      BLUE      WHITE
uint64_t ir_col [] = { 0xFF9867, 0xFFD827, 0xFF8877, 0xFFA857,
                       0xFFE817, 0xFF02FD, 0xFF50AF, 0xFF38C7,  // red column top > down
                       0xFF48B7, 0xFF32CD, 0xFF7887, 0xFF28D7,  // green column top > down
                       0xFF6897, 0xFF20DF, 0xFF708F, 0xFFF00F}; // blue column top > down

                       
CRGB remote_colors [] = {CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::White,
                         CRGB::Crimson, CRGB::DarkOrange, CRGB::Gold, CRGB::Orange,
                         CRGB::DarkGreen, CRGB::DeepSkyBlue, CRGB::DodgerBlue, CRGB::DarkTurquoise,
                         CRGB::MediumSlateBlue, CRGB::Indigo, CRGB::DarkViolet, CRGB::DeepPink };

RGBRemoteController::RGBRemoteController () {
  irrecv = new IRrecv (IR_PIN);
  irrecv->enableIRIn( );  // Start the receiver

  selectedColor = CRGB::White;
}


bool RGBRemoteController::checkIRRemote (void) {
  if (irrecv->decode(&results)) {
    // print() & println() can't handle printing long longs. (uint64_t)
    ir_codes_pos code = resolveIRCode();
    //Serial.print ("IR Code nr: ");
    //Serial.println (code);

    switch (code) {
      case ir_on:           cbOn (); break;
      case ir_off:          cbOff (); break;
      case ir_lup:          cbLup (); break;
      case ir_ldn:          cbLdn (); break;
      case ir_flash:        cbEup (); break;
      case ir_strobe:       cbEdn (); break;
      case ir_fade:         break;
      case ir_smooth:       break;
      case ir_color:        cbCol (); break;  // color must be loaded before
      
      default : 
        //Serial.print("Not implemented Code: 0x");
        //serialPrintUint64(results.value, HEX); 
        //Serial.println();
        break;
    }

    irrecv->resume();  // Receive the next value
  }
}

CRGB RGBRemoteController::getCallbackColor (void) {
  return selectedColor;
}

// Translate code to enum in order to know the button
ir_codes_pos RGBRemoteController::resolveIRCode (void) {

  // check if code is a comand
  for (int code = 0; code != IR_COMAND_CODES; code++) {
    // compare codes with the stored codes of the remote 
    if (results.value == ir_one[code]) {

      return (ir_codes_pos) code;
    }
  }
  // check if code is a color
  for (int code = 0; code != IR_COLOR_CODES; code++) {
      if (results.value == ir_col[code]) {
        selectedColor = remote_colors [ code ];
        return ir_color;
      }
   }
  
  return ir_error;
}

void RGBRemoteController::setCallbackOn   (void (*_cbOn)(void))  { cbOn    = _cbOn; }
void RGBRemoteController::setCallbackOff  (void (*_cbOff)(void)) { cbOff   = _cbOff; }
void RGBRemoteController::setCallbackLup  (void (*_cbLup)(void)) { cbLup   = _cbLup; }
void RGBRemoteController::setCallbackLdn  (void (*_cbLdn)(void)) { cbLdn   = _cbLdn; }
void RGBRemoteController::setCalbackColor (void (*_cbCol)(void)) { cbCol   = _cbCol; }
void RGBRemoteController::setCalbackEffectUp  (void (*_cbEup)(void)) { cbEup   = _cbEup; }
void RGBRemoteController::setCalbackEffectDn  (void (*_cbEdn)(void)) { cbEdn   = _cbEdn; }
