# PCB-illustration

The project *PCBi (PCB-illustration)* is a private project to illustrate PCBs I made in the past as a hardware designer. 
The illustration uses a self built LED-Panel. It is built from an old LCD-TV using the plexiglass and blurred-plate to diffuse the light smooth behind the PCBs.
 
##  Electronic setup

I use the following components:

- esp8266 (Wemos D1 mini)
- Grove Touch slider sensor (CY8C4014LQI)
- DHT22 temperature and humidity sensor
- Pololu Logic Level Shifter
- Pololu Low-Side MOSFET
- 5m sk6812 led strip (RGBW)

## Software

The esp8266 is entirely programmed using the Arduino IDE and uses the following third-party libraries:
- FastLED for sk6812 (https://www.partsnotincluded.com/neopixels/fastled-rgbw-neopixels-sk6812/)
- IRremoteESP8266
- DHT for Arduino

To use the esp8266 I needed to downgrade the esp library to version 0.5.4 (blynk)
To control the setup with my smartphone I use Blynk. 


## PCBi Functionality

- [x] :alarm_clock: Timer based LED mode scheduler (effects) without delay
- [x] :bar_chart: DHT22 temperature and humidity streaming
- [x] :electric_plug: MOSFET based on of switch for LED strip to reduce power consumption
- [x] :link: ir remote control using common rgb remote from ebay
- [ ] :clap: Double clap detection to control the panel 
- [ ] :musical_note: Music controlled LED strip using fft 



 ## Upload software
 The Panel has a USB Interface on the downside. Use the following settings :
 ![Alt text](board.png?raw=true "Settings")

