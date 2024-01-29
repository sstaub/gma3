#include "gma3.h"

// internal structs
struct Message receiveMessage, sendMessage;

// network IP address
IPAddress ipGma3;

// network sockets
UDP *udpGma3;
Client *tcpGma3;

// network ports
uint16_t portUdpGma3;
uint16_t portTcpGma3;

// global variables
uint16_t pageCommon = 1;
uint16_t poolCommon = 1;
char namePrefix[NAME_LENGTH_MAX] = "gma3";
char namePrefixSearch[NAME_LENGTH_MAX] = "/gma3/";
char namePool[NAME_LENGTH_MAX] = "DataPool"; // Page name
char namePage[NAME_LENGTH_MAX] = "Page"; // Page name
char nameFader[NAME_LENGTH_MAX] = "Fader"; // Fader name
char nameExecutorKnob[NAME_LENGTH_MAX] = "Encoder"; // ExecutorKnob name
char nameKey[NAME_LENGTH_MAX] = "Key"; // Key name

void prefixName(const char *prefix) {
	memset(namePrefix, 0, NAME_LENGTH_MAX);
	strcpy(namePrefix, prefix);
	memset(namePrefixSearch, 0, NAME_LENGTH_MAX);
	if (strlen(prefix) != 0) {
		strcpy(namePrefixSearch, "/");
		strcat(namePrefixSearch, prefix);
		strcat(namePrefixSearch, "/");
		}
	}

void dataPoolName(const char *pool) {
	memset(namePool, 0, NAME_LENGTH_MAX);
	strcpy(namePool, pool);
	}

void pageName(const char *page) {
	memset(namePage, 0, NAME_LENGTH_MAX);
	strcpy(namePage, page);
	}

void faderName(const char *fader) {
	memset(nameFader, 0, NAME_LENGTH_MAX);
	strcpy(nameFader, fader);
	}

void executorKnobName(const char *executorKnob) {
	memset(nameExecutorKnob, 0, NAME_LENGTH_MAX);
	strcpy(nameExecutorKnob, executorKnob);
	}

void keyName(const char *key) {
	memset(nameKey, 0, NAME_LENGTH_MAX);
	strcpy(nameKey, key);
	}

void interface(UDP &udp, IPAddress ip, uint16_t port) {
	ipGma3 = ip;
	udpGma3 = &udp;
	portUdpGma3 = port;
	sendMessage.protocol = UDPOSC;
	receiveMessage.protocol = UDPOSC;
	udpGma3->begin(port);
	}

void interface(Client &tcp, protocol_t protocol, IPAddress ip, uint16_t port) {
	ipGma3 = ip;
	tcpGma3 = &tcp;
	portTcpGma3 = port;
	sendMessage.protocol = TCP;
	receiveMessage.protocol = TCP;
	tcpGma3->connect(ipGma3, port);
	}

void sendOSC() {
	switch (sendMessage.protocol) {
		case UDPOSC:
			udpGma3->beginPacket(ipGma3, portUdpGma3);
			udpGma3->write(sendMessage.message, sendMessage.size);
			udpGma3->endPacket();
			break;
		case TCP:
			if (!tcpGma3->connected()) tcpGma3->connect(ipGma3, portTcpGma3);
			tcpGma3->write(sendMessage.message, sendMessage.size);
			break;
		}
	}

void command(const char cmd[]) {
	char pattern[OSC_PATTERN_SIZE];
	memset(pattern, 0, sizeof(pattern));
	pattern[0] = '/';
	if (strlen(namePrefix) != 0) {
		strcat(pattern, namePrefix);
		pattern[strlen(pattern)] = '/';
		}
	strcat(pattern, "cmd");
	oscMessage(pattern, cmd);
	}

uint16_t commonPool() {
	return poolCommon;
	}

void commonPool(uint16_t pool) {
	poolCommon = pool;
	}

uint16_t commonPage() {
	return pageCommon;
	}

void commonPage(uint16_t page) {
	pageCommon = page;
	}

Parser::Parser(cbptr callback) {
	this->callback = callback;
	}

const char* Parser::patternOSC() {
	return receiveData.pattern + strlen(namePrefixSearch);
	}

int Parser::dataStructure(uint8_t level) {
	if (level < 5) return dataValue[level];
	return 0;
	}

