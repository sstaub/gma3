#include <TeensyID.h>
#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>
#include "OSCMessage.h"
#include "gma3.h"

// network data
uint8_t mac[6];
#define GMA3_UDP_PORT  8000 // UDP Port configured in gma3

IPAddress localIP(10, 101, 1, 201); // IP address of the microcontroller board
IPAddress subnet(255, 255, 0, 0); // subnet range
uint16_t localUdpPort = GMA3_UDP_PORT;
IPAddress gma3IP(10, 101, 1, 100); // IP address of the gma3 console
uint16_t gma3UdpPort = GMA3_UDP_PORT;

EthernetUDP udp;

// hardware definitions
Key key201(A1, 1, 201);
Fader fader201(A0, 1, 201);
ExecutorKnob enc301(A3, A4, 1, 301, REVERSE);
CmdButton macro1(A2, "GO+ Macro 1");

// use of generic OSC button
#define QLAB_GO_PIN 2
IPAddress qlabIP(10, 101, 1, 100); // IP of QLab
uint16_t qlabPort = 53000; // QLab receive port
OscButton qlabGo(QLAB_GO_PIN , "/go", qlabIP, qlabPort);

void setup() {
  teensyMAC(mac);
	Ethernet.begin(mac, localIP, subnet);
	udp.begin(localUdpPort);
	interface(udp, gma3IP, gma3UdpPort);
	setPrefix("gma3");
	}

void loop() {
	key201.update();
	fader201.update();
	enc301.update();
	macro1.update();
	qlabGo.update();
	}
