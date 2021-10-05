#include "gma3.h"

// internal structs
struct Message oscIn, oscOut;
struct OSC osc;

// network IP addresses
IPAddress ipGma3;
IPAddress ipExternUdp;
IPAddress ipExternTcp;

// network sockets
UDP *udpGma3;
UDP *udpExtern;
Client *tcpGma3;
Client *tcpExtern;

// network ports
uint16_t portUdpGma3;
uint16_t portTcpGma3;
uint16_t portExternUdp;
uint16_t portExternTcp;

void interfaceGMA3(IPAddress ip) {
	ipGma3 = ip;
	}

void interfaceUDP(UDP &udp, uint16_t port) {
	udpGma3 = &udp;
	portUdpGma3 = port;
	udpGma3->begin(port);
	}

void interfaceTCP(Client &tcp, uint16_t port) {
	tcpGma3 = &tcp;
	portTcpGma3 = port;
	tcpGma3->connect(ipGma3, port);
	}

void interfaceExternUDP(UDP &udp, IPAddress ip, uint16_t port) {
	udpExtern = &udp;
	ipExternUdp = ip;
	portExternUdp = port;
	}

void interfaceExternTCP(Client &tcp, IPAddress ip, uint16_t port) {
	tcpExtern = &tcp;
	ipExternTcp = ip;
	portExternTcp = port;
	tcpExtern->connect(ip, port);
	}

void sendUDP() {
	udpGma3->beginPacket(ipGma3 ,portUdpGma3);
	udpGma3->write(oscOut.message, oscOut.size);
	udpGma3->endPacket();
	}

void sendExternUDP() {
	udpExtern->beginPacket(ipExternUdp ,portExternUdp);
	udpExtern->write(oscOut.message, oscOut.size);
	udpExtern->endPacket();
	}

bool receiveUDP() {
	size_t size = udpGma3->parsePacket();
	if (size > 0) {
		oscIn.size = size;
		memset(oscIn.message, 0, OSC_MESSAGE_SIZE);
		if (size <= sizeof(oscIn.message)) {
			udpGma3->read(oscIn.message, size);
			if (memcmp(oscIn.message, PREFIX_PATTERN, 6) == 0) {
				parseOSC(oscIn.message);
				oscIn.update = true;
				return true;
				}
			}
		}
	return false;
	}

void sendTCP() {
	if (!tcpGma3->connected()) {
		tcpGma3->connect(ipGma3, portTcpGma3);
		}
	tcpGma3->write(oscOut.message, oscOut.size);
	}

void sendExternTCP() {
	if (!tcpExtern->connected()) {
		tcpExtern->connect(ipExternTcp, portExternTcp);
		}
	tcpExtern->write(oscOut.message, oscOut.size);
	}

void sendOSC() {
	switch (oscOut.protocol) {
		case UDPOSC:
			sendUDP();
			break;
		case TCP:
			sendTCP();
			break;
		case TCPSLIP:
			sendTCP();
			break;
		}
	}

void sendExternOSC() {
	switch (oscOut.protocol) {
		case UDPOSC:
			sendExternUDP();
			break;
		case TCP:
			sendExternTCP();
			break;
		case TCPSLIP:
			sendExternTCP();
			break;
		}
	}

void parseOSC(uint8_t *msg) {
	memset(osc.pattern, 0, sizeof(osc.pattern));
	memset(osc.tag, 0, sizeof(osc.tag));
	memset(osc.string, 0, sizeof(osc.string)); // what happens with to string
	*osc.int32 = 0;
	osc.float32 = 0.0f;
	int dataStart;
	strncpy(osc.pattern, (char*)msg, sizeof(osc.pattern) - 1);
	int patternSize = strlen(osc.pattern);
	int patternOffset = patternSize % 4;
	int tagStart;
	if (patternOffset == 0) tagStart = patternSize + 4;
	else tagStart = patternSize + (4 - patternOffset);
	strncpy(osc.tag, (char*)msg + tagStart, sizeof(osc.tag) - 1);
	int tagSize = strlen(osc.tag); // including ','
	int tagOffset = tagSize % 4;
	if (tagOffset == 0) dataStart = tagSize + 4 + tagStart;
	else dataStart = tagSize + (4 - tagOffset) + tagStart;
	
	if (osc.tag[1] != 0) {
		if (osc.tag[1] == 's') {
			strncpy(osc.string, (char*)msg + dataStart, sizeof(osc.string) - 1);
			int stringSize = strlen(osc.string);
			int stringOffset = stringSize % 4;
			if (stringOffset == 0) dataStart = dataStart + stringSize + 4;
			else dataStart = dataStart + stringSize + (4 - stringOffset);
			}
		}
	else return;
	
	if (osc.tag[2] != 0) {
		if (osc.tag[2] == 'i') {
			osc.int32[0] = htoi(msg, dataStart);
			dataStart = dataStart + 4;
			}
		}
	else return;

	if (osc.tag[3] != 0) {
		if (osc.tag[3] == 'i') {
			osc.int32[1] = htoi(msg, dataStart);
			dataStart = dataStart + 4;
			}
		else if (osc.tag[3] == 'f') {
			osc.float32 = htof(msg, dataStart);
			dataStart = dataStart + 4;
			}
		}
	}

