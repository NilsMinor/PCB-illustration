
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
#include <TimeLib.h>
#include <WidgetRTC.h>

#include "Seeed_CY8C401XX.h"    // Grove touch sensor
#include "DHT.h"                // DHT22 temperature and humidity sensor

// DHT 22 setting
#define DHTPIN 13                // what digital pin we're connected to
#define DHTTYPE DHT22           // DHT 22  (AM2302), AM2321

// LED strip related configs
#define PIXEL_PIN   12          // Wemos D6 
#define PIXEL_COUNT 180         // Pixels Count
#define MODE_MAX    10          

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRBW + NEO_KHZ800);

CY8C touch;
DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;


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
void nextMode (void);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  Blynk.begin(auth, wlan_name, wlan_pw);

  strip.begin();
  dht.begin();
  touch.init();

  connectWLANIndicator ();

  red = green = blue = 0;

  timer.setInterval(1000L, sendTemperatureHumidity); // Setup a function to be called every second
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
  nextMode ();
}

BLYNK_WRITE(V5) {
  red = param [0].asInt ();
  green = param [1].asInt ();
  blue = param [2].asInt ();    
}

//----------------------------------------------------------------------------------------------------

void loop() {
  
  if (on_off) 
    //all_leds (0,0,0,255);
    all_leds(red,green,blue,0);
  else 
    all_off ();
    
    handle_touch ();

    //delay(10);
    Blynk.run();
    timer.run();
}

void all_off (void) {
   all_leds (0,0,0,0);
}

// Connect to WLAN, wait while not conected
void connectWLANIndicator (void) {
  
  all_off ();

  // wait for wlan connection
  Serial.print("Connecting");
  single_led (10,0,0,200,0);
  while (Blynk.connect() == false) { 
    Serial.print(".");
    delay(100); 
  } 
  Serial.println();

  Serial.print("connected");
  all_off ();
}

// handle next mode (step) event
void nextMode (void) {
  selected_mode = ( selected_mode + 1 ) % MODE_MAX;
  
  Serial.print("Mode : ");
  Serial.println(selected_mode);
  
  Blynk.virtualWrite(V3, selected_mode); 
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
        // toggle on of state
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
  
        nextMode ( );
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
