# **Arduino OSC library for GrandMA3 consoles v2**
An object orientated library for Arduino to control GrandMA3 consoles with OSC over Ethernet UDP/TCP. The goal of the library is to have a smart toolbox to create your own hardware which covers your needing by endless combinations of hardware elements.

## Changes for v2
- TCP support
- UDP parser for echo replays to get usefull informations, e.g. fader value ...
- independent from the CNNMAT OSC library
- change of naming conventions
- change license to CC BY-NC-SA 4.0

# Installation
1. Download from Releases
2. Follow the instruction on the Arduino website https://www.arduino.cc/en/Guide/Libraries
You can import the .zip file from the IDE with *Sketch / Include Library / Add .ZIP Library...*
3. For PlatformIO Unzip and move the folder to the lib folder of your project.

# Boards
You can use any Arduino compatible board which gives you the possibility of an ethernet port. Some boards like Teensy or STM32 Nucleo-F767ZI have an Ethernet port build in, others need an external ethernet port based e.g. on Wiznet W5500 boards like USR-ES1 or Arduino EthernetShield 2. 
WLAN boards like ESP32 should work but are not tested and there is no guarantee for a stable connection. 
There are also some Arduino based SPS controllers on the market which are ideal for rough enviroment using 24V.

- Controllino https://www.controllino.com
- Industrial Shields https://www.industrialshields.com

# Ethernet Usage
The in the Arduino board example used Ethernet library only supports the Wiznet 5500 chip, used on Ethernet Shield 2 or the popular USR-ES1 module which you can buy for a small pice at aliexpress.com

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