const char* patternOSC() {
	return osc.pattern;
	}

const char* stringOSC() {
	return osc.string;
	}

int32_t int1OSC() {
	return osc.int32[0];
	}

int32_t int2OSC() {
	return osc.int32[1];
	}

float floatOSC() {
	return osc.float32;
	}

Key::Key(uint8_t pin, uint16_t page, uint16_t key, protocol_t protocol) {
	this->pin = pin;
	pinMode(pin, INPUT_PULLUP);
	last = digitalRead(pin);
	this->protocol = protocol;
	if (strlen(PREFIX) != 0) {
		strcat(pattern, PREFIX);
		strcat(pattern, "/");
		}
	strcat(pattern, PAGE);
	strcat(pattern, itoa(page));
	strcat(pattern, "/");
	strcat(pattern, KEY);
	strcat(pattern, itoa(key));
	}

void Key::update() {
	if ((digitalRead(pin)) != last) {
		if (last == LOW) {
			last = HIGH;
			oscMessage(pattern, BUTTON_RELEASE, protocol);
			}
		else {
			last = LOW;
			oscMessage(pattern, BUTTON_PRESS, protocol);
			}
		sendOSC();
		} 
	}

Fader::Fader(uint8_t analogPin, uint16_t page, uint16_t fader, protocol_t protocol) {
	this->analogPin = analogPin;
	this->protocol = protocol;
	if (strlen(PREFIX) != 0) {
		strcat(pattern, PREFIX);
		strcat(pattern, "/");
		}
	strcat(pattern, PAGE);
	strcat(pattern, itoa(page));
	strcat(pattern, "/");
	strcat(pattern, FADER);
	strcat(pattern, itoa(fader));
	analogLast = 0xFFFF; // forces an osc output of the fader
	updateTime = millis();
	}

void Fader::update() {
	if ((updateTime + FADER_UPDATE_RATE_MS) < millis()) {
		int16_t raw = analogRead(analogPin);
  	raw = constrain(raw, FADER_THRESHOLD * 2, 1015); // ignore jitter
  	if (raw < (analogLast - FADER_THRESHOLD) || raw > (analogLast + FADER_THRESHOLD)) {
  	  analogLast = raw;
  	  int32_t value = map(analogLast, 8, 1015, 0, 100); // convert to 0...100
  	  if (valueLast != value) {
  	    valueLast = value;
				oscMessage(pattern, value, protocol);
				sendOSC();
  	  	}
			}
		updateTime = millis();
		}	
	}

ExecutorKnob::ExecutorKnob(uint8_t pinA, uint8_t pinB, uint16_t page, uint16_t executorKnob, protocol_t protocol, uint8_t direction) {
	this->pinA = pinA;
	this->pinB = pinB;
	this->direction = direction;
	pinMode(pinA, INPUT_PULLUP);
	pinMode(pinB, INPUT_PULLUP);
	this->protocol = protocol;
	if (strlen(PREFIX) != 0) {
		strcat(pattern, PREFIX);
		strcat(pattern, "/");
		}
	strcat(pattern, PAGE);
	strcat(pattern, itoa(page));
	strcat(pattern, "/");
	strcat(pattern, EXECUTOR_KNOB);
	strcat(pattern, itoa(executorKnob));
	}

void ExecutorKnob::update() {
	encoderMotion = 0;
	pinACurrent = digitalRead(pinA);	
	if ((pinALast) && (!pinACurrent)) {
		if (digitalRead(pinB)) {
			encoderMotion = - 1;
			}
		else {
			encoderMotion = 1;
			}
		if (direction == REVERSE) encoderMotion = -encoderMotion;
		}
	pinALast = pinACurrent;
	if (encoderMotion != 0) {
		oscMessage(pattern, (int32_t)encoderMotion, protocol);
		sendOSC();
		}
	}