const char* Parser::stringOSC() {
	return receiveData.string;
	}

int32_t Parser::int1OSC() {
	return receiveData.int32[0];
	}

int32_t Parser::int2OSC() {
	return receiveData.int32[1];
	}

float Parser::floatOSC() {
	return receiveData.float32;
	}

void Parser::update() {
	if(receiveMessage.protocol == UDPOSC) {
		if(receiveUDP()) {
			if (callback != nullptr) callback();
			return;
			}
		}
	if(receiveMessage.protocol == TCP) {
		if(receiveTCP()) {
			if (callback != nullptr) callback();
			}
		}
	}

void Parser::parseOSC() {
	memset(receiveData.pattern, 0, sizeof(receiveData.pattern));
	memset(receiveData.tag, 0, sizeof(receiveData.tag));
	memset(receiveData.string, 0, sizeof(receiveData.string)); // what happens with to string
	*receiveData.int32 = 0;
	receiveData.float32 = 0.0f;
	int dataStart;
	strncpy(receiveData.pattern, (char*)receiveMessage.message, sizeof(receiveData.pattern) - 1);
	sscanf(receiveData.pattern + strlen(namePrefixSearch), "%d.%d.%d.%d.%d", &dataValue[0], &dataValue[1], &dataValue[2], &dataValue[3], &dataValue[4]);
	int patternSize = strlen(receiveData.pattern);
	int patternOffset = patternSize % 4;
	int tagStart;
	if (patternOffset == 0) tagStart = patternSize + 4;
	else tagStart = patternSize + (4 - patternOffset);
	strncpy(receiveData.tag, (char*)receiveMessage.message + tagStart, sizeof(receiveData.tag) - 1);
	int tagSize = strlen(receiveData.tag); // including ','
	int tagOffset = tagSize % 4;
	if (tagOffset == 0) dataStart = tagSize + 4 + tagStart;
	else dataStart = tagSize + (4 - tagOffset) + tagStart;
	
	if (receiveData.tag[1] != 0) {
		if (receiveData.tag[1] == 's') {
			strncpy(receiveData.string, (char*)receiveMessage.message + dataStart, sizeof(receiveData.string) - 1);
			int stringSize = strlen(receiveData.string);
			int stringOffset = stringSize % 4;
			if (stringOffset == 0) dataStart = dataStart + stringSize + 4;
			else dataStart = dataStart + stringSize + (4 - stringOffset);
			}
		}
	else return;
	
	if (receiveData.tag[2] != 0) {
		if (receiveData.tag[2] == 'i') {
			receiveData.int32[0] = htoi(receiveMessage.message, dataStart);
			dataStart = dataStart + 4;
			}
		}
	else return;

	if (receiveData.tag[3] != 0) {
		if (receiveData.tag[3] == 'i') {
			receiveData.int32[1] = htoi(receiveMessage.message, dataStart);
			dataStart = dataStart + 4;
			}
		else if (receiveData.tag[3] == 'f') {
			receiveData.float32 = htof(receiveMessage.message, dataStart);
			dataStart = dataStart + 4;
			}
		}
	}

bool Parser::receiveUDP() {
	size_t size = udpGma3->parsePacket();
	if (size > 0) {
		receiveMessage.size = size;
		memset(receiveMessage.message, 0, OSC_MESSAGE_SIZE);
		if (size <= sizeof(receiveMessage.message)) {
			udpGma3->read(receiveMessage.message, size);
			if (strlen(namePrefix) != 0) {
				if (memcmp(receiveMessage.message, namePrefixSearch, strlen(namePrefixSearch)) != 0) return false;
				}
			parseOSC();
			return true;
			}
		}
	return false;
	}

bool Parser::receiveTCP() {
	size_t size = tcpGma3->available();
	if (size > 0) {
		receiveMessage.size = size;
		memset(receiveMessage.message, 0, OSC_MESSAGE_SIZE);
		if (size <= sizeof(receiveMessage.message)) {
			tcpGma3->read(receiveMessage.message, size);
			if (strlen(namePrefix) != 0) {
				if (memcmp(receiveMessage.message, namePrefixSearch, strlen(namePrefixSearch)) != 0) return false;
				}
			parseOSC();
			return true;
			}
		}
	return false;
	}

