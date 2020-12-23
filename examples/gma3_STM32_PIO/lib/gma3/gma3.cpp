#include "gma3.h"

UDP *udpGMA3;
IPAddress ipGMA3;
uint16_t portGMA3;
char prefixName[NAME_LENGTH_MAX] = "";
char pageName[NAME_LENGTH_MAX] = "Page"; // Page name
char faderName[NAME_LENGTH_MAX] = "Fader"; // Fader name
char executorKnobName[NAME_LENGTH_MAX] = "Encoder"; // ExecutorKnob name
char keyName[NAME_LENGTH_MAX] = "Key"; // Key name

void interface(UDP &gma3Udp, IPAddress gma3IP, uint16_t gma3Port) {
	udpGMA3 = &gma3Udp;
	ipGMA3 = gma3IP; 
	portGMA3 = gma3Port;
	}

void setPrefix(const char prefix[]) {
	memset(prefixName, 0, NAME_LENGTH_MAX);
	strcpy(prefixName, prefix);
	}

void setPageName(const char page[]) {
	memset(pageName, 0, NAME_LENGTH_MAX);
	strcpy(pageName, page);
	}

void setFaderName(const char fader[]) {
	memset(faderName, 0, NAME_LENGTH_MAX);
	strcpy(faderName, fader);
	}

void setExecutorKnobName(const char executorKnob[]) {
	memset(executorKnobName, 0, NAME_LENGTH_MAX);
	strcpy(executorKnobName, executorKnob);
	}

void setKeyName(const char key[]) {
	memset(keyName, 0, NAME_LENGTH_MAX);
	strcpy(keyName, key);
	}

void send(OSCMessage& msg) {
	udpGMA3->beginPacket(ipGMA3, portGMA3);
	msg.send(*udpGMA3);
	udpGMA3->endPacket();
	}

void send(OSCMessage& msg, IPAddress ip, uint16_t port) {
	udpGMA3->beginPacket(ip, port);
	msg.send(*udpGMA3);
	udpGMA3->endPacket();
	}


Key::Key(uint8_t pin, uint16_t page, uint16_t key) {
	this->pin = pin;
	pinMode(pin, INPUT_PULLUP);
	last = digitalRead(pin);
	sprintf(pageString, "%d", page);
	sprintf(keyString, "%d", key);
	}

void Key::update() {
	if ((digitalRead(pin)) != last) {
		char oscPattern[PATTERN_LENGTH_MAX];
		memset(oscPattern, 0, sizeof(oscPattern));
		oscPattern[0] = '/';
		if (strlen(prefixName) != 0) {
			strcat(oscPattern, prefixName);
			oscPattern[strlen(oscPattern)] = '/';
			}
		strcat(oscPattern, pageName);
		strcat(oscPattern, pageString);
		oscPattern[strlen(oscPattern)] = '/';
		strcat(oscPattern, keyName);
		strcat(oscPattern, keyString);
		OSCMessage keyUpdate(oscPattern);
		if (last == LOW) {
			last = HIGH;
			keyUpdate.add(BUTTON_RELEASE);
			}
		else {
			last = LOW;
			keyUpdate.add(BUTTON_PRESS);
			}
		send(keyUpdate);
		} 
	}


Fader::Fader(uint8_t analogPin, uint16_t page, uint16_t fader) {
	this->analogPin = analogPin;
	sprintf(pageString, "%d", page);
	sprintf(faderString, "%d", fader);
	analogLast = 0xFFFF; // forces an osc output of the fader
	updateTime = millis();
	}

void Fader::update() {
	if ((updateTime + FADER_UPDATE_RATE_MS) < millis()) {
		int16_t raw = analogRead(analogPin);
  	raw = constrain(raw, FADER_THRESHOLD * 2, 1015); // ignore jitter
  	if (raw < (analogLast - FADER_THRESHOLD) || raw > (analogLast + FADER_THRESHOLD)) {
  	  analogLast = raw;
  	  uint8_t value = map(analogLast, 8, 1008, 0, 100); // convert to 0...100
  	  if (valueLast != value) {
				char oscPattern[PATTERN_LENGTH_MAX];
				memset(oscPattern, 0, sizeof(oscPattern));
				oscPattern[0] = '/';
				if (strlen(prefixName) != 0) {
					strcat(oscPattern, prefixName);
					oscPattern[strlen(oscPattern)] = '/';
	  			}
				strcat(oscPattern, pageName);
				strcat(oscPattern, pageString);
				oscPattern[strlen(oscPattern)] = '/';
				strcat(oscPattern, faderName);
				strcat(oscPattern, faderString);
				OSCMessage faderUpdate(oscPattern);
				faderUpdate.add(value);
  	    valueLast = value;
				send(faderUpdate);
  	  	}
			}
		updateTime = millis();
		}	
	}