CmdButton::CmdButton(uint8_t pin, const char command[], protocol_t protocol) {
	this->pin = pin;
	pinMode(pin, INPUT_PULLUP);
	last = digitalRead(pin);
	this->protocol = protocol;
	if (strlen(PREFIX) != 0) {
		strcat(pattern, PREFIX);
		strcat(pattern, "/");
		}
	strcat(pattern, "cmd");
	strncpy(cmdString, command, OSC_STRING_SIZE - 1);
	}

void CmdButton::update() {
	if ((digitalRead(pin)) != last) {
		if (last == LOW) {
			last = HIGH;
			}
		else {
			last = LOW;
			oscMessage(pattern, cmdString, protocol);
			sendOSC();
			}
		}
	}


OscButton::OscButton(uint8_t pin, const char pattern[], int32_t integer32, protocol_t protocol) {
	this->pin = pin;
	this->integer32 = integer32;
	this->protocol = protocol;
	pinMode(pin, INPUT_PULLUP);
	last = digitalRead(pin);
	typ = INT32;
	memset(patternString, 0, sizeof(patternString));
	strcpy(patternString, pattern);
	}

OscButton::OscButton(uint8_t pin, const char pattern[], float float32, protocol_t protocol) {
	this->pin = pin;
	this->float32 = float32;
	this->protocol = protocol;
	pinMode(pin, INPUT_PULLUP);
	last = digitalRead(pin);
	typ = FLOAT32;
	memset(patternString, 0, sizeof(patternString));
	strcpy(patternString, pattern);
	}

OscButton::OscButton(uint8_t pin, const char pattern[], const char message [], protocol_t protocol) {
	this->pin = pin;
	this->protocol = protocol;
	pinMode(pin, INPUT_PULLUP);
	last = digitalRead(pin);
	typ = STRING;
	memset(patternString, 0, sizeof(patternString));
	strcpy(patternString, pattern);
	memset(messageString, 0, sizeof(messageString));
	strcpy(messageString, message);
	}

OscButton::OscButton(uint8_t pin, const char pattern[], protocol_t protocol) {
	this->pin = pin;
	this->protocol = protocol;
	pinMode(pin, INPUT_PULLUP);
	last = digitalRead(pin);
	typ = NONE;
	memset(patternString, 0, sizeof(patternString));
	strcpy(patternString, pattern);
	}

void OscButton::update() {
	if ((digitalRead(pin)) != last) {
		if (last == LOW) {
			last = HIGH;
			if (typ == INT32) {
				oscMessage(patternString, (int32_t)0, protocol);
				sendExternOSC();
				return;
				}
			if (typ == FLOAT32) {
				oscMessage(patternString, 0.0f, protocol);
				sendExternOSC();
				return;
				}
			}
		else {
			last = LOW;
			switch (typ) {
				case INT32:
					oscMessage(patternString, integer32, protocol);
					break;
				case FLOAT32:
					oscMessage(patternString, float32, protocol);
					break;
				case STRING:
					oscMessage(patternString, messageString, protocol);
					break;
				case NONE:
					oscMessage(patternString, protocol);
					break;
				}
			sendExternOSC();
			}
		} 
	}

void oscMessage(const char pattern[], const char string[], protocol_t protocol) {
	memset(oscOut.message, 0, sizeof(oscOut.message));
	oscOut.size = 0;
	int patternLength = strlen(pattern);
	int stringLength = strlen(string);
	memcpy(oscOut.message, pattern, patternLength);
	int patternOffset = patternLength % 4;
	int tagStart;
	if (patternOffset == 0) tagStart = patternLength + 4;
	else tagStart = patternLength + (4 - patternOffset);
	memcpy(oscOut.message + tagStart, ",s\0\0", 4);
	int stringStart = tagStart + 4;
	memcpy(oscOut.message + stringStart, string, stringLength);
	int stringOffset = stringLength % 4;
	if (stringOffset == 0) oscOut.size = stringStart + stringLength + 4;
	else oscOut.size = stringStart + stringLength + (4 - stringOffset);
	switch (protocol) {
		case UDPOSC:
			oscOut.protocol = UDPOSC;
			break;
		case TCP:
			oscOut.protocol = TCP;
			break;
		case TCPSLIP:
		slipEncode();
			oscOut.protocol = TCPSLIP;
			break;
		}
	}

void oscMessage(const char pattern[], float float32, protocol_t protocol) {
	memset(oscOut.message, 0, sizeof(oscOut.message));
	oscOut.size = 0;
	int patternLength = strlen(pattern);
	memcpy(oscOut.message, pattern, patternLength);
	int patternOffset = patternLength % 4;
	int tagStart;
	if (patternOffset == 0) tagStart = patternLength + 4;
	else tagStart = patternLength + (4 - patternOffset);
	memcpy(oscOut.message + tagStart, ",f\0\0", 4);
	int dataStart = tagStart + 4;
	ftoh(oscOut.message, dataStart, float32);
	oscOut.size = dataStart + 4;
	switch (protocol) {
		case UDPOSC:
			oscOut.protocol = UDPOSC;
			break;
		case TCP:
			oscOut.protocol = TCP;
			break;
		case TCPSLIP:
			slipEncode();
			oscOut.protocol = TCPSLIP;
			break;
		}
	}