Pools::Pools(uint8_t pinUp, uint8_t pinDown, uint8_t poolsStart, uint8_t poolsEnd, send_t mode, cbptr callback) {
	this->pinUp = pinUp;
	pinMode(pinUp, INPUT_PULLUP);
	pinUpLast = digitalRead(pinUp);
	this->pinDown = pinDown;
	pinMode(pinDown, INPUT_PULLUP);
	pinDownLast = digitalRead(pinDown);
	this->poolsStart = poolsStart;
	this->poolsEnd = poolsEnd;
	this->callback = callback;
	this->mode = mode;
	poolNumber = poolsStart;
	poolCommon = poolsStart;
	}

Pools::Pools(uint8_t poolsStart, uint8_t poolsEnd, send_t mode, cbptr callback) {
	this->poolsStart = poolsStart;
	this->poolsEnd = poolsEnd;
	this->callback = callback;
	this->mode = mode;
	poolNumber = poolsStart;
	poolCommon = poolsStart;
	}

uint16_t Pools::currentPool() {
	return poolCommon;
	}

uint16_t Pools::lastPool() {
	return poolLast;
	}

void Pools::update() {
	if (digitalRead(pinUp) != pinUpLast) {
		if (pinUpLast == false) {
			pinUpLast = true; // button release
			}
		else { // button press
			pinUpLast = false;
			poolLast = poolNumber;
			if (poolNumber >= poolsEnd) poolNumber = poolsStart; // rollover to first page
			else poolNumber++;
			if(mode == GLOBAL || mode == CONSOLE) sendPool(poolNumber);
			poolCommon = poolNumber;
			if (callback != nullptr) callback(); // execute callback
			}
		return;
		}
	if (digitalRead(pinDown) != pinDownLast) {
		if (pinDownLast == false) {
			pinDownLast = true; // button release
			}
		else { // button press
			pinDownLast = false;
			poolLast = poolNumber;
			if (poolNumber <= poolsStart) poolNumber = poolsEnd; // rollover to last page
			else poolNumber--;
			if(mode == GLOBAL || mode == CONSOLE) sendPool(poolNumber);
			poolCommon = poolNumber;
			if (callback != nullptr) callback(); // execute callback
			}
		return;
		}
	}

void Pools::update(bool stateUp, bool stateDown) {
	if (stateUp != pinUpLast) {
		if (pinUpLast == false) {
			pinUpLast = true; // button press
			poolLast = poolNumber;
			if (poolNumber >= poolsEnd) poolNumber = poolsStart; // rollover to first page
			else poolNumber++;
			if(mode == GLOBAL || mode == CONSOLE) sendPool(poolNumber);
			poolCommon = poolNumber;
			if (callback != nullptr) callback(); // execute callback
			}
		else { // button release
			pinUpLast = false;
			}
		return;
		}
	if (stateDown != pinDownLast) {
		if (pinDownLast == false) {
			pinDownLast = true; // button press
			poolLast = poolNumber;
			if (poolNumber <= poolsStart) poolNumber = poolsEnd; // rollover to last page
			else poolNumber--;
			if(mode == GLOBAL || mode == CONSOLE) sendPool(poolNumber);
			poolCommon = poolNumber;
			if (callback != nullptr) callback(); // execute callback
			}
		else { // button release
			pinDownLast = false;
			}
		return;
		}
	}

void Pools::sendPool(uint16_t pool) {
	char cmd[12] = "dataPool ";
	strcat(cmd, itoa(pool));
	command(cmd);
	}

Pages::Pages(uint8_t pinUp, uint8_t pinDown, uint8_t pagesStart, uint8_t pagesEnd, send_t mode, cbptr callback) {
	this->pinUp = pinUp;
	pinMode(pinUp, INPUT_PULLUP);
	pinUpLast = digitalRead(pinUp);
	this->pinDown = pinDown;
	pinMode(pinDown, INPUT_PULLUP);
	pinDownLast = digitalRead(pinDown);
	this->pagesStart = pagesStart;
	this->pagesEnd = pagesEnd;
	this->mode = mode;
	this->callback = callback;
	pageLast = pagesStart;
	pageCommon = pagesStart;
	}

Pages::Pages(uint8_t pagesStart, uint8_t pagesEnd, send_t mode, cbptr callback) {
	this->pagesStart = pagesStart;
	this->pagesEnd = pagesEnd;
	this->mode = mode;
	this->callback = callback;
	pageLast = pagesStart;
	pageCommon = pagesStart;
	}

uint16_t Pages::currentPage() {
	return pageCommon;
	}

uint16_t Pages::lastPage() {
	return pageLast;
	}

void Pages::update() {
	if (digitalRead(pinUp) != pinUpLast) {
		if (pinUpLast == false) {
			pinUpLast = true; // button release
			}
		else { // button press
			pinUpLast = false;
			pageLast = pageNumber;
			if (pageNumber >= pagesEnd) pageNumber = pagesStart; // rollover to first page
			else pageNumber++;
			if(mode == GLOBAL || mode == CONSOLE) sendPage(pageNumber);
			pageCommon = pageNumber;
			if (callback != nullptr) callback(); // execute callback
			}
		return;
		}
	if (digitalRead(pinDown) != pinDownLast) {
		if (pinDownLast == false) {
			pinDownLast = true; // button release
			}
		else { // button press
			pinDownLast = false;
			pageLast = pageNumber;
			if (pageNumber <= pagesStart) pageNumber = pagesEnd; // rollover to last page
			else pageNumber--;
			if(mode == GLOBAL || mode == CONSOLE) sendPage(pageNumber);
			pageCommon = pageNumber;
			if (callback != nullptr) callback(); // execute callback
			}
		return;
		}
	}

void Pages::update(bool stateUp, bool stateDown) {
	if (stateUp != pinUpLast) {
		if (pinUpLast == false) {
			pinUpLast = true; // button press
			pageLast = pageNumber;
			if (pageNumber >= pagesEnd) pageNumber = pagesStart; // rollover to first page
			else pageNumber++;
			if(mode == GLOBAL || mode == CONSOLE) sendPage(pageNumber);
			pageCommon = pageNumber;
			if (callback != nullptr) callback(); // execute callback
			}
		else { // button release
			pinUpLast = false;
			}
		return;
		}
	if (stateDown != pinDownLast) {
		if (pinDownLast == false) {
			pinDownLast = true; // button press
			pageLast = pageNumber;
			if (pageNumber <= pagesStart) pageNumber = pagesEnd; // rollover to last page
			else pageNumber--;
			if(mode == GLOBAL || mode == CONSOLE) sendPage(pageNumber);
			pageCommon = pageNumber;
			if (callback != nullptr) callback(); // execute callback
			}
		else { // button release
			pinDownLast = false;
			}
		return;
		}
	}

void Pages::sendPage(uint16_t page) {
	char cmd[12] = "Page ";
	strcat(cmd, itoa(page));
	command(cmd);
	}

Key::Key(uint8_t pin, uint16_t key) {
	this->pin = pin;
	pinMode(pin, INPUT_PULLUP);
	last = HIGH;
	this->key = key;
	}

Key::Key(uint16_t key) {
	this->key = key;
	}

void Key::pool(uint16_t poolLocal) {
	this->poolLocal = poolLocal;
	}

void Key::page(uint16_t pageLocal) {
	this->pageLocal = pageLocal;
	}

void Key::update() {
	if ((digitalRead(pin)) != last) {
		char pattern[OSC_PATTERN_SIZE];
		memset(pattern, 0, sizeof(pattern));
		pattern[0] = '/';
		if (strlen(namePrefix)) {
			strcat(pattern, namePrefix);
			pattern[strlen(pattern)] = '/';
			}
		if(strlen(namePool)) {
			strcat(pattern, namePool);
			if (poolLocal > 0) strcat(pattern, itoa(poolLocal));
			else strcat(pattern, itoa(poolCommon));
			pattern[strlen(pattern)] = '/';
			}
		if(strlen(namePage)) {
			strcat(pattern, namePage);
			if (pageLocal > 0) strcat(pattern, itoa(pageLocal));
			else strcat(pattern, itoa(pageCommon));
			pattern[strlen(pattern)] = '/';
			}
		strcat(pattern, nameKey);
		strcat(pattern, itoa(key));
		if (last == LOW) {
			last = HIGH;
			oscMessage(pattern, BUTTON_RELEASE);
			}
		else {
			last = LOW;
			oscMessage(pattern, BUTTON_PRESS);
			}
		} 
	}

