#include "Arduino.h"
#include "Ethernet3.h"
#include "gma3.h"

// constants and macros

#define BTN_1       2 // GO button
#define BTN_2       3 // Macro button
#define BTN_3       4 // page up button
#define BTN_4       5	// page down button
#define BTN_5       6 // QLab GO button
#define ENC_1_A     7 // encoder A
#define ENC_1_B     8 // encoder B
#define FADER_1     A0 // fader

// Network config
#define GMA3_UDP_PORT 8000 // UDP Port configured in gma3
#define GMA3_TCP_PORT 9000 // UDP Port configured in gma3

uint8_t mac[] = {0x90, 0xA2, 0xDA, 0x10, 0x14, 0x48};
IPAddress localIP(10, 101, 1, 201); // IP address of the microcontroller board
IPAddress subnet(255, 255, 0, 0); // subnet range
uint16_t localUdpPort = GMA3_UDP_PORT;
IPAddress gma3IP(10, 101, 1, 100); // IP address of the gma3 console
uint16_t gma3UdpPort = GMA3_UDP_PORT;
uint16_t gma3TcpPort = GMA3_TCP_PORT;

EthernetUDP udp;
EthernetUDP udpQLab;
EthernetClient tcp;
EthernetClient tcpQLab;

#define PAGES 4
int pageNumber = 1;
int16_t value[PAGES];

// hardware definitions
Key key201(BTN_1, 201, TCP);
Fader fader201(FADER_1, 201);
ExecutorKnob enc301(ENC_1_A, ENC_1_B, 301, UDPOSC, REVERSE);
CmdButton macro1(BTN_2, "GO+ Macro 1");

void upButton() {
	if (fader201.lock() == false)
		value[pageNumber] = fader201.value(); // stores value if the fader is not locked
	if (pageNumber >= PAGES) pageNumber = 1; // rollover to last page
	else pageNumber++;
	char cmd[12] = "Page ";
	strcat(cmd, itoa(pageNumber));
	command(cmd);
	page(pageNumber);
	if (value[pageNumber] != fader201.value())
		fader201.fetch(value[pageNumber]); // force to value of the new page
	else fader201.lock(false); // no lock needed because stored and fader value are equal
	Serial.print("Page ");
	Serial.println(pageNumber);
	}

void downButton() {
	if (fader201.lock() == false)
		value[pageNumber] = fader201.value(); // stores value if the fader is not locked
	if (pageNumber <= 1) pageNumber = PAGES; // rollover to first page
	else pageNumber--;
	char cmd[12] = "Page ";
	strcat(cmd, itoa(pageNumber));
	command(cmd);
	page(pageNumber);
	if (value[pageNumber] != fader201.value())
		fader201.fetch(value[pageNumber]); // force to value of the new page
	else fader201.lock(false); // no lock needed because stored and fader value are equal
	Serial.print("Page ");
	Serial.println(pageNumber);
	}

Button pageUp(BTN_3, upButton);
Button pageDown(BTN_4, downButton);

// use of generic OSC button
IPAddress qlabIP(10, 101, 1, 100); // IP of QLab
uint16_t qlabPort = 53000; // QLab receive port
OscButton qlabGo(BTN_5 , "/go");

void setup() {
	pinMode(LED_BUILTIN, OUTPUT);
	Serial.begin(9600);
	Ethernet.begin(mac, localIP, subnet);
	interfaceGMA3(gma3IP);
	interfaceUDP(udp, gma3UdpPort);
	interfaceTCP(tcp, gma3TcpPort);
	interfaceExternUDP(udpQLab, qlabIP, qlabPort);
	interfaceExternTCP(tcpQLab, qlabIP, qlabPort);
	fader201.fetch(0);
	}

void loop() {
	pageUp.update();
	pageDown.update();
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

	key201.update();
	fader201.update();
	enc301.update();
	macro1.update();
	qlabGo.update();

	digitalWrite(LED_BUILTIN, fader201.lock());
	}