![Development for testing using a breadboard](https://github.com/sstaub/gma3/blob/master/images/gma3_teensy41.png?raw=true)
Test configuartion with Teensy41
# Usage 
You can find general information about OSC on http://opensoundcontrol.org/
Please refer to the GrandMA3 manual for more information about using OSC on GrandMA3.

Here an example OSC setup in the GrandMA3 software
![GrandMA3 OSC Setup](https://github.com/sstaub/gma3/blob/master/images/gma3_osc_setup.png?raw=true)

For use with PlatformIO https://platformio.org, as a recommanded IDE with MS VSCode, there is an extra start example folder called **gma3**.
![Development on PlatformIO](https://github.com/sstaub/gma3/blob/master/images/gma3_development.png?raw=true)

If you have wishes for other functions or classes enter the discussion forum. If you find bugs make an issue, nobody is perfect.

## Ethernet configuration and initialization
The Ethernet functionality is now independent from the hardware port (e.g. WIFI or other Ethernet hardware than WizNet W5500) and libraries. Behind the scenes it uses the virtual Arduino UDP class.

Before using Ethernet there a some things that must be done. It can be different between the diverse libraries.
1. Include the necessary #defines e.g.
- Arduino or Teensy 3.x with WIZnet5500
```cpp
#include "Ethernet3.h"
#include "EthernetUdp3.h"
```

- STM32-Nucleo (e.g. Nucleo-F767ZI)
```cpp
#include <LwIP.h>
#include <STM32Ethernet.h>
#include <EthernetUdp.h>
```

- Teensy 4.1 with FNET network stack
```cpp
#include <TeensyID.h>
#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>
```

2. You need to define IP addresses, ports and sockets 

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

#define GMA3_UDP_PORT 8000 // UDP Port configured in gma3
#define GMA3_TCP_PORT 9000 // TCP Port configured in gma3
IPAddress localIP(10, 101, 1, 201); // IP address of the microcontroller board
IPAddress subnet(255, 255, 0, 0); // subnet range
uint16_t localUdpPort = GMA3_UDP_PORT;
IPAddress gma3IP(10, 101, 1, 100); // IP address of the gma3 console
uint16_t gma3UdpPort = GMA3_UDP_PORT;
uint16_t gma3TcpPort = GMA3_TCP_PORT;
```
3. You need an UDP/TCP socket, must done before setup().
```cpp
EthernetUDP udp;
EthernetClient tcp;
```
4. In the beginning of setup() you must start network services.
```cpp
// for Teensy call: teensyMAC(mac);
Ethernet.begin(mac, localIP, subnet);
// for STM32 use: Ethernet.begin(localIP, subnet);
interfaceGMA3(gma3IP);
interfaceUDP(udp, gma3UdpPort);
interfaceTCP(tcp, gma3TcpPort);
```

# Example
A simple example for use with an Arduino UNO with EthernetShield 2
```cpp
#include <Ethernet3.h>
#include <EthernetUdp3.h>
#include "gma3.h"

#define BTN_KEY 2
#define ENC_1_A 3
#define ENC_1_B 4
#define BTN_CMD 5
#define FADER   A0

// network data
uint8_t mac[] = {0x90, 0xA2, 0xDA, 0x10, 0x14, 0x48};
#define GMA3_UDP_PORT  8000 // UDP Port configured in gma3
#define GMA3_TCP_PORT  9000 // UDP Port configured in gma3

IPAddress localIP(10, 101, 1, 201); // IP address of the microcontroller board
IPAddress subnet(255, 255, 0, 0); // subnet range
IPAddress gma3IP(10, 101, 1, 100); // IP address of the gma3 console
uint16_t gma3UdpPort = GMA3_UDP_PORT;
uint16_t gma3TcpPort = GMA3_TCP_PORT;

EthernetUDP udp;
EthernetClient tcp;

// hardware definitions
Fader fader201(FADER, 1, 201);
Key key201(BTN_KEY, 1, 201, TCP); // using TCP
ExecutorKnob enc301(ENC_1_A, ENC_1_B, 1, 301);
CmdButton macro1(BTN_CMD, "GO+ Macro 1");

void setup() {
	Serial.begin(9600);
	Ethernet.begin(mac, localIP, subnet);
	interfaceGMA3(gma3IP);
	interfaceUDP(udp, gma3UdpPort);
	interfaceTCP(tcp, gma3TcpPort);
	}

void loop() {
	key201.update();
	fader201.update();
	enc301.update();
	macro1.update();
	}
```

## Examples folders
There are some basic examples for for different board types using the Arduino IDE.

## gma3_Arduino_PIO
Is a project folder for use with PlatformIO and includes an example code for an Arduino UNO as a starting point. It also include an extra button for a GO command to QLab.

# Setup functions

## **Interfaces**
The Interfaces are needed for initialize the connection to the GrandMA3 console and external receivers.
Following settings must done in ```setup()```:
- IP address of the GrandMA3 console
- name of the UDP class member
- name of the TCP class member
- IP Address of the GrandMA3 console
- OSC Port, set in the GrandMA3 console, standard for UDP is port 8000, for TCP 9000
- IP addresses and ports for external receivers using ```oscButton()``` function

### interfaceGMA3()
```cpp
void interfaceGMA3(IPAddress gma3IP)
```
Set the IP address of the GrandMA3 software

### interfaceUDP()
```cpp
void interfaceUDP(UDP &udp, uint16_t port = 8000)
```
Set the UDP socket and port of the GrandMA3 software, standard UDP port is 8000

### interfaceTCP()
```cpp
void interfaceTCP(UDP &tcp, uint16_t port = 9000)
```
Set the TCP socket and port of the GrandMA3 software, standard TCP port is 9000

### interfaceExternUDP()
```cpp
void interfaceExternUDP(UDP &udp, uint16_t port)
```
Set the UDP socket and port external receivers accessing via ```oscButton()``` function

### interfaceExternTCP()
```cpp
void interfaceExternTCP(UDP &tcp, uint16_t port)
```
Set the TCP socket and port external receivers accessing via ```oscButton()``` function

**Examples**
```cpp
interfaceGMA3(gma3IP);
interfaceUDP(udp, gma3UdpPort);
interfaceTCP(tcp, gma3TcpPort);
interfaceExternUDP(udpQLab, qlabIP, qlabPort);
interfaceExternTCP(tcpQLab, qlabIP, qlabPort);
```

## GrandMA3 naming conventions
This has changed from v1.x to avoid performance problems.
Naming of prefix and others are done in the gma3.h file.
The standard prefix is **gma3** now, when changing the prefix you should also change the PREFIX_PATTERN, this is needed for echo replay.
The naming must the same as in the GrandMA3 software
```cpp
// GMA3 naming conventions
#define PREFIX "gma3"
#define PREFIX_PATTERN "/gma3/"
#define PAGE "Page"
#define FADER "Fader"
#define EXECUTOR_KNOB "Encoder"
#define KEY "Key"
```

## Transport modes
- **UDPOSC** standard mode using UDP protocol
- **TCP** for OSC 1.0 supported by GrandMA3 using TCP
- **TCPSLIP** for OSC 1.1 e.g. for QLab TCP with SLIP encoding

# Classes

## **Key**
With this class you can create Key objects which can be triggered with a button.
```cpp
Key(uint8_t pin, uint16_t page, uint16_t key, protocol_t protocol = UDPOSC);
```
- ```pin``` are the connection Pin for the button hardware
- ```page``` is the page number of the executors, refer to the GrandMA3 manual
- ```key``` is the key number of the executors, refer to the GrandMA3 manual
- ```protocol``` is the transport protocol you want to use, it can UDPOSC and TCP

Example, this should done before the setup()
```cpp
Key key201(2, 1, 201, TCP);
// executor button 201 on page 1, using pin 2, transport with TCP
```
To get the actual button state you must call inside the ```loop()``` function
```cpp
void update();
```
Example, this must happen in the ```loop()``` function
```cpp
key201.update();
```

## **Fader**
This class allows you to control a fader containing  with a hardware (slide) potentiometer as an executor fader. 
```cpp
Fader(uint8_t analogPin, uint16_t page, uint16_t fader, protocol_t protocol = UDPOSC);
```
- ```analogPin``` are the connection Analog Pin for the fader leveler
- ```page``` is the page number of the executors, refer to the GrandMA3 manual
- ```fader``` is the fader number of the executors, refer to the GrandMA3 manual
- ```protocol``` is the transport the protocol you want to use, it can UDPOSC and TCP, UDPOSC is recommanded

Example, this should done before the setup()
```cpp
Fader fader201(A0, 1, 201);
// leveler is Analog Pin A0, page is 1 and executor number of the fader 201
```
To get the actual button state you must call inside the ```loop()``` function
```cpp
void update();
```
Example, this must happen in the ```loop()``` function
```cpp
fader201.update();
```

## **ExecutorKnob**
The ExecutorKnob class creates an encoder object which allows to control the executor knobs:
```cpp
ExecutorKnob(uint8_t pinA, uint8_t pinB, uint16_t page, uint16_t executorKnob, protocol_t protocol = UDPOSC, uint8_t direction = FORWARD);
```
- ```pinA``` and **pinB** are the connection Pins for the encoder hardware
- ```page``` is the page number of the executors, refer to the GrandMA3 manual
- ```executorKnob``` is the number of the executor knob, refer to the GrandMA3 manual
- ```protocol``` is the transport protocol you want to use, it can UDPOSC and TCP
- ```direction``` is used for changing the direction of the encoder to clockwise if pinA and pinB are swapped. The directions are FORWARD (standard) or REVERSE

Example, this should done before the setup()
```cpp
ExecutorKnob enc301(3, 4, 1, 301, UDPOSC, REVERSE); 
// the encoder pins are 3/4, page is 1, the number of the executorKnob is 301, send with UDP, encoder pins are swapped (REVERSE)
```

To get the actual encoder state you must call inside the ```loop()``` function
```cpp
void update();
```
Example, this must happen in the ```loop()``` function
```cpp
enc301.update();
```

## **CmdButton**
With this class you can create Keya button which allows to send commands to the console.
```cpp
CmdButton(uint8_t pin, const char command[], protocol_t protocol = UDPOSC);
```
- ```pin``` are the connection Pin for the button hardware
- ```command``` is a command string which should send to the console, refer also to the GrandMA3 manual
- ```protocol``` is the transport protocol you want to use, it can UDPOSC and TCP


Example, this should done before the setup()
```cpp
CmdButton macro1(A2, "GO+ Macro 1");
// button on pin A2, fires Macro 1
```
To get the actual button state you must call inside the ```loop()``` function
```cpp
void update();
```
Example, this must happen in the ```loop()``` function
```cpp
macro1.update();
```

## **OscButton**
With this class you can create generic buttons which allows you to control other OSC compatible software in the network like QLab. The class initializer is overloaded to allow sending different OSC data types: Interger 32 bit, Float, Strings or no data.
**! When using interger or float, 0 or 0.0 is explicit send when releasing the button !**

For Ethernet UDP
```cpp
OscButton(uint8_t pin, const char pattern[], int32_t integer32, protocol_t protocol = UDPOSC);
OscButton(uint8_t pin, const char pattern[], float float32, protocol_t protocol = UDPOSC);
OscButton(uint8_t pin, const char pattern[], const char message[], protocol_t protocol = UDPOSC);
OscButton(uint8_t pin, const char pattern[], protocol_t protocol = UDPOSC);
```
- ```pin``` the connection Pin for the button hardware
- ```pattern``` the OSC address pattern
- ```integer32``` optional Integer data to send, you must cast this data e.g. ```(int32_t)1```
- ```float32``` optional Float data to send, you must cast this data e.g. ```1.0f```
- ```message``` optional String to send
- ```protocol``` is the transport protocol you want to use, it can UDPOSC, TCP and TCPSLIP


Example for Ethernet UDP using a button on Pin 0, this should done before the setup()
```cpp
#define QLAB_GO_PIN 0
IPAddress qlabIP(10, 101, 1, 101); // IP of QLab
uint16_t qlabPort = 53000; // QLab receive port
OscButton qlabGo(QLAB_GO_PIN , "/go", qlabIP, qlabPort);
```
To get the actual button state you must call inside the ```loop()``` function
```cpp
void update();
```
Example, this must happen in the ```loop()``` function
```cpp
qlabGo.update();
```

## Send OSC message manually
Two steps are needed for send OSC messages manually:
1. Create an OSC message using
```cpp
void oscMessage(const char pattern[], int32_t int32, protocol_t protocol = UDPOSC);
void oscMessage(const char pattern[], float float32, protocol_t protocol = UDPOSC);
void oscMessage(const char pattern[], const char string[], protocol_t protocol = UDPOSC);
void oscMessage(const char pattern[], protocol_t protocol = UDPOSC);
```
- ```const char pattern[]``` is the OSC pattern 
- ```int32_t int32``` for interger data
- ```float float32``` for float date
- ```const char string[]``` for strings
- it is also possible to send no argument, e.g. for QLab Go cmd
- ```protocol_t protocol``` is the protocol type UDPOSC, TCP or TCPSLIP

1. Send an OSC Message, depending on the choosen interface.
This functions can called inside setup() or loop() after init the interface.
```cpp
void sendUDP();
void sendExternUDP();
void sendTCP();
void sendExternTCP();
```

## Receive UDP data with ```receiveUdp()```
You can receive the data send by the GrandMA software. Only UDP receive is supported.
With the ```receiveUdp()``` function you get the data which are automatic parsed to seperate the OSC data.
```cpp
bool receiveUDP(); // returns true if there is data present
```

The OSC data consists of the OSC pattern and max. 3 arguments,
1 String, up to two Integers and one Float argument
To get the OSC data inside you can use following functions:

```cpp
patternOSC() // returns the pattern string
stringOSC() // returns the string argument which the command
int1OSC() // returns the 1. integer argument
int2OSC() // returns the 2. integer argument if available
floatOSC() // returns the float argument if available
```

**Example**
This must happen in the ```loop()``` function
```cpp
if (receiveUDP()) {
	Serial.print("OSC Pattern: ");
	Serial.print(patternOSC());
	Serial.print(" String: ");
	Serial.print(stringOSC());
	Serial.print(" Integer 1: ");
	Serial.print(int1OSC());
	Serial.print(" Integer 2: ");
	Serial.print(int2OSC());
	Serial.print(" Float: ");
	Serial.println(floatOSC());
	}
```

Exapmle Outputs are
```
OSC Pattern: /gma3/13.13.1.5.2 String: Go+ Integer 1: 1 Integer 2: 0 Float: 0.00
OSC Pattern: /gma3/13.13.1.5.2 String: Go+ Integer 1: 0 Integer 2: 0 Float: 0.00
OSC Pattern: /gma3/13.13.1.5.2 String: FaderMaster Integer 1: 1 Integer 2: 0 Float: 100.00
```
The OSC pattern data is very cryptic because of the representation of the internal structure which there is no real docuentation.