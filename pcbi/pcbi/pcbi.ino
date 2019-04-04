
#include "pcbi_config.h"
#include <BlynkSimpleEsp8266.h>



/*
   Blynk connections :

    Pin    description         value
    V0     on-off switch       0 - 1
    V1     brightness          0 - 100
    V2     mode selection      0 - ?
    V3     mode menu           0 - ?
    V4     effect speed        0 - 10
    V5     rgb channel

    V10    temeprature value   XX.X °C
    V11    humidity value      XX.X %


    based on https://community.blynk.cc/t/esp8266-ws2812b-cool-desk-lamp/5633

    use http://arduino.esp8266.com/stable/package_esp8266com_index.json for esp8266 support (V2.4.2)
    using wemos d1 mini
    use blynk V0.5.4 (otherwise esp floating point is not working for virtual writes
    Adafruit Sesnor AM2302


    Groove Touch Slider:
    _________________________
   |         [0]  [100]      |
   |(ON/OFF) >>>>>>>>> (MODE)|
   |_________________________|

*/

#define BLYNK_PRINT Serial      // Comment this out to disable prints and save space
#include <ESP8266WiFi.h>        // 
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

const char* host = "PCBi panel";

#include "Seeed_CY8C401XX.h"    // Grove touch sensor
#include "pcbi_light.h"         // LightController class
#include "pcbi_ir.h"            // ir remote controller
#include "pcbi_sensors.h"       // tempertaure and humidity


CY8C touch;
BlynkTimer timer;
LightController lc;
RGBRemoteController ir;
PCBISensors sensors;  



// global variables
uint8_t red, green, blue;
bool on_off = false;
unsigned long previousTime = 0;

void sendTemperatureHumidity(void);
void handleTouch (void);
void handleClap (void);

void lightTurnOn (void) {
  on_off = true;
  lc.setMOSFET (true);
  Blynk.virtualWrite (V0, on_off);
}
void lightTurnOff (void) {
  on_off = false;
  lc.setMOSFET (false);
  Blynk.virtualWrite (V0, on_off);
}
void lightTurnUp (void) {
  lc.upBrightness ();
  Blynk.virtualWrite (V1, (int)(lc.getBrightness() ));
}
void lightTurnDn (void) {
  lc.downBrightness ();
  Blynk.virtualWrite (V1, (int)(lc.getBrightness() ));
}
void lightEffectUp (void) {
  lc.nextEffectMode();
  Blynk.virtualWrite(V3, lc.getEffectMode());
}
void lightEffectDn (void) {
  lc.prevEffectMode();
  Blynk.virtualWrite(V3, lc.getEffectMode());
}
void lightTurnColor (void) {
  // change mode to rgb wheel
  lc.setEffectMode(2);
  Blynk.virtualWrite(V3, lc.getEffectMode());
  // resolve selected color
  CRGB color = ir.getCallbackColor ();
  red = color.red;
  green = color.green;
  blue = color.blue;
  //lc.fl_all_leds_set(color);
}

void setup() {

  red = green = blue = 0;
  
  Serial.begin(115200);
  Blynk.begin(auth, wlan_name, wlan_pw);
  
  touch.init();

  // setup blynk app
  //timer.setInterval(1000L, sendTemperatureHumidity);  // Setup a function to be called every second
  timer.setInterval(100, runLEDMode);             // timer for led modes/effects

  Blynk.virtualWrite(V1, (int)(lc.getBrightness() * 100));   // set default brightness
  Blynk.virtualWrite(V3, lc.getEffectMode() );             // set default mode
  Blynk.virtualWrite(V4, lc.getEffectSpeed () );              // set effect speed
  Blynk.syncAll();

  ArduinoOTA.setHostname("PCBi panel");
  ArduinoOTA.setPassword("1234");
  ArduinoOTA.begin();
  ArduinoOTA.onStart([]() {
    lc.all_off ();
    for (int i = 0; i != 25; i++)
      lc.fl_single_led_set (i, 0, 255, 0, 0);
  });

  ir.setCallbackOn   ( &lightTurnOn );
  ir.setCallbackOff  ( &lightTurnOff );
  ir.setCallbackLup  ( &lightTurnUp );
  ir.setCallbackLdn  ( &lightTurnDn );
  ir.setCalbackColor ( &lightTurnColor );
  ir.setCalbackEffectUp ( &lightEffectUp );
  ir.setCalbackEffectDn ( &lightEffectDn );

  lc.connectWLANIndicator ( );
}

//----------------------------------------------------------------------------------------------------

void runLEDMode ( void ) {
  lc.runLEDMode ( );
}

BLYNK_WRITE(V0) {
  on_off = (float) param.asInt();
}