void Key::update(bool state) {
	if (state != last) {
		char pattern[OSC_PATTERN_SIZE];
		memset(pattern, 0, sizeof(pattern));
		pattern[0] = '/';
		if (strlen(namePrefix)) {
			strcat(pattern, namePrefix);
			pattern[strlen(pattern)] = '/';
			}
		if(strlen(namePool)) {
			strcat(pattern, namePool);
			if (poolLocal > 0) strcat(pattern, itoa(poolLocal));
			else strcat(pattern, itoa(poolCommon));
			pattern[strlen(pattern)] = '/';
			}
		if(strlen(namePage)) {
			strcat(pattern, namePage);
			if (pageLocal > 0) strcat(pattern, itoa(pageLocal));
			else strcat(pattern, itoa(pageCommon));
			pattern[strlen(pattern)] = '/';
			}
		strcat(pattern, nameKey);
		strcat(pattern, itoa(key));
		if (last == LOW) {
			last = HIGH;
			oscMessage(pattern, BUTTON_PRESS);
			}
		else {
			last = LOW;
			oscMessage(pattern, BUTTON_RELEASE);
			}
		} 
	}

Fader::Fader(uint8_t analogPin, uint16_t fader) {
	this->analogPin = analogPin;
	this->fader = fader;
	analogLast = 0xFFFF; // forces an receiveData output of the fader
	updateTime = millis();
	}

Fader::Fader(uint16_t fader) {
	this->fader = fader;
	analogLast = 0xFFFF; // forces an receiveData output of the fader
	updateTime = millis();
	}

void Fader::pool(uint16_t poolLocal) {
	this->poolLocal = poolLocal;
	}

void Fader::page(uint16_t pageLocal) {
	this->pageLocal = pageLocal;
	}

int32_t Fader::value() {
	return valueLast;
	}

void Fader::fetch(int16_t value) {
	fetchValue = value;
	lockState = true;
	}

void Fader::jitter(uint8_t delta) {
	this->delta = delta;
	}

bool Fader::lock() {
	return lockState;
	}

void Fader::lock(bool state) {
	lockState = state;
	}

void Fader::update() {
	if ((updateTime + FADER_UPDATE_RATE_MS) < millis()) {
		int16_t raw = analogRead(analogPin) >> 2;
		if (raw < (analogLast - FADER_THRESHOLD) || raw > (analogLast + FADER_THRESHOLD)) {
  	  analogLast = raw;
			int32_t value = map(analogLast, FADER_THRESHOLD, 255 - FADER_THRESHOLD, 0, 100); // convert to 0...100
  	  if (valueLast != value) {
  	    valueLast = value;
				if (lockState == true) {
					if ((valueLast <= fetchValue + delta) && (valueLast >= fetchValue - delta)) lockState = false; 
					}
				if (lockState == false) {
					char pattern[OSC_PATTERN_SIZE];
					memset(pattern, 0, sizeof(pattern));
					pattern[0] = '/';
					if (strlen(namePrefix)) {
						strcat(pattern, namePrefix);
						pattern[strlen(pattern)] = '/';
						}
					if(strlen(namePool)) {
						strcat(pattern, namePool);
						if (poolLocal > 0) strcat(pattern, itoa(poolLocal));
						else strcat(pattern, itoa(poolCommon));
						pattern[strlen(pattern)] = '/';
						}
					if(strlen(namePage)) {
						strcat(pattern, namePage);
						if (pageLocal > 0) strcat(pattern, itoa(pageLocal));
						else strcat(pattern, itoa(pageCommon));
						pattern[strlen(pattern)] = '/';
						}
					strcat(pattern, nameFader);
					strcat(pattern, itoa(fader));
					oscMessage(pattern, value);
					}
  	  	}
			}
		updateTime = millis();
		}
	}

