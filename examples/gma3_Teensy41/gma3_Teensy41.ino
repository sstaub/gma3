// Example for Teensy 4.1 with built-in Ethernet, use the TeensyID library for MAC address

#include <TeensyID.h>
#include <NativeEthernet.h> // for NativeEthernet library https://github.com/vjmuzik/NativeEthernet
#include <NativeEthernetUdp.h> // 
//#include <QNEthernet.h> // for QNEthernet library https://github.com/ssilverman/QNEthernet
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

uint8_t mac[6];

// EthernetUDP udp; // for UDP
EthernetClient tcp; // for TCP

// functions prototypes
void parse();

// hardware definitions
Key key101(BTN_KEY_1, 101);
Fader fader201(FADER, 201);
Key key201(BTN_KEY_2, 201);
Key key301(BTN_KEY_3, 301);
ExecutorKnob enc301(ENC_1_A, ENC_1_B, 301);
Key key401(BTN_KEY_4, 401);
ExecutorKnob enc401(ENC_2_A, ENC_2_B, 301);
CmdButton macro1(BTN_CMD, "GO+ Macro 1");
Parser parser(parse);


void setup() {
	Serial.begin(9600);
  teensyMAC(mac);
	Ethernet.begin(mac, localIP, subnet);
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
	parser.update();
	}

void parse() {
	Serial.print("OSC Message at ");
	Serial.print(millis());
	Serial.print("ms Pattern: ");
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