BLYNK_WRITE(V1) {
  lc.setBrightness( (float) param.asInt()  );
}

BLYNK_WRITE(V2) {
  if (param.asInt() == 1)
    lc.nextEffectMode ( );
  else
    lc.prevEffectMode ( );
  Blynk.virtualWrite(V3, lc.getEffectMode());
}
BLYNK_WRITE(V3) {
  lc.setEffectMode( param.asInt() );
}
BLYNK_WRITE(V4) {
  lc.setEffectSpeed ( param.asInt() * 50 );
  Serial.println( lc.getEffectSpeed() );
}

BLYNK_WRITE(V5) {
  red = param [0].asInt ();
  green = param [1].asInt ();
  blue = param [2].asInt ();
}

//----------------------------------------------------------------------------------------------------

void loop() {

  //handleClap( );
  handleTouch ();

  ir.checkIRRemote ();

  if (on_off) {
    timer.run();                      // update timer
    switch ( lc.getEffectMode () ) {

      case 0:   // Frontlight (Warm White)
        lc.fl_all_leds_set(0, 0, 0, 255);
        lc.bl_all_leds_set(0, 0, 0, 0);
        break;

      case 1: // Backlight (Warm White)
        lc.fl_all_leds_set (0, 0, 0, 0);
        lc.bl_all_leds_set (0, 0, 0, 255);
        break;

      case 2: // RGB (free-wheel)
        lc.fl_all_leds_set( red, green, blue, 0);
        lc.bl_all_leds_set (0, 0, 0, 0);
        break;

      case 3: // music mode
        break;

      case 4:
        break;

      case 5: // RGB (Feuer)
        // lc.runLEDMode ( );   // timer based effect
        break;
    }
  }

  else {
    lc.all_off ();
  }

  if (sensors.update ()) {
    Blynk.virtualWrite(V10, sensors.getTemperature());
    Blynk.virtualWrite(V11, sensors.getHumidity());
  }

  Blynk.run();          // run blynk communication
  lc.update ();         // update leds
  
  ArduinoOTA.handle();  // For OTA
}

void handleClap (void) {

  static int claps = 0;
  static long detectionSpanInitial = 0;
  static long detectionSpan = 0;
  static double prev_adc_value = 0;
  static double adc_value = 0;


  adc_value = analogRead(A0) - 400;

  if (adc_value < 0)
    adc_value = adc_value * (-1);

  /* if (prev_adc_value > adc_value)
     adc_value = 400 + (prev_adc_value - adc_value);

    prev_adc_value = adc_value;
  */
  //Serial.println(adc_value);


  //int sensorState = digitalRead(MIC_PIN);

  if ((adc_value > 200 && claps == 0) || adc_value > 100 && claps == 1)
  {
    if (claps == 0)
    {
      detectionSpanInitial = detectionSpan = millis();
      Serial.println("Clap 1");
      claps++;
    }
    else if (claps > 0 && millis() - detectionSpan >= 50)
    {
      detectionSpan = millis();
      Serial.println("Clap 2");
      claps++;
    }
  }

  if (millis() - detectionSpanInitial >= 400)
  {
    if (claps == 2)
    { Serial.println("Toggle on off");
      // toggle on/off state
      if (on_off) on_off = false;
      else        on_off = true;
    }
    claps = 0;
  }
}
// handle touch events, needed to change variable types (eg u8 to uint8_t) for esp8266
void handleTouch (void) {
  uint8_t value = 0;
  static bool pressed1 = false;
  static bool pressed2 = false;

  touch.get_touch_button_value(&value);

  // handle touch button by adding a pressed/released function
  if (value & 0x01 && pressed1 == false) {     // button is pressed
    Serial.println("button 1 is pressed");
    pressed1 = true;
    // toggle on/off state
    if (on_off) on_off = false;
    else        on_off = true;

    Blynk.virtualWrite (V0, on_off);
  }
  else if (!(value & 0x01) && pressed1 == true) { // button was released
    Serial.println("button 1 is released");
    pressed1 = false;
  }

  // button 2
  if (value & 0x2 && pressed2 == false) {       // button is pressed
    // use for event setting
    pressed2 = true;
    Serial.println("button 1 is pressed");

    lc.nextEffectMode ( );
  }
  else if (!(value & 0x02) && pressed2 == true) { // button was released
    Serial.println("button 1 is released");
    pressed2 = false;
  }


  // slider
  touch.get_touch_slider_value(&value);

  if (value != 0) {
    lc.setBrightness(1 - ((double) value / 100.0) );
    Blynk.virtualWrite (V1, (int)(lc.getBrightness() * 100));
    Serial.print("slider value is ");
    Serial.println(value);
  }
}
