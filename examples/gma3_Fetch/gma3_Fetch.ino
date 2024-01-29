// simple example for paging and fetching

#include "Ethernet.h"
#include "gma3.h"

// I/O config
#define BTN_1   2
#define BTN_2   3
#define FADER_1 A0

// Network config
#define GMA3_UDP_PORT 8000 // UDP Port configured in gma3
#define GMA3_TCP_PORT 9000 // UDP Port configured in gma3

uint8_t mac[] = {0x90, 0xA2, 0xDA, 0x10, 0x14, 0x48};
IPAddress localIP(10, 101, 1, 201); // IP address of the microcontroller board
IPAddress subnet(255, 255, 0, 0); // subnet range
IPAddress dns(10, 101, 1, 100); // DNS address of your device
IPAddress gateway(10, 101, 1, 100); // Gateway address of your device
uint16_t localUdpPort = GMA3_UDP_PORT;
IPAddress gma3IP(10, 101, 1, 100); // IP address of the gma3 console
uint16_t gma3UdpPort = GMA3_UDP_PORT;
uint16_t gma3TcpPort = GMA3_TCP_PORT;

EthernetUDP udp;
EthernetClient tcp;

#define PAGES 4
int16_t value[PAGES + 1] = {0};
void pageChange();

Fader fader201(FADER_1, 201);
Pages pages(BTN_1, BTN_2, 1, 4, GLOBAL, pageChange); // using 4 pages starting with page 1

void setup() {
	pinMode(LED_BUILTIN, OUTPUT);
	Serial.begin(9600);
	Ethernet.begin(mac, localIP);
	interface(tcp, TCP, gma3IP, gma3TcpPort); // init inteface for TCP
	//interface(udp, gma3IP, gma3UdpPort); // init interface for UDP
	fader201.jitter(2);
	fader201.fetch(0); // force fader to 0
	}

void loop() {
	fader201.update();
	pages.update();
	digitalWrite(LED_BUILTIN, fader201.lock()); // shows if fader is locked
	}

void pageChange() {
	if(fader201.lock() == false) value[pages.lastPage()] = fader201.value(); // if not locked store the value for the last page
	if(fader201.value() != value[pages.currentPage()]) { // if the fader value is not the expected value then fetch to current value
		fader201.fetch(value[pages.currentPage()]);
		}
	Serial.print("Page changed to page ");
	Serial.println(pages.currentPage());
	}