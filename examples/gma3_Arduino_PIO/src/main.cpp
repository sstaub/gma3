#include "Arduino.h"
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
EthernetUDP udpQLab;
EthernetClient tcp;
EthernetClient tcpQLab;

// hardware definitions
Fader fader201(FADER, 1, 201);
Key key201(BTN_KEY, 1, 201, TCP);
ExecutorKnob enc301(ENC_1_A, ENC_1_B, 1, 301);
CmdButton macro1(BTN_CMD, "GO+ Macro 1", TCP);

// use of generic OSC button
#define QLAB_GO_PIN 6
IPAddress qlabIP(10, 101, 1, 100); // IP of QLab
uint16_t qlabPort = 53000; // QLab receive port
OscButton qlabGo(QLAB_GO_PIN , "/go", TCPSLIP);

void setup() {
	Serial.begin(9600);
	/* for USR-ES1 module
	Ethernet.setRstPin(9);
	Ethernet.hardreset();
	*/
	Ethernet.begin(mac, localIP, subnet);
	interfaceGMA3(gma3IP);
	interfaceUDP(udp, gma3UdpPort);
	interfaceTCP(tcp, gma3TcpPort);
	interfaceExternUDP(udpQLab, qlabIP, qlabPort);
	interfaceExternTCP(tcpQLab, qlabIP, qlabPort);
}

void loop() {
	key201.update();
	fader201.update();
	enc301.update();
	macro1.update();
	qlabGo.update();
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
}