ExecutorKnob::ExecutorKnob(uint8_t pinA, uint8_t pinB, uint16_t page, uint16_t executorKnob, uint8_t direction) {
	this->pinA = pinA;
	this->pinB = pinB;
	this->direction = direction;
	pinMode(pinA, INPUT_PULLUP);
	pinMode(pinB, INPUT_PULLUP);
	sprintf(pageString, "%d", page);
	sprintf(executorKnobString, "%d", executorKnob);
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
		char oscPattern[PATTERN_LENGTH_MAX];
		memset(oscPattern, 0, sizeof(oscPattern));
		oscPattern[0] = '/';
		if (strlen(prefixName) != 0) {
			strcat(oscPattern, prefixName);
			oscPattern[strlen(oscPattern)] = '/';
			}
		strcat(oscPattern, pageName);
		strcat(oscPattern, pageString);
		oscPattern[strlen(oscPattern)] = '/';
		strcat(oscPattern, executorKnobName);
		strcat(oscPattern, executorKnobString);
		OSCMessage executorKnobUpdate(oscPattern);
		executorKnobUpdate.add(encoderMotion);
		send(executorKnobUpdate);
		}
	}


CmdButton::CmdButton(uint8_t pin, const char command[]) {
	this->pin = pin;
	pinMode(pin, INPUT_PULLUP);
	last = digitalRead(pin);
	memset(cmdString, 0, sizeof(cmdString));
	strcpy(cmdString, command);
	}

void CmdButton::update() {
	if ((digitalRead(pin)) != last) {
		if (last == LOW) {
			last = HIGH;
			}
		else {
			last = LOW;
			char oscPattern[PATTERN_LENGTH_MAX];
			memset(oscPattern, 0, sizeof(oscPattern));
			oscPattern[0] = '/';
			if (strlen(prefixName) != 0) {
				strcat(oscPattern, prefixName);
	  		}
			strcat(oscPattern, "/cmd");
			OSCMessage cmdUpdate(oscPattern);
			cmdUpdate.add(cmdString);
			send(cmdUpdate);
			}
		}
	}


OscButton::OscButton(uint8_t pin, const char pattern[], int32_t integer32, IPAddress ip, uint16_t port) {
	this->pin = pin;
	this->integer32 = integer32;
	this->ip = ip;
	this->port = port;
	pinMode(pin, INPUT_PULLUP);
	last = digitalRead(pin);
	typ = INT32;
	memset(patternString, 0, sizeof(patternString));
	strcpy(patternString, pattern);
	}

OscButton::OscButton(uint8_t pin, const char pattern[], float float32, IPAddress ip, uint16_t port) {
	this->pin = pin;
	this->float32 = float32;
	this->ip = ip;
	this->port = port;
	pinMode(pin, INPUT_PULLUP);
	last = digitalRead(pin);
	typ = FLOAT32;
	memset(patternString, 0, sizeof(patternString));
	strcpy(patternString, pattern);
	}


OscButton::OscButton(uint8_t pin, const char pattern[], const char message [], IPAddress ip, uint16_t port) {
	this->pin = pin;
	//this->message = message;
	this->ip = ip;
	this->port = port;
	pinMode(pin, INPUT_PULLUP);
	last = digitalRead(pin);
	typ = STRING;
	memset(patternString, 0, sizeof(patternString));
	strcpy(patternString, pattern);
	memset(messageString, 0, sizeof(messageString));
	strcpy(messageString, message);
	}

OscButton::OscButton(uint8_t pin, const char pattern[], IPAddress ip, uint16_t port) {
	this->pin = pin;
	this->ip = ip;
	this->port = port;
	pinMode(pin, INPUT_PULLUP);
	last = digitalRead(pin);
	typ = NONE;
	memset(patternString, 0, sizeof(patternString));
	strcpy(patternString, pattern);
	}

void OscButton::update() {
	if ((digitalRead(pin)) != last) {
		OSCMessage osc(patternString);
		if (last == LOW) {
			last = HIGH;
			if (typ == INT32) {
				osc.add(0);
				send(osc, ip, port);
				return;
				}
			if (typ == FLOAT32) {
				osc.add(0.0f);
				send(osc, ip, port);
				return;
				}
			}
		else {
			last = LOW;
			if (typ == INT32) osc.add(integer32);
			if (typ == FLOAT32) osc.add(float32);
			if (typ == STRING) osc.add(messageString);
			send(osc, ip, port);
			}
		} 
	}