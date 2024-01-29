# **Arduino OSC library for GrandMA3 consoles v4**
An object orientated library for Arduino to control GrandMA3 consoles with OSC over Ethernet UDP/TCP. The goal of the library is to have a smart toolbox to create your own hardware which covers your needing by endless combinations of hardware elements.

### Changes for v4
- support for DataPools
- virtual inputs for touch screens and external I/O devices
- Pools and Pages classes instead of button class
- minor improvements for pages and pools functions
- new Parser class which support also TCP receive
- simplified network configuration, you can use TCP or UDP
- no more support for extern OSC buttons, this should done with console macros

### Changes for v3
- global / local page() numbering
- command() function for sending raw commands
- fetch(), value() and jitter() functions for fader handling
- buttons with callback for e.g. paging

### Changes for v2
- TCP support
- UDP parser for echo replays to get useful informations, e.g. fader value ...
- independent from the CNNMAT OSC library
- change of naming conventions
- change license to CC BY-NC-SA 4.0

## Installation
You can install directly with the library manager of the Arduino IDE.
Simply search for "gma3"

## Boards
You can use any Arduino compatible board which gives you the possibility of an ethernet connection. Some boards like Teensy or STM32 Nucleo-F767ZI have an Ethernet port build in, others need an external ethernet port based e.g. on WIZnet W5500 boards like USR-ES1 or Arduino EthernetShield 2. 
WLAN boards like ESP32 should work but are not tested and there is no guarantee for a stable connection. 
There are also some Arduino based SPS controllers on the market which are ideal for rough environment using 24V.

- Controllino https://www.controllino.com
- Industrial Shields https://www.industrialshields.com

## Ethernet Usage
The in the Arduino board example used Ethernet library only supports the Wiznet 5500 chip, used on Ethernet Shield 2 or the popular USR-ES1 module which you can buy for a small pice at aliexpress.com

**Following libraries must downloaded for use with Ethernet**<br>
**!!! Beware, the Ethernet libraries have different init procedures !!!**

**WIZNet w5500 boards like Ethernet Shield 2**
- an Arduino compatible Ethernet libraries
    - Ethernet3 https://github.com/sstaub/Ethernet3
    - Ethernet https://github.com/arduino-libraries/Ethernet

**Teensy 4.1 with build-in Ethernet**

optional for Teensy MAC address https://github.com/sstaub/TeensyID

based on FNET
- https://github.com/vjmuzik/NativeEthernet
- https://github.com/vjmuzik/FNET

or LWIP based
- https://github.com/ssilverman/QNEthernet

**STM32duino (https://github.com/stm32duino)**
- https://github.com/stm32duino/STM32Ethernet
- https://github.com/stm32duino/LwIP

Following Nucleo boards are supported with Ethernet: F207ZG / F429ZI / F746ZG / F756ZG / F767ZI

## Hardware
The library support hardware elements like encoders, faders, buttons with some helper functions. The library allows you to use hardware elements as an object and with the use of the helper functions, code becomes much easier to write and read and to understand.
- **Buttons**
	You can use every momentary push button on the market, e.g. MX Keys which are also used by MA Lighting, the keys are available with different push characters and have therefore different color markers. One pin must connect to a Digital Pin D*n* the other to ground.<br>
	**! A 100nF capacitor is recommended between the button pins !**<br>
- **Faders**
  Recommended are linear faders with 10k Ohm from **Bourns** or **ALPS** which are available in different lengths and qualities.<br>
	Beware that ARM boards like STM32-Nucleo use 3.3V, classic AVR boards like Arduino UNO use 5V. The leveler must connect to the Analog Pin A*n*. The other pins must connect to ground and 3.3V or 5V.<br>
	**! A 10nF capacitor is recommended between leveler and ground !**<br>

- **Rotary Encoders**
  You can use encoders from **ALPS** or equivalent.
	The middle pin of the encoders must connect to ground, the both other pins A/B must connect to Digital Pins D*n*.<br>
  **! Two 100nF capacitors are recommended between the button pin A/B and ground !**<br>

### Additional Advices for Analog Pins
The most problems comes from bad grounding and cables that are to long,
on PCB's the shielding design is very important.

- **Arduino UNO, MEGA with WIZnet5500**
Use AREF Pin instead +5V to the top (single pin) of the fader (100%).
Use GND next to AREF and connect to the center button pin (2 pins, the outer pin is normally for the leveler) of the fader (0%)

- **STM32-Nucleo**
use IOREF Pin instead +3.3V to the top (single pin) of the fader (100%).
GND to the center button pin (2 pins, the outer pin is normally for the leveler) of the fader (0%).

- **TEENSY 3.x with WIZnet5500**
+3.3V to the top (single pin) of the fader (100%)
use ANALOG GND instead the normal GND to the center button pin (2 pins, the outer pin is normally for the leveler) of the fader (0%).

![Development for testing using a breadboard](https://github.com/sstaub/gma3/blob/main/images/gma3_teensy41.png?raw=true)
Test configuration with Teensy41

## Usage 
You can find general information about OSC on http://opensoundcontrol.org/
Please refer to the GrandMA3 manual for more information about using OSC on GrandMA3.

Here an example OSC setup in the GrandMA3 software
![GrandMA3 OSC Setup](https://github.com/sstaub/gma3/blob/main/images/gma3_osc_setup.png?raw=true)

For use with PlatformIO https://platformio.org, as a recommended IDE with MS VSCode, there is an extra start example folder called **gma3_Arduino_PIO**, you must add the needed libraries manually.
![Development on PlatformIO](https://github.com/sstaub/gma3/blob/main/images/gma3_development.png?raw=true)

If you have wishes for other functions or classes enter the discussion forum. If you find bugs make an issue, nobody is perfect.

### Ethernet configuration and initialization
The Ethernet functionality is now independent from the hardware port (e.g. WIFI or other Ethernet hardware than WizNet W5500) and libraries. Behind the scenes it uses the virtual Arduino UDP class.

Before using Ethernet there a some things that must be done. It can be different between the diverse libraries.
1. Include the necessary #defines e.g.
- Arduino or Teensy 3.x with WIZnet5500
```cpp
#include "Ethernet3.h"
#include "EthernetUdp3.h"
```
- or for the original Arduino Ethernet library
```cpp
#include "Ethernet.h"
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

- Teensy 4.1 with QNEthernet network stack
```cpp
#include <TeensyID.h>
#include <QNEthernet.h>
```

2. You need to define IP addresses, ports and sockets 

- **mac** - You need a unique MAC address, for Teensy 3.x / 4.1 you can use the TeensyID library on this GitHub site, for STM32-Nucleo there is a build in MAC address
- **localIP** - You need a static IP address for your Arduino in the subnet range of network system
- **dns** - DNS address is optional necessary
- **gateway** - A gateway range is optional necessary
- **subnet** - A subnet range is optional necessary
- **localPort** - This is the destination port of your Arduino
- **gma3IP** - This is the GrandMA3 console IP address
- **gma3Port** - This is the destination port of the GrandMA3 console


**Example** must done before ```setup()```
```cpp
// Network config
#define GMA3_UDP_PORT 8000 // UDP Port configured in gma3
#define GMA3_TCP_PORT 9000 // UDP Port configured in gma3

uint8_t mac[] = {0x90, 0xA2, 0xDA, 0x10, 0x14, 0x48};
IPAddress ip(10, 101, 1, 201); // IP address of the microcontroller board
IPAddress subnet(255, 255, 0, 0); // subnet range
IPAddress dns(10, 101, 1, 100); // DNS address of your device
IPAddress gateway(10, 101, 1, 100); // Gateway address of your device
uint16_t localUdpPort = GMA3_UDP_PORT;
IPAddress gma3IP(10, 101, 1, 100); // IP address of the gma3 console
uint16_t gma3UdpPort = GMA3_UDP_PORT;
uint16_t gma3TcpPort = GMA3_TCP_PORT;
```
3. You need an UDP orTCP socket, must done before ```setup()```.
```cpp
EthernetUDP udp; // for UDP connection
EthernetClient tcp; // for TCP connection
```
4. In the beginning of ```setup()``` you must start network services.
```cpp
Ethernet.begin(mac, ip, dns, gateway, subnet); // for Arduino ETH library
interface(udp, gma3IP, gma3UdpPort); // for UDP
// interface(tcp, TCP, gma3IP, gma3TcpPort); // for TCP
```

## Example
A simple example for use with an Arduino UNO with EthernetShield 2
```cpp
#include <Ethernet.h>
#include "gma3.h"

// I/O config
#define BTN_KEY_1 2
#define BTN_KEY_2 3
#define BTN_KEY_3 4
#define BTN_KEY_4 5
#define ENC_1_A   6
#define ENC_1_B   7
#define ENC_2_A   8
#define ENC_2_B   9
#define BTN_CMD   10
#define FADER     A0

// Network config
#define GMA3_UDP_PORT 8000 // UDP Port configured in gma3
#define GMA3_TCP_PORT 9000 // UDP Port configured in gma3

uint8_t mac[] = {0x90, 0xA2, 0xDA, 0x10, 0x14, 0x48};
IPAddress localIp(10, 101, 1, 201); // IP address of the microcontroller board
IPAddress subnet(255, 255, 0, 0); // optional subnet range
IPAddress dns(10, 101, 1, 100); // optional DNS address of your device
IPAddress gateway(10, 101, 1, 100); // optional Gateway address of your device
uint16_t localUdpPort = GMA3_UDP_PORT;
IPAddress gma3IP(10, 101, 1, 100); // IP address of the gma3 console
uint16_t gma3UdpPort = GMA3_UDP_PORT;
uint16_t gma3TcpPort = GMA3_TCP_PORT;

// EthernetUDP udp; // for UDP
EthernetClient tcp; // for TCP

// hardware definitions
Key key101(BTN_KEY_1, 101);
Fader fader201(FADER, 201);
Key key201(BTN_KEY_2, 201);
Key key301(BTN_KEY_3, 301);
ExecutorKnob enc301(ENC_1_A, ENC_1_B, 301);
Key key401(BTN_KEY_4, 401);
ExecutorKnob enc401(ENC_2_A, ENC_2_B, 301);
CmdButton macro1(BTN_CMD, "GO+ Macro 1");

void setup() {
	Serial.begin(9600);
	Ethernet.begin(mac, localIp);
	//interface(udp, gma3IP, gma3UdpPort); // for UDP
	interface(tcp, TCP, gma3IP, gma3TcpPort); // for TCP
	}

void loop() {
	key101.update();
	key201.update();
	key301.update();
	key401.update();
	fader201.update();
	enc301.update();
	enc401.update();
	macro1.update();
	}
```

### Examples folders
There are some basic examples for different board types using the Arduino IDE.

## RAM usage adjustment
Because using strictly stack allocation of OSC strings,
you need to adjust the allocation size in the gma3.h file.
```cpp
// OSC settings
#define OSC_PATTERN_SIZE 64 // length depends on naming conventions
#define OSC_STRING_SIZE  64 // length depends on maximum command length
#define OSC_STRING_SIZE  64 // max. size of string arguments
#define OSC_MESSAGE_SIZE 128 // this should OSC_PATTERN_SIZE + OSC_STRING_SIZE
```

## Transport modes
- **UDPOSC** standard mode using UDP protocol
- **TCP** pure TCP without extra encoding like SLIP or length declaimer

## GrandMA3 naming conventions
The naming must the same as in the GrandMA3 software
```cpp
// GMA3 default naming conventions
char namePrefix[NAME_LENGTH_MAX] = "gma3";
char namePool[NAME_LENGTH_MAX] = "DataPool"; // Page name
char namePage[NAME_LENGTH_MAX] = "Page"; // Page name
char nameFader[NAME_LENGTH_MAX] = "Fader"; // Fader name
char nameExecutorKnob[NAME_LENGTH_MAX] = "Encoder"; // ExecutorKnob name
char nameKey[NAME_LENGTH_MAX] = "Key"; // Key name
```

The names can changed by following functions, this should done in ```setup()```
```cpp
void prefixName(const char *prefix);
void dataPoolName(const char *pool);
void pageName(const char *page);
void faderName(const char *fader);
void executorKnobName(const char *executorKnob);
void keyName(const char *key);
```

**Examples**
```cpp
prefixName("gma3");
dataPoolName("DataPool");
pageName("Page");
faderName("Fader");
executorKnobName("Encoder");
keyName("Key");
```

# Setup functions

## **Interfaces**
The Interfaces are needed for initialize the connection to the GrandMA3 console and external receivers.
Following settings must done in ```setup()```:
- IP address of the GrandMA3 console
- name of the UDP class member
- name of the TCP class member
- IP Address of the GrandMA3 console
- OSC Port, set in the GrandMA3 console, standard port for 
    - UDP is 8000
    - TCP is 9000

### interface()
```cpp
void interface(UDP &udp, IPAddress ip, uint16_t port = 8000);
```
- **UDP &udp** - UDP socket
- **IPAddress ip** - IP address of the console
- **uint16_t port = 8000** - UDP port of the GrandMA3 software, default UDP port is 8000

```cpp
void interface(Client &tcp, protocol_t protocol, IPAddress ip, uint16_t port = 9000);
```
- **Client &tcp** - TCP socket
- **protocol_t protocol** - Protocol Type, only TCP without special encoding is supported
- **IPAddress ip** - IP address of the console
- **uint16_t port = 9000** - TCP port of the GrandMA3 software, default UDP port is 9000

**Examples**
```cpp
interface(udp, gma3IP, gma3UdpPort);
interface(tcp, TCP, gma3IP, gma3TcpPort);
```
**You can only use TCP or UDP, not both at once!**

# Helper Functions

## Pool Number
You can change the Pool number common for all classes or for a single class member.

### Common Page Number
For changing the common page number use
```cpp
void poolCommon(uint16_t pool);
```
Default is **Pool 1**

Example
```cpp
poolCommon(2); // set common pool to 2
```

### Pool Number by class
Refer to the classes documentation

### Send Pool Number
You can change the pool number of the the console

```cpp
void sendPool(uint16_t pool);
```

**Example**
```cpp
poolSend(2); // set the console pool to 2
```

## Page Number
You can change the Page number common for all classes or for a single class member.

### Common Page Number
For changing the common page number use
```cpp
void pageCommon(uint16_t page);
```
Default is **Page 1**

**Example**
```cpp
pageCommon(2); // set common page to 2
```

### Page Number by class
Refer to the classes documentation

### Send Page Number
You can change the page number of the the console

```cpp
void sendPage(uint16_t page);
```

**Example**
```cpp
pageSend(2); // set the console page to 2
```

## **command()**
Send a command message
```cpp
void command(const char command[], protocol_t protocol = UDPOSC);
```

**Example**
```cpp
command("Page+");
```

# Classes

## **Parser**
This class parse the message send from the console 

### Constructor
```cpp
Parser(cbptr callback);
```
- **callback** callback pointer to a function which is called when a new message arrived

**Example**
```cpp
void parse() {
  Serial.print("OSC Pattern: ");
  Serial.print(parser.patternOSC());
  Serial.print(" String: ");
  Serial.print(parser.stringOSC());
  Serial.print(" Integer 1: ");
  Serial.print(parser.int1OSC());
  Serial.print(" Integer 2: ");
  Serial.print(parser.int2OSC());
  Serial.print(" Float: ");
  Serial.println(parser.floatOSC());
  }
Parser parser(parse);
```

The OSC data consists of the OSC pattern and max. 3 arguments,
1 String, up to two Integers and one Float argument
To get the OSC data inside you can use following class members:

```cpp
const char* patternOSC(); // returns the pattern string
int dataStructure(uint8_t level); // returns the parts of the internal data structure as an integer, the level can 0 thru 4
const char* stringOSC(); // returns the string argument which the command
int32_t int1OSC(); // returns the 1. integer argument
int32_t int2OSC(); // returns the 2. integer argument if available
float floatOSC(); // returns the float argument if available
```

Example Outputs are
```
OSC Pattern: 13.13.1.5.2 String: Go+ Integer 1: 1 Integer 2: 0 Float: 0.00
OSC Pattern: 13.13.1.5.2 String: Go+ Integer 1: 0 Integer 2: 0 Float: 0.00
OSC Pattern: 13.13.1.5.2 String: FaderMaster Integer 1: 1 Integer 2: 0 Float: 100.00
```
The OSC pattern data is very cryptic because of the representation of the internal structure which there is no real documentation.

### Update
To get the messages send by console you must call inside the ```loop()``` function
```cpp
void update();
```
Example, this must happen in the ```loop()``` function
```cpp
parser.update();
```

# Hardware Classes
It is now possible to use virtual devices like touchscreens e.g. from Nextion https://github.com/sstaub/NextionX2 or I/O expanders for analog (MCP3208 https://github.com/sstaub/MCP_3208) and digital (MCP32017 https://github.com/sstaub/MCP_23017) inputs. You can find Libraries for this devices are on the my GitHub 

## **Key**
With this class you can create Key objects which can be triggered with a button.

### Constructor
```cpp
Key(uint8_t pin, uint16_t key);
Key(uint16_t key); // for virtual control
```
- **pin** are the connection Pin for the button hardware, this is not needed for virtual control
- **key** is the key number of the executors, refer to the GrandMA3 manual

**Example** this should done before the ```setup()```
```cpp
Key key201(2, 201);
// executor button 201, using pin 2
Key key201(201); // for virtual control
```

### Pool
```cpp
void pool(uint16_t poolLocal = 0);
```
- **poolLocal** is the pool number of the executors, refer to the GrandMA3 manual

Set a local page number which overrides the global.

**Example**
```cpp
key201.pool(2); // set local pool 2
key201.pool(); // reset to global pool
```

### Page
```cpp
void page(uint16_t pageLocal = 0);
```
- **pageLocal** is the page number of the executors, refer to the GrandMA3 manual

Set a local page number which overrides the global.

**Example**
```cpp
key201.page(2); // set local page 2
key201.page(); // reset to global page
```

### Update
```cpp
void update();
void update(bool state); // for virtual control
```
- **state** optional for virtual devices, TRUE if button press

To get the actual button state you must call inside the ```loop()``` function

Example, this must happen in the ```loop()``` function
```cpp
key201.update();
// key201.update(TRUE);
```

## **Fader**
This class allows you to control a fader containing  with a hardware (slide) potentiometer as an executor fader.

### Timing constants
```cpp
#define FADER_UPDATE_RATE_MS  1 // update rate, must low at possible for fetching
#define FADER_THRESHOLD       4 // Jitter threshold of the faders
```

### Constructor
```cpp
Fader(uint8_t analogPin, uint16_t fader);
Fader(uint16_t fader); // for virtual control
```
- **analogPin** are the connection Analog Pin for the fader leveler, this is not needed for virtual control
- **fader** is the fader number of the executors, refer to the GrandMA3 manual

Example, this should done before the ```setup()```
```cpp
Fader fader201(A0, 201); // leveler is Analog Pin A0, executor number of the fader is 201
Fader fader201(201); // for virtual control
```

### Pool
```cpp
void pool(uint16_t poolLocal = 0);
```
- **poolLocal** is the pool number of the executors, refer to the GrandMA3 manual

Set a local page number which overrides the global.

**Example**
```cpp
fader201.pool(2); // set local pool 2
fader201.pool(); // reset to global pool
```

### Page
```cpp
void page(uint16_t pageLocal = 0);
```
- **pageLocal** is the page number of the executors, refer to the GrandMA3 manual

Set a local page number which overrides the global.

Example
```cpp
fader201.page(2); // set local page 2
fader201.page(); // reset to global page
```

### Value
```cpp
int32_t value();
```
Return the value (0...100) of the fader.

Example
```cpp
int32_t value = fader201.value();
```

### Fetch
```cpp
void fetch(uint16_t value);
```
- **value** unlock value

Lock the sending of OSC fader data until the value defined in fetch() is reached.<br>
This functionality is intended for page changing. So you need to fetch the fader before you can use it.

Example
```cpp
fader201.fetch(0); // set fetch value to 0
```

### Lock
```cpp
bool lock();
void lock(bool state);
```

Get or set the state of the fetch function, can used for indication of the fader state or force a new state.
- **true** locked fader
- **false** unlocked fader

Example
```cpp
bool state = fader201.lock(); // get the lock state
fader201.lock(false); // set the lock state

```

### Jitter
```cpp
void jitter(uint8_t delta);
```
- ```delta``` +/- value range

This functionality is a helper function for fetching.<br> 
e.g. if ```fetch(20)``` and ```jitter(2)``` the unlock value expand to a range from ```18 ... 22```

Example
```cpp
fader201.jitter(2); // set fetch range to +/- 2
```

### Update
To get the actual button state you must call inside the ```loop()``` function
```cpp
void update();
void update(uint16_t value); // for virtual control
```

- **value** optional for virtual inputs with 10 bits

Example, this must happen in the ```loop()``` function
```cpp
fader201.update();
void update(255); // about 25%
```

## **ExecutorKnob**
The ExecutorKnob class creates an encoder object which allows to control the executor knobs:
```cpp
ExecutorKnob(uint8_t pinA, uint8_t pinB, uint16_t executorKnob, uint8_t direction = FORWARD);
ExecutorKnob(uint16_t executorKnob, uint8_t direction = FORWARD);
```
- **pinA** and **pinB** are the connection Pins for the encoder hardware, this is not needed for virtual control
- **executorKnob** is the number of the executor knob, refer to the GrandMA3 manual
- **direction** is used for changing the direction of the encoder to clockwise if pinA and pinB are swapped. The directions are FORWARD (standard) or REVERSE

Example, this should done before the ```setup()```
```cpp
ExecutorKnob enc301(3, 4, 301, REVERSE);
// the encoder pins are 3/4, the number of the executorKnob is 301, encoder pins are swapped (REVERSE)
ExecutorKnob enc301(301, REVERSE); // for virtual control
```

### Pool
```cpp
void pool(uint16_t poolLocal = 0);
```
- **poolLocal** is the pool number of the executors, refer to the GrandMA3 manual

Set a local page number which overrides the global.

**Example**
```cpp
enc301.pool(2); // set local pool 2
enc301.pool(); // reset to global pool
```

### Page
```cpp
void page(uint16_t pageLocal = 0);
```
- **pageLocal** is the page number of the executors, refer to the GrandMA3 manual

Set a local page number which overrides the global.

Example
```cpp
enc301.page(2); // set local page 2
enc301.page(); // reset to global page
```

### Update
To get the actual encoder state you must call inside the ```loop()``` function
```cpp
void update();
void update(uint8_t stateA, uint8_t stateB); // for virtual control
```
- **stateA** optional for virtual devices, TRUE if encoderA contacts
- **stateB** optional for virtual devices, TRUE if encoderB contacts

Example, this must happen in the ```loop()``` function
```cpp
enc301.update();
enc301.update(TRUE, FALSE); // for virtual control
```

## **CmdButton**
With this class you can create a button which allows to send commands to the console.
```cpp
CmdButton(uint8_t pin, const char command[]);
CmdButton(const char *command); // for virtual control
```
- **pin** are the connection Pin for the button hardware, this not needed for virtual control
- **command** is a command string which should send to the console, refer also to the GrandMA3 manual

Example, this should done before the ```setup()```
```cpp
CmdButton macro1(A2, "GO+ Macro 1"); // button on pin A2, fires Macro 1
CmdButton macro1("GO+ Macro 1"); // for virtual control
```

### Update
To get the actual button state you must call inside the ```loop()``` function
```cpp
void update();
void update(bool state); // for virtual control
```
- **state** optional for virtual devices, TRUE if button press

Example, this must happen in the ```loop()``` function
```cpp
macro1.update();
macro1.update(TRUE); // for virtual button press
```

## **Pages**
With this class you can create a Page object which can be controlled with a two button.

### Constructor
```cpp
Pages(uint8_t pinUp, uint8_t pinDown, uint8_t pagesStart, uint8_t pagesEnd, send_t mode = GLOBAL, cbptr callback = nullptr);
Pages(uint8_t pagesStart, uint8_t pagesEnd, send_t mode = GLOBAL, cbptr callback = nullptr); // for virtual control
```
- **pinUp** are the connection Pin for the up button hardware, this is not needed for virtual control
- **pinDown** are the connection Pin for the up button hardware, this is not needed for virtual control
- **pageStart** is the start page number
- **pageLast** is the last used page number
- **mode** you can control how page information is used
    - LOCAL page is only used for internal use
    - CONSOLE page is only send to the console
    - GLOBAL page is used overall
- **callback** callback pointer to a function which is called when a button is pressed, within the callback function you can proceed fetch() or display functions using ```currentPage()``` and ```lastPage()``` functions

**Example** this should done before the ```setup()```
```cpp
Pages pages(2, 3, 1, PAGES, GLOBAL, pageChange); // up button pin 2, down button pin 3, start with page 1, last page is 4, global control, pageChange() is callback function name
Pages pages(1, PAGES, GLOBAL, pageChange); // for virtual control
```

### currentPage
```cpp
uint16_t currentPage();
```

Get the current page number.

**Example**
```cpp
uint16_t pageCurrent = pages.currentPage()
```

### lastPage
```cpp
uint16_t lastPage();
```

Get the last used page number.

**Example**
```cpp
uint16_t pageLast = pages.lastPage()
```

### Update
```cpp
void update();
void update(bool stateUp, bool stateDown); // for virtual control
```
- **stateUp** state of the up button, optional for virtual devices, TRUE if button press
- **stateDown** state of the down button, optional for virtual devices, TRUE if button press

To get the actual button states you must call inside the ```loop()``` function

Example, this must happen in the ```loop()``` function
```cpp
pages.update();
pages.update(TRUE, FALSE); // for virtual control
```

## **Pools**
With this class you can create a Pool object which can be controlled with a two button.

### Constructor
```cpp
Pools(uint8_t pinUp, uint8_t pinDown, uint8_t poolsStart, uint8_t poolsEnd, send_t mode = GLOBAL, cbptr callback = nullptr);
Pools(uint8_t pagesStart, uint8_t pagesEnd, send_t mode = GLOBAL, cbptr callback = nullptr); // for virtual control
```
- **pinUp** are the connection Pin for the up button hardware, this is not needed for virtual control
- **pinDown** are the connection Pin for the up button hardware, this is not needed for virtual control
- **pageStart** is the start page number
- **pageLast** is the last used page number
- **mode** you can control how page information is used
    - LOCAL page is only used for internal use
    - CONSOLE page is only send to the console
    - GLOBAL page is used overall
- **callback** callback pointer to a function which is called when a button is pressed, within the callback function you can proceed fetch() or display functions using ```currentPage()``` and ```lastPage()``` functions

**Example** this should done before the ```setup()```
```cpp
Pools pools(4, 5, 1, PAGES, GLOBAL, pageChange); // up button pin 4, down button pin 5, start with page 1, last page is 4, global control, pageChange() is callback function name
Pages pages(1, PAGES, GLOBAL, pageChange); // for virtual control
```

### currentPool
```cpp
uint16_t currentPool();
```

Get the current pool number.

**Example**
```cpp
uint16_t poolCurrent = pools.currentPool()
```

### lastPool
```cpp
uint16_t lastPool();
```

Get the last used pool number.

**Example**
```cpp
uint16_t poolLast = pools.lastPool()
```

### Update
```cpp
void update();
void update(bool stateUp, bool stateDown); // for virtual control
```
- **stateUp** state of the up button, optional for virtual devices, TRUE if button press
- **stateDown** state of the down button, optional for virtual devices, TRUE if button press

To get the actual button states you must call inside the ```loop()``` function

Example, this must happen in the ```loop()``` function
```cpp
pools.update();
pools.update(TRUE, FALSE); // for virtual control
```

# Send an OSC message manually
Send an OSC message with different data tags
```cpp
void oscMessage(const char pattern[], int32_t int32);
void oscMessage(const char pattern[], float float32);
void oscMessage(const char pattern[], const char string[]);
void oscMessage(const char pattern[]);
```
- **const char pattern[]** is the OSC pattern 
- **int32_t int32** for integer data
- **float float32** for float date
- **const char string[]** for strings