void Fader::update(uint16_t analog) {
	if ((updateTime + FADER_UPDATE_RATE_MS) < millis()) {
		int16_t raw = analog >> 2;
  	if (raw < (analogLast - FADER_THRESHOLD) || raw > (analogLast + FADER_THRESHOLD)) {
  	  analogLast = raw;
  	  int32_t value = map(analogLast, FADER_THRESHOLD, 255 - FADER_THRESHOLD, 0, 100); // convert to 0...100
  	  if (valueLast != value) {
  	    valueLast = value;
				if (lockState == true) {
					if ((valueLast <= fetchValue + delta) && (valueLast >= fetchValue - delta)) lockState = false; 
					}
				if (lockState == false) {
					char pattern[OSC_PATTERN_SIZE];
					memset(pattern, 0, sizeof(pattern));
					pattern[0] = '/';
					if (strlen(namePrefix)) {
						strcat(pattern, namePrefix);
						pattern[strlen(pattern)] = '/';
						}
					if(strlen(namePool)) {
						strcat(pattern, namePool);
						if (poolLocal > 0) strcat(pattern, itoa(poolLocal));
						else strcat(pattern, itoa(poolCommon));
						pattern[strlen(pattern)] = '/';
						}
					if(strlen(namePage)) {
						strcat(pattern, namePage);
						if (pageLocal > 0) strcat(pattern, itoa(pageLocal));
						else strcat(pattern, itoa(pageCommon));
						pattern[strlen(pattern)] = '/';
						}
					strcat(pattern, nameFader);
					strcat(pattern, itoa(fader));
					oscMessage(pattern, value);
					}
  	  	}
			}
		updateTime = millis();
		}	
	}

ExecutorKnob::ExecutorKnob(uint8_t pinA, uint8_t pinB, uint16_t executorKnob, uint8_t direction) {
	this->pinA = pinA;
	this->pinB = pinB;
	pinMode(pinA, INPUT_PULLUP);
	pinMode(pinB, INPUT_PULLUP);
	this->direction = direction;
	this->executorKnob = executorKnob;
	}

ExecutorKnob::ExecutorKnob(uint16_t executorKnob, uint8_t direction) {
	this->direction = direction;
	this->executorKnob = executorKnob;
	}

void ExecutorKnob::pool(uint16_t poolLocal) {
	this->poolLocal = poolLocal;
	}

void ExecutorKnob::page(uint16_t pageLocal) {
	this->pageLocal = pageLocal;
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
		char pattern[OSC_PATTERN_SIZE];
		memset(pattern, 0, sizeof(pattern));
		pattern[0] = '/';
		if (strlen(namePrefix)) {
			strcat(pattern, namePrefix);
			pattern[strlen(pattern)] = '/';
			}
		if(strlen(namePool)) {
			strcat(pattern, namePool);
			if (poolLocal > 0) strcat(pattern, itoa(poolLocal));
			else strcat(pattern, itoa(poolCommon));
			pattern[strlen(pattern)] = '/';
			}
		if(strlen(namePage)) {
			strcat(pattern, namePage);
			if (pageLocal > 0) strcat(pattern, itoa(pageLocal));
			else strcat(pattern, itoa(pageCommon));
			pattern[strlen(pattern)] = '/';
			}
		strcat(pattern, nameExecutorKnob);
		strcat(pattern, itoa(executorKnob));
		oscMessage(pattern, (int32_t)encoderMotion);
		}
	}

void ExecutorKnob::update(uint8_t stateA, uint8_t stateB) {
	encoderMotion = 0;
	pinACurrent = stateA;	
	if ((pinALast) && (!pinACurrent)) {
		if (stateB) {
			encoderMotion = - 1;
			}
		else {
			encoderMotion = 1;
			}
		if (direction == REVERSE) encoderMotion = -encoderMotion;
		}
	pinALast = pinACurrent;
	if (encoderMotion != 0) {
		char pattern[OSC_PATTERN_SIZE];
		memset(pattern, 0, sizeof(pattern));
		pattern[0] = '/';
		if (strlen(namePrefix)) {
			strcat(pattern, namePrefix);
			pattern[strlen(pattern)] = '/';
			}
		if(strlen(namePool)) {
			strcat(pattern, namePool);
			if (poolLocal > 0) strcat(pattern, itoa(poolLocal));
			else strcat(pattern, itoa(poolCommon));
			pattern[strlen(pattern)] = '/';
			}
		if(strlen(namePage)) {
			strcat(pattern, namePage);
			if (pageLocal > 0) strcat(pattern, itoa(pageLocal));
			else strcat(pattern, itoa(pageCommon));
			pattern[strlen(pattern)] = '/';
			}
		strcat(pattern, nameExecutorKnob);
		strcat(pattern, itoa(executorKnob));
		oscMessage(pattern, (int32_t)encoderMotion);
		}
	}

