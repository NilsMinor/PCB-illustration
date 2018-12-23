
#include "pcbi_config.h"

/* 
 * Blynk connections :
 * 
 *  Pin    description         value
 *  V0     on-off switch       0 - 1
 *  V1     brightness          0 - 100
 *  V2     mode selection      0 - ?
 *  V3     mode menu           0 - ?
 *  
 *  V5     rgb channel         
 *  
 *  V10    temeprature value   XX.X °C
 *  V11    humidity value      XX.X %
 *  
 *  based on https://community.blynk.cc/t/esp8266-ws2812b-cool-desk-lamp/5633
 * 
 *  use http://arduino.esp8266.com/stable/package_esp8266com_index.json for esp8266 support (V2.4.2)
 *  using wemos d1 mini
 *  use blynk V0.5.4 (otherwise esp floating point is not working for virtual writes
 *  
 *  
 *  Groove Touch Slider:
 *  _________________________
 * |         [0]  [100]      |
 * |(ON/OFF) >>>>>>>>> (MODE)|
 * |_________________________|          
 *  
 */

#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Adafruit_NeoPixel.h>


#include "Seeed_CY8C401XX.h"    // Grove touch sensor
#include "DHT.h"                // DHT22 temperature and humidity sensor
#include "arduinoFFT.h"
arduinoFFT FFT = arduinoFFT();

// DHT 22 setting
#define DHTPIN 13               // 
#define DHTTYPE DHT22           // DHT 22  (AM2302), AM2321

// LED strip related configs
#define PIXEL_PIN   12          // Wemos D6 
#define PIXEL_COUNT 159         // Pixels Count
#define MODE_MAX    6           // max modes
#define MIC_PIN     0          //           


CY8C touch;
DHT dht(DHTPIN, DHTTYPE);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRBW + NEO_KHZ800);
BlynkTimer timer;
BlynkTimer ledTimer;

// global variables
float brightness = 0.5;
bool on_off = false;
uint8_t selected_mode = 1;
uint8_t red, green, blue;


void all_on (void);
void all_off (void);
void all_color (uint8_t r, uint8_t g, uint8_t b);
void all_wwhite (uint8_t w);
void single_led (uint8_t pin, uint8_t r, uint8_t g, uint8_t b, uint8_t w);
void all_leds (uint8_t r, uint8_t g, uint8_t b, uint8_t w);
void sendTemperatureHumidity(void);
void handle_touch (void);
void connectWLANIndicator (void);
void nextMode (int v);
void runLEDMode (void);
void handleClap (void);

void Fire(int Cooling, int Sparking, int SpeedDelay);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  Blynk.begin(auth, wlan_name, wlan_pw);

  strip.begin();
  dht.begin();
  touch.init();

  pinMode(MIC_PIN, INPUT);

  connectWLANIndicator ();

  red = green = blue = 0;

  timer.setInterval(1000L, sendTemperatureHumidity);  // Setup a function to be called every second
  ledTimer.setInterval(300L, runLEDMode);             // timer for led modes/effects

  brightness = 0.5;
  Blynk.virtualWrite(V1, (int)(brightness * 100));   // set default brightness
  Blynk.virtualWrite(V3, selected_mode);             // set default mode   
  Blynk.syncAll();  
}

//----------------------------------------------------------------------------------------------------

BLYNK_WRITE(V0) {
  on_off = (float)param.asInt();
}

BLYNK_WRITE(V1) {
  brightness = (float) param.asInt()/100;
}

BLYNK_WRITE(V2) {
  nextMode (param.asInt());
}

BLYNK_WRITE(V3) {
  selected_mode = param.asInt();
}

BLYNK_WRITE(V5) {
  red = param [0].asInt ();
  green = param [1].asInt ();
  blue = param [2].asInt ();    
}

//----------------------------------------------------------------------------------------------------

void loop() {

  if (on_off) {
    switch (selected_mode) {

      case 0:   // Normal (Warm White)
      case 255:
              all_leds(0, 0, 0, 255);
        break;
      
      case 1: // Normal (Fading 1)
              all_leds(0, 0, 0, 255);
        break;
  
      case 2: // Normal (Fading 2)
        break;
        
      case 3: // RGB (free-wheel)
            all_leds(red, green, blue,0);
        break;
  
      case 4: // RGB (rainbow)
            runLEDMode ( );   // timer based effect
        break;

      case 5: // RGB (Feuer)
            runLEDMode ( );   // timer based effect
        break;
    }
  }
    
  else {
    all_off ();
  }

  
  handleClap( );
  handle_touch ();

  Blynk.run();
  timer.run();
}



// Connect to WLAN, wait while not conected
void connectWLANIndicator (void) {
  
  all_off ();

  // wait for wlan connection
  Serial.print("Connecting");
  single_led (8,0,0,55,0);
  single_led (9,0,0,155,0);
  single_led (10,0,0,255,0);
  single_led (11,0,0,155,0);
  single_led (12,0,0,55,0);
  while (Blynk.connect() == false) { 
    Serial.print(".");
    delay(100); 
  } 
  Serial.println();

  Serial.print("connected");
  all_off ();
}

// handle next mode (step) event
void nextMode (int v) {
  selected_mode = ( selected_mode + v ) % MODE_MAX;
  
  Serial.print("Mode : ");
  Serial.println(selected_mode);
  
  Blynk.virtualWrite(V3, selected_mode); 
  
}