void oscMessage(const char pattern[], int32_t int32, protocol_t protocol) {
	memset(oscOut.message, 0, sizeof(oscOut.message));
	oscOut.size = 0;
	int patternLength = strlen(pattern);
	memcpy(oscOut.message, pattern, patternLength);
	int patternOffset = patternLength % 4;
	int tagStart;
	if (patternOffset == 0) tagStart = patternLength + 4;
	else tagStart = patternLength + (4 - patternOffset);
	memcpy(oscOut.message + tagStart, ",i\0\0", 4);
	int dataStart = tagStart + 4;
	itoh(oscOut.message, dataStart, int32);
	oscOut.size = dataStart + 4;
	switch (protocol) {
		case UDPOSC:
			oscOut.protocol = UDPOSC;
			break;
		case TCP:
			oscOut.protocol = TCP;
			break;
		case TCPSLIP:
			slipEncode();
			oscOut.protocol = TCPSLIP;
			break;
		}
	}

void oscMessage(const char pattern[], protocol_t protocol) {
	memset(oscOut.message, 0, sizeof(oscOut.message));
	oscOut.size = 0;
	int patternLength = strlen(pattern);
	memcpy(oscOut.message, pattern, patternLength);
	int patternOffset = patternLength % 4;
	int tagStart;
	if (patternOffset == 0) tagStart = patternLength + 4;
	else tagStart = patternLength + (4 - patternOffset);
	memcpy(oscOut.message + tagStart, ",\0\0\0", 4);
	oscOut.size = tagStart + 4;
	switch (protocol) {
		case UDPOSC:
			oscOut.protocol = UDPOSC;
			break;
		case TCP:
			oscOut.protocol = TCP;
			break;
		case TCPSLIP:
			slipEncode();
			oscOut.protocol = TCPSLIP;
			break;
		}
	}

void slipEncode() {
	uint8_t buffer[OSC_MESSAGE_SIZE] = {0};
	int slipLength = 0;
	buffer[0] = END;
	slipLength++;
	for (int i = 0; i < oscOut.size; i++) {
		if (oscOut.message[i] == END) {
			buffer[slipLength] = ESC;
			buffer[slipLength + 1] = ESC_END;
			slipLength = slipLength + 2;
			}
		else if (oscOut.message[i] == ESC) {
			buffer[slipLength] = ESC;
			buffer[slipLength + 1] = ESC_ESC;
			slipLength = slipLength + 2;
			}
		else {
			buffer[slipLength] = oscOut.message[i];
			slipLength++;
			}
		}
	buffer[slipLength] = END;
	slipLength++;
	oscOut.size = slipLength;
	memcpy(oscOut.message, buffer, oscOut.size);
	}

float htof(uint8_t *msg, uint8_t dataStart) {
	uint8_t floatArray[4];
	floatArray[0] = msg[dataStart + 3];
	floatArray[1] = msg[dataStart + 2];
	floatArray[2] = msg[dataStart + 1];
	floatArray[3] = msg[dataStart];
	return *((float*)floatArray);
	}

void ftoh(uint8_t *msg, uint8_t dataStart, float value) {
	uint8_t *floatArray = (uint8_t *) &value;
	msg[dataStart] = floatArray[3];
	msg[dataStart + 1] = floatArray[2];
	msg[dataStart + 2] = floatArray[1];
	msg[dataStart + 3] = floatArray[0];
}

int32_t htoi(uint8_t *msg, uint8_t dataStart) {
	uint8_t int32Array[4];
	int32Array[0] = msg[dataStart + 3];
	int32Array[1] = msg[dataStart + 2];
	int32Array[2] = msg[dataStart + 1];
	int32Array[3] = msg[dataStart];
	return *((int32_t*)int32Array);;
	}

void itoh(uint8_t *msg, uint8_t dataStart, int32_t value) {
	uint8_t *int32Array = (uint8_t *) &value;
	msg[dataStart] = int32Array[3];
	msg[dataStart + 1] = int32Array[2];
	msg[dataStart + 2] = int32Array[1];
	msg[dataStart + 3] = int32Array[0];
	}	

const char* itoa(int number) {
	static char numstring[12];
	sprintf(numstring, "%d", number);
	return numstring;
	}