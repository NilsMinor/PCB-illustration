# PCB-illustration

The project *PCBi (PCB-illustration)* is a private project to illustrate PCBs I made in the past. The illustration uses a self built LED-Panel. It is built from an old LCD-TV using the plexiglass and blurred-plate to diffuse the light smooth behind the PCBs.
 

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
- A
- B
- C

To use the esp8266 I needed to downgrade the esp library to version XXXX
To control the setup with my smartphone I use Blynk. 


## PCBi Functionality

Thinks I plan to implement: 

- [ ] :alarm_clock: Timer based LED mode scheduler (effects) without delay
- [ ] :bar_chart: DHT22 temperature and humidity streaming
- [ ] :electric_plug: MOSFET based on of switch for LED strip to reduce power consumption
- [ ] :clap: Double clap detection to control the panel 
- [ ] :musical_note: Music controlled LED strip using fft 
- [ ] :link: ir remote control using common rgb remote from ebay


 
