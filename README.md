# **Arduino OSC library for GrandMA3 consoles**
An object orientated library for Arduino to control GrandMA3 consoles with OSC over Ethernet UDP. The goal of the library is to have a smart toolbox to create your own hardware which covers your needing by endless combinations of hardware elements.

The library depends on:
- the OSC library from CNMAT https://github.com/CNMAT/OSC

# Ethernet Usage

The in the Arduino examples used Ethernet library only supports the Wiznet 5500 chip, used on Ethernet Shield 2 or the popular USR-ES1 module which you can buy for a small pice at aliexpress.com

**Following libraries must downloaded for use with Ethernet
!!! Beware, the Ethernet libraries have different init procedures !!!**

**WIZNet w5500 boards like Ethernet Shield 2**
- an Arduino compatible Ethernet library like Ethernet3 https://github.com/sstaub/Ethernet3
- optional for Teensy MAC address https://github.com/sstaub/TeensyID

**Teensy 4.1 with buildin Ethernet**
- https://github.com/vjmuzik/NativeEthernet
- https://github.com/vjmuzik/FNET

**STM32duino (https://github.com/stm32duino)**
- https://github.com/stm32duino/STM32Ethernet
- https://github.com/stm32duino/LwIP

## Plans for Future versions:
- general library addition depends of the extensions done by the software engineers of MA Lighting
- TCP is not possible in the moment, because there is no further development of the original CNMAT library. Maybe I use my own OSC library for sending data.
- parser for echo replays to get usefull informations, e.g. cue name ...
- library for MBED

# Installation
1. Download from Releases
2. Follow the instruction on the Arduino website https://www.arduino.cc/en/Guide/Libraries
You can import the .zip file from the IDE with *Sketch / Include Library / Add .ZIP Library...*
3. Install the OSC library from CNMAT
4. For PlatformIO Unzip and move the folder to the lib folder of your project.

# Hardware
The library support hardware elements like encoders, faders, buttons with some helper functions. The library allows you to use hardware elements as an object and with the use of the helper functions, code becomes much easier to write and read and to understand.
- **Buttons**
	You can use every momentary push button on the market, e.g. MX Keys which are also used by MA Lighting, the keys are available with different push characters and have therefore different color markers. One pin must connect to a Digital Pin D*x* the other to ground.<br>
	**! A 100nF capitor is recommanded between the button pins !**<br>
- **Faders**
  Recommanded are linear faders with 10k Ohm from **Bourns** or **ALPS** which are available in different lengths and qualities.
	Beware that ARM boards like STM32-Nucleo use 3.3V, classic AVR boards like Arduino UNO use 5V. The leveler must connect to the Analog Pin A*x*. The other pin must connect to ground and 3.3/5 V depending on hardware alignment.<br>
	**! A 10nF capitor is recommanded between leveler and ground !**<br>

- **Rotary Encoders**
  You can use encoders from **ALPS** or equivalent.
	The middle pin of the encoders must connect to ground, the both other pins A/B must connect to Digital Pins D*x*.<br>
  **! Two 100nF capitors are recommanded between the button pin A/B and ground !**<br>

## Additional Advices for Analog Pins
- **Arduino UNO, MEGA with WIZnet5500**
Use IOREF Pin instead +5V to the top (single pin) of the fader (100%).
GND to the center button pin (2 pins, the outer pin is normaly for the leveler) of the fader (0%)

- **STM32-Nucleo**
use IOREF Pin instead +3.3V to the top (single pin) of the fader (100%).
GND to the center button pin (2 pins, the outer pin is normaly for the leveler) of the fader (0%).

- **TEENSY 3.x with WIZnet5500**
+3.3V to the top (single pin) of the fader (100%)
use ANALOG GND instead the normal GND to the center button pin (2 pins, the outer pin is normaly for the leveler) of the fader (0%).

![Development for testing using a breadboard](https://github.com/sstaub/gma3/blob/master/images/gma3_nucleo.png?raw=true)

# Usage 

Please refer to the GrandMA3 manual for more information about using OSC on GrandMA3.
You can find general information about OSC on http://opensoundcontrol.org/introduction-osc and https://github.com/CNMAT/OSC

Here an example OSC setup
![GrandMA3 OSC Setup](https://github.com/sstaub/gma3/blob/master/images/gma3_osc_setup.png?raw=true)

For use with PlatformIO https://platformio.org, as a recommanded IDE with MS VSCode, there is an extra start example folder called **gma3**.
![Development on PlatformIO](https://github.com/sstaub/gma3/blob/master/images/gma3_development.png?raw=true)

If you have whishes for other functions or classes enter the discussion forum. If you find bugs make an issue, nobody is perfect.

## Ethernet configuration and initialization
The Ethernet functionality is now independent from the hardware port (e.g. WIFI or other Ethernet hardware than WizNet W5500) and libraries. Behind the scenes it uses the virtual Arduino UDP class.

Before using Ethernet there a some things that must be done. It can be different between the diverse libraries.
1. Include the necessary #defines e.g.
- Arduino or Teensy 3.x with WIZnet5500
```
#include "Ethernet3.h"
#include "EthernetUdp3.h"
```

- STM32-Nucleo (e.g. Nucleo-F767ZI)
```
#include <LwIP.h>
#include <STM32Ethernet.h>
#include <EthernetUdp.h>
```

- Teensy 4.1
```
#include <TeensyID.h>
#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>
```

1. You need to define IP addresses and ports 

- **mac** - You need a unique MAC address, for Teensy 3.x / 4.1 you can use the TeensyID library on this GitHub site, for STM32-Nucleo there is a build in MAC address
- **localIP** - You need a static IP address for your Arduino in the subnet range of network system
- **subnet** - A subnet range is necessary
- **localPort** - This is the destinitaion port of your Arduino
- **gma3IP** - This is the GrandMA3 console IP address
- **gma3Port** - This is the destination port of the GrandMA3 console


```cpp
// configuration example, must done before setup()
uint8_t mac[] = {0x90, 0xA2, 0xDA, 0x10, 0x14, 0x48}; // MAC Address only needed for WIZNet5500 chips
// for Teensy this can empty: uint8_t mac[6];

#define GMA3_UDP_PORT  8000 // UDP Port configured in gma3
IPAddress localIP(10, 101, 1, 201); // IP address of the microcontroller board
IPAddress subnet(255, 255, 0, 0); // subnet range
uint16_t localUdpPort = GMA3_UDP_PORT;
IPAddress gma3IP(10, 101, 1, 100); // IP address of the gma3 console
uint16_t gma3UdpPort = GMA3_UDP_PORT;
```
1. You need an UDP constructor, must done before setup().
```cpp
EthernetUDP udp;
```
4. In the beginning of setup() you must start network services.
```cpp
// for Teensy call: teensyMAC(mac);
Ethernet.begin(mac, localIP, subnet);
// for STM32 use: Ethernet.begin(localIP, subnet);
udp.begin(localPort);
interface(udp, gma3IP, gma3UdpPort);
```

# Examples
There are some basic examples for for different board types using the Arduino IDE.
- gma3_Arduino<br>
.ino file for Arduino boards like UNO or MEGA with an EthernetShield2
- gma3_STM32<br>
.ino file for STM32-Nucleo boards with buildIn Ethernet using STM32duino
- gma3_Teensy41<br>
.ino file for Teensy 4.1 board buildIn Ethernet
- gma3_STM32_PIO<br>
Is a project folder for use with PlatformIO and includes the Example code for STM32-Nucleo-F767ZI as a starting point.

# Helper functions

## **Interface**
```
void interface(UDP &gma3Udp, IPAddress gam3IP, uint16_t gma3Port = 8000);
```
Interface is needed for initialize the connection to the GrandMA3 console.
Following settings must done:
- name of the UDP class member
- IP Address of the GrandMA3 console
- OSC Port, set in the GrandMA3 console, standard is port 8000

```cpp
interface(udp, gma3IP, gma3UdpPort);
```

## Prefix
```
void setPrefix(const char prefix[]);
```
**setPrefix()** allows you to use a Prefix Name, given in the GrandMA3 software.
- same Prefix String as configured in the console 

```cpp
setPrefix("gma3");
```

## Page Name
```
void setPageName(const char prefix[]);
```
**setPageName()** allows you to use an alternative Page Name, given in the GrandMA3 software.
- same Page String as configured in the console 

```cpp
setPageName("Seite");
```

## Fader Name
```
void setFaderName(const char prefix[]);
```
**setFaderName()** allows you to use an alternative Fader Name, given in the GrandMA3 software.
- same Fader String as configured in the console 

```cpp
setFaderName("Regler");
```

## ExecutorKnob Name
```
void setExecutorKnobName(const char prefix[]);
```
**setExecutorKnobName()** allows you to use an alternative ExecutorKnob Name, given in the GrandMA3 software.
- same ExecutorKnob String as configured in the console 

```cpp
setExecutorKnobName("Drehregler");
```

## Send OSC message
For sending the OSC message, only for internal usage.
```
void send(OSCMessage& msg);
void send(OSCMessage& msg, IPAddress ip, uint16_t port);
```
Send an OSC Message, depending on the choosen interface.
Example, this function can called inside setup() or loop() after init the interface.
```
send(message);
send(message, ip, port);
```

# Classes

## **Key**
With this class you can create Key objects which can be triggered with a button.
```
Key(uint8_t pin, uint16_t page, uint16_t key);
```
- **pin** are the connection Pin for the button hardware
- **page** is the page number of the executors, refer to the GrandMA3 manual
- **key** is the key number of the executors, refer to the GrandMA3 manual

Example, this should done before the setup()
```
Key key201(A1, 1, 201);
// executor button 201 on page 1 on pin A1
```
To get the actual button state you must call inside the loop():
```
void update();
```
Example, this must happen in the loop() 
```
key201.update();
```

## **Fader**
This class allows you to control a fader containing  with a hardware (slide) potentiometer as an executor fader. 
```
Fader(uint8_t analogPin, uint16_t page, uint16_t fader);
```
- **analogPin** are the connection Analog Pin for the fader leveler
- **page** is the page number of the executors, refer to the GrandMA3 manual
- **fader** is the fader number of the executors, refer to the GrandMA3 manual

Example, this should done before the setup()
```
Fader fader201(A0, 1, 201);
// leveler is Analog Pin A0, page is 1 and executor number of the fader 201
```
To get the actual button state you must call inside the loop():
```
void update();
```
Example, this must happen in the loop()
```
fader201.update();
```

## **ExecutorKnob**
The ExecutorKnob class creates an encoder object which allows to control the executor knobs:
```
ExecutorKnob(uint8_t pinA, uint8_t pinB, uint16_t page, uint16_t executorKnob, uint8_t direction = FORWARD);
```
- **pinA** and **pinB** are the connection Pins for the encoder hardware
- **page** is the page number of the executors, refer to the GrandMA3 manual
- **executorKnob** is the number of the executor knob, refer to the GrandMA3 manual
- **direction** is used for changing the direction of the encoder to clockwise if pinA and pinB are swapped. The directions are FORWARD (standard) or REVERSE

Example, this should done before the setup()
```
ExecutorKnob enc301(A3, A4, 1, 301, REVERSE); 
// the encoder pins are A3/A4, page is 1, the number of the executorKnob is 301, encoder pins are swapped (REVERSE)
```

To get the actual encoder state you must call inside the loop():
```
void update();
```
Example, this must happen in the loop() 
```
enc301.update();
```

## **CmdButton**
With this class you can create Keya button which allows to send commands to the console.
```
CmdButton(uint8_t pin, const char command[]);
```
- **pin** are the connection Pin for the button hardware
- **command** is a command string which should send to the console, refer also to the GrandMA3 manual


Example, this should done before the setup()
```
CmdButton macro1(A2, "GO+ Macro 1");
// button on pin A2, fires Macro 1
```
To get the actual button state you must call inside the loop():
```
void update();
```
Example, this must happen in the loop() 
```
macro1.update();
```

## **OscButton**
With this class you can create generic buttons which allows you to control other OSC compatible software in the network like QLab. The class initializer is overloaded to allow sending different OSC data types: Interger 32 bit, Float, Strings or no data.
**! When using Interger or Float, 0 or 0.0 is send when releasing the button !**

For Ethernet UDP
```
OscButton(uint8_t pin, const char pattern[], int32_t integer32, IPAddress ip, uint16_t port);
OscButton(uint8_t pin, const char pattern[], float float32, IPAddress ip, uint16_t port);
OscButton(uint8_t pin, const char pattern[], const char message[], IPAddress ip, uint16_t port);
OscButton(uint8_t pin, const char pattern[], IPAddress ip, uint16_t port);
```
- **pin** the connection Pin for the button hardware
- **pattern** the OSC address pattern
- **integer32** optional Integer data to send, you must cast this data e.g. ```(int32_t)1```
- **float32** optional Float data to send, you must cast this data e.g. ```1.0f```
- **message** optional String to send
- **ip** optional destination IP address
- **port** optional destination port address

Example for Ethernet UDP using a button on Pin 0, this should done before the setup()
```
#define QLAB_GO_PIN 0
IPAddress qlabIP(10, 101, 1, 101); // IP of QLab
uint16_t qlabPort = 53000; // QLab receive port
OscButton qlabGo(QLAB_GO_PIN , "/go", qlabIP, qlabPort);
```
To get the actual button state you must call inside the loop():
```
void update();
```
Example, this must happen in the loop() 
```
qlabGo.update();
```