CmdButton::CmdButton(uint8_t pin, const char *command) {
	this->pin = pin;
	pinMode(pin, INPUT_PULLUP);
	last = HIGH;
	strncpy(cmdString, command, OSC_STRING_SIZE - 1);
	}

CmdButton::CmdButton(const char *command) {
	strncpy(cmdString, command, OSC_STRING_SIZE - 1);
	}

void CmdButton::update() {
	if ((digitalRead(pin)) != last) {
		if (last == LOW) {
			last = HIGH;
			}
		else {
			last = LOW;
			char pattern[OSC_PATTERN_SIZE];
			memset(pattern, 0, sizeof(pattern));
			pattern[0] = '/';
			if (strlen(namePrefix)) {
				strcat(pattern, namePrefix);
				strcat(pattern, "/");
				}
			strcat(pattern, "cmd");
			oscMessage(pattern, cmdString);
			}
		}
	}

void CmdButton::update(bool state) {
	if (state != last) {
		if (last == LOW) {
			last = HIGH;
			char pattern[OSC_PATTERN_SIZE];
			memset(pattern, 0, sizeof(pattern));
			pattern[0] = '/';
			if (strlen(namePrefix)) {
				strcat(pattern, namePrefix);
				strcat(pattern, "/");
				}
			strcat(pattern, "cmd");
			oscMessage(pattern, cmdString);
			}
		else {
			last = LOW;
			}
		}
	}

void oscMessage(const char pattern[], const char string[]) {
	memset(sendMessage.message, 0, sizeof(sendMessage.message));
	sendMessage.size = 0;
	int patternLength = strlen(pattern);
	int stringLength = strlen(string);
	memcpy(sendMessage.message, pattern, patternLength);
	int patternOffset = patternLength % 4;
	int tagStart;
	if (patternOffset == 0) tagStart = patternLength + 4;
	else tagStart = patternLength + (4 - patternOffset);
	memcpy(sendMessage.message + tagStart, ",s\0\0", 4);
	int stringStart = tagStart + 4;
	memcpy(sendMessage.message + stringStart, string, stringLength);
	int stringOffset = stringLength % 4;
	if (stringOffset == 0) sendMessage.size = stringStart + stringLength + 4;
	else sendMessage.size = stringStart + stringLength + (4 - stringOffset);
	sendOSC();
	}

void oscMessage(const char pattern[], float float32) {
	memset(sendMessage.message, 0, sizeof(sendMessage.message));
	sendMessage.size = 0;
	int patternLength = strlen(pattern);
	memcpy(sendMessage.message, pattern, patternLength);
	int patternOffset = patternLength % 4;
	int tagStart;
	if (patternOffset == 0) tagStart = patternLength + 4;
	else tagStart = patternLength + (4 - patternOffset);
	memcpy(sendMessage.message + tagStart, ",f\0\0", 4);
	int dataStart = tagStart + 4;
	ftoh(sendMessage.message, dataStart, float32);
	sendMessage.size = dataStart + 4;
	sendOSC();
	}

void oscMessage(const char pattern[], int32_t int32) {
	memset(sendMessage.message, 0, sizeof(sendMessage.message));
	sendMessage.size = 0;
	int patternLength = strlen(pattern);
	memcpy(sendMessage.message, pattern, patternLength);
	int patternOffset = patternLength % 4;
	int tagStart;
	if (patternOffset == 0) tagStart = patternLength + 4;
	else tagStart = patternLength + (4 - patternOffset);
	memcpy(sendMessage.message + tagStart, ",i\0\0", 4);
	int dataStart = tagStart + 4;
	itoh(sendMessage.message, dataStart, int32);
	sendMessage.size = dataStart + 4;
	sendOSC();
	}

void oscMessage(const char pattern[]) {
	memset(sendMessage.message, 0, sizeof(sendMessage.message));
	sendMessage.size = 0;
	int patternLength = strlen(pattern);
	memcpy(sendMessage.message, pattern, patternLength);
	int patternOffset = patternLength % 4;
	int tagStart;
	if (patternOffset == 0) tagStart = patternLength + 4;
	else tagStart = patternLength + (4 - patternOffset);
	memcpy(sendMessage.message + tagStart, ",\0\0\0", 4);
	sendMessage.size = tagStart + 4;
	sendOSC();
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

const char* itoa(int32_t number) {
	static char numstring[16];
	sprintf(numstring, "%ld", number);
	return numstring;
	}