/*
void calibADC (void) {
  int adc_count = 500;
  double adc = 0;
  for (int i=0; i!= adc_count; i++) {
    adc = analogRead(A0);
  }
  mid_adc = 
}*/
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


  int sensorState = digitalRead(MIC_PIN);
 
  if ((adc_value > 200 && claps == 0) || adc_value > 100 && claps == 1)
  {
    if (claps == 0)
    {
      detectionSpanInitial = detectionSpan = millis();
      Serial.println("Clap 1");
      claps++;
    }
    else if (claps > 0 && millis()-detectionSpan >= 50)
    {
      detectionSpan = millis();
      Serial.println("Clap 2");
      claps++;
    }
  }
 
  if (millis()-detectionSpanInitial >= 400)
  {
    if (claps == 2)
    {   Serial.println("Toggle on off");
        // toggle on/off state
        if (on_off) on_off = false;
        else        on_off = true;
    }
    claps = 0;
  }
}

// handle touch events
// needed to change variable types (eg u8 to uint8_t) for esp8266
void handle_touch (void) {
    uint8_t value=0;
    static bool pressed1 = false;
    static bool pressed2 = false;
    
    touch.get_touch_button_value(&value);

    // handle touch button by adding a pressed/released function
    if(value&0x01 && pressed1 == false) {        // button is pressed
        Serial.println("button 1 is pressed");
        pressed1 = true;
        // toggle on/off state
        if (on_off) on_off = false;
        else        on_off = true;

        Blynk.virtualWrite (V0, on_off);
    }
    else if(!(value&0x01) && pressed1 == true) { // button was released
        Serial.println("button 1 is released");
        pressed1 = false;
    }

    // button 2
    if(value&0x2 && pressed2 == false){           // button is pressed
        // use for event setting
        pressed2 = true;
        Serial.println("button 1 is pressed");
  
        nextMode (1); // +1
    }
    else if(!(value&0x02) && pressed2 == true) {   // button was released
        Serial.println("button 1 is released");
        pressed2 = false;
    }


    // slider
    touch.get_touch_slider_value(&value);
      
    if (value != 0) {  
      brightness = 1 - ((double) value / 100.0);  
      Blynk.virtualWrite (V1, (int)(brightness*100));
      Serial.print("slider value is ");
      Serial.println(value);
    }
}

// update DHT22 temperature and humidity values
void sendTemperatureHumidity(void) {
  
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  
  Blynk.virtualWrite(V10, t);
  Blynk.virtualWrite(V11, h);
}


// ------------------------------------------------------------
void runLEDMode (void) {
  switch (selected_mode) {
 
      case MODE_RAINBOW:  // RGB (rainbow)
           rainbowCycle (100);
        break;

      case MODE_FIRE:     // RGB (fire)
            Fire(55,120,15);
        break;
    }
}
void all_off (void) {
   all_leds (0,0,0,0);
}

// set all leds with one command
void all_leds (uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
  for (int i=0;i!=PIXEL_COUNT;i++) {
      strip.setPixelColor(i, strip.Color(r*brightness, g*brightness, b*brightness,w*brightness));
  }
  strip.show();
}

void single_led (uint8_t pin, uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
  strip.setPixelColor(pin, strip.Color(r*brightness, g*brightness, b*brightness,w*brightness));
  strip.show();
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

// rainbow mode 
void rainbowCycle (uint8_t wait) {
  uint16_t i, j;
  Serial.println("Vor loop");
  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));

      if (selected_mode != MODE_RAINBOW) return;
      Blynk.run();   
    }
    strip.show();
    //delay(wait);
    Blynk.run();   
  }
  Serial.println("Nach loop");
}

void setPixelHeatColor (int Pixel, byte temperature) {
  // Scale 'heat' down from 0-255 to 0-191
  byte t192 = round((temperature/255.0)*191);
 
  // calculate ramp up from
  byte heatramp = t192 & 0x3F; // 0..63
  heatramp <<= 2; // scale up to 0..252
 
  // figure out which third of the spectrum we're in:
  if( t192 > 0x80) {                     // hottest
    strip.setPixelColor(Pixel, 255, 255, heatramp);
  } else if( t192 > 0x40 ) {             // middle
    strip.setPixelColor(Pixel, 255, heatramp, 0);
  } else {                               // coolest
    strip.setPixelColor(Pixel, heatramp, 0, 0);
  }
}

void Fire(int Cooling, int Sparking, int SpeedDelay) {
  static byte heat[PIXEL_COUNT];
  int cooldown;
  
  // Step 1.  Cool down every cell a little
  for( int i = 0; i < PIXEL_COUNT; i++) {
    cooldown = random(0, ((Cooling * 10) / PIXEL_COUNT) + 2);
    
    if(cooldown>heat[i]) {
      heat[i]=0;
    } else {
      heat[i]=heat[i]-cooldown;
    }
  }
  
  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for( int k= PIXEL_COUNT - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }
    
  // Step 3.  Randomly ignite new 'sparks' near the bottom
  if( random(255) < Sparking ) {
    int y = random(7);
    heat[y] = heat[y] + random(160,255);
    //heat[y] = random(160,255);
  }

  // Step 4.  Convert heat to LED colors
  for( int j = 0; j < PIXEL_COUNT; j++) {
    setPixelHeatColor(j, heat[j] );
  }

  strip.show();
  //delay(SpeedDelay);
}
