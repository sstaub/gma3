#include "Arduino.h"
#include "Ethernet3.h"
#include "gma3.h"

// constants and macros
#define BTN_1       2
#define BTN_2       3

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

int pageNumber = 1;
void upButton() {
	if (pageNumber >= 9) pageNumber = 1;
	else pageNumber++;
	char cmd[12] = "Page ";
	strcat(cmd, itoa(pageNumber));
	command(cmd); // send page command
	page(pageNumber); // set page number
	Serial.print("Page ");
	Serial.println(pageNumber);
}

void downButton() {
	if (pageNumber <= 1) pageNumber = 9;
	else pageNumber--;
	char cmd[12] = "Page ";
	strcat(cmd, itoa(pageNumber));
	command(cmd); // send page command
	page(pageNumber); // set page number
	Serial.print("Page ");
	Serial.println(pageNumber);
	}

// add Up / Down keys
Button pageUp(BTN_1, upButton);
Button pageDown(BTN_2, downButton);

void setup() {
	Serial.begin(9600);
	Ethernet.begin(mac, localIP, subnet);
	interfaceGMA3(gma3IP);
	interfaceUDP(udp, gma3UdpPort);
	interfaceTCP(tcp, gma3TcpPort);
	}

void loop() {
	pageUp.update();
	pageDown.update();
	}