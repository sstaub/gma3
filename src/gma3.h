/*
gma3 for GrandMa3 consoles by Stefan Staub (c) 2023 is licensed under CC BY-NC-SA 4.0
Attribution-NonCommercial-ShareAlike 4.0 International
*/

/**
 * @brief hardware sugestions
 * - fader is a linear type with 10kOhm, from Bourns or ALPS and can be 60/80/100mm long
 * - put a 10nF ceramic capitors between GND and fader leveler to prevent analog noise
 * - Arduino UNO, MEGA
 *   use AREF instead +5V to the top (single pin) of the fader (100%), use the GND next to AREF
 *   GND to the center button pin (2 pins, the outer pin is normaly for the leveler) of the fader (0%)
 * - TEENSY 3.x
 *   use ANALOG GND instead of the normal GND
 * - STM32
 *   use AGND and AVDD
 * - put 100nF ceramic capitors between ground and the inputs of the buttons and ecoders
 */

#ifndef GMA3_H
#define GMA3_H

#include "Arduino.h"
#include "Udp.h"
#include "Client.h"

// callback type
typedef void (*cbptr)();

// button values
#define BUTTON_PRESS   (int32_t)1
#define BUTTON_RELEASE (int32_t)0

// encoder direction
#define FORWARD 0
#define REVERSE 1

// fader settings
#define FADER_UPDATE_RATE_MS  40 // update rate, must low at possible for fetching
#define FADER_THRESHOLD       2 // Jitter threshold of the faders

// OSC settings
#define NAME_LENGTH_MAX  32
#define OSC_PATTERN_SIZE 64
#define OSC_STRING_SIZE  64
#define OSC_MESSAGE_SIZE 128

// defines for TCPSLIP
#define END     0xC0 // indicates end of packet
#define ESC     0xDB // indicates byte stuffing
#define ESC_END 0xDC // ESC ESC_END means END data byte
#define ESC_ESC 0xDD // ESC ESC_ESC means ESC data byte

/**
 * @brief Network protocol type and coding
 * 
 */
typedef enum ProtocolType {
	UDPOSC,
	TCP,
	} protocol_t;

/**
 * @brief OSC data types
 * 
 */
typedef enum OscType {
	NODATA,
	INT32,
	FLOAT32,
	STRING,
	} osc_t;

typedef enum SendMode {
	GLOBAL,
	CONSOLE,
	LOCAL,
	} send_t;

struct Message {
	uint8_t message[OSC_MESSAGE_SIZE];
	int32_t size;
	protocol_t protocol;
	};

struct Data {
	char pattern[OSC_PATTERN_SIZE];
	char tag[12];
	char string[OSC_STRING_SIZE];
	int32_t int32[2];
	float float32;
	};

/**
 * @brief set the Prefix name
 * 
 * @param prefix 
 */
void prefixName(const char *prefix);

/**
 * @brief set the DataPool name
 * 
 * @param pool 
 */
void dataPoolName(const char *pool);

/**
 * @brief set the Page name
 * 
 * @param page 
 */
void pageName(const char *page);

/**
 * @brief set the Fader name
 * 
 * @param fader 
 */
void faderName(const char *fader);

/**
 * @brief set the ExecutorKnob name
 * 
 * @param executorKnob 
 */
void executorKnobName(const char *executorKnob);

/**
 * @brief set the Key name
 * 
 * @param key 
 */
void keyName(const char *key);

/**
 * @brief Set UDP interface
 * 
 * @param udp UDP interface
 * @param ip GMA3 console IP address
 * @param port UDP port off GrandMA3, standard port is 8000
 */
void interface(UDP &udp, IPAddress ip, uint16_t port = 8000);

/**
 * @brief Set TCP interface
 * 
 * @param tcp TCP interface
 * @param protocol_t protocol only TCP is possible in the moment
 * @param ip GMA3 console IP address
 * @param port TCP port off GrandMA3, standard port is 9000
 */
void interface(Client &tcp, protocol_t protocol, IPAddress ip, uint16_t port = 9000);

/**
 * @brief Send OSC data to gma3 or external receiver
 * 
 */
void sendOSC();

/**
 * @brief Send a command to the console
 * 
 * @param cmd 
 */
void command(const char cmd[]);

/**
 * @brief Get the common DataPool number
 * 
 * @return uint16_t common DataPool number
 */
uint16_t commonPool();

/**
 * @brief Set the common DataPool number
 * 
 * @param pool DataPool number
 */
void commonPool(uint16_t pool);

/**
 * @brief Get the common Page number
 * 
 * @return uint16_t common page number
 */
uint16_t commonPage();

/**
 * @brief Set the common Page number
 * 
 * @param page page number
 */
void commonPage(uint16_t page);

/**
 * @brief Parser object to handle messages from the console
 * * following assumtions
 * - the gma3 osc message could contains 1 string, max. 2 integer and 1 float
 * - the first item can only be a type of string
 * - the second item can only be a type of integer
 * - the third item can be a type of integer or float
 */
class Parser {
	public:
		/**
		 * @brief 
		 * 
		 * @param callback function callback
		 */
		Parser(cbptr callback = nullptr);

		/**
		 * @brief Return the dataPool pattern send by the console
		 * 
		 * @return const char* dataPool pattern
		 */
		const char* patternOSC();

		/**
		 * @brief Parse for dataPool level
		 * 
		 * @param level 0 - 4, root is 0
		 * @return uint8_t 
		 */
		int dataStructure(uint8_t level);

		/**
		 * @brief Return the string argument send by the console
		 * 
		 * @return const char* 
		 */
		const char* stringOSC();

		/**
		 * @brief Return the first interger argument send by the console
		 * 
		 * @return int32_t 
		 */
		int32_t int1OSC();

		/**
		 * @brief Return the second interger argument send by the console
		 * 
		 * @return int32_t 
		 */
		int32_t int2OSC();

		/**
		 * @brief Return the float argument send by the console
		 * 
		 * @return float 
		 */
		float floatOSC();

		void update();

	private:
		void parseOSC();
		bool receiveUDP();
		bool receiveTCP();
		int dataValue[5];
		cbptr callback = nullptr;
		struct Data receiveData;
	};

/**
 * @brief Pools object to handle DataPools via buttons 
 * allows to step through an defined amount of data pools
 * 
 */
class Pools {
	public:
		/**
		 * @brief Construct a new Pools object with warp around functionality
		 * 
		 * @param pinUp pin of the up button, not needed for virtual devices
		 * @param pinDown pin of the down button, not needed for virtual devices
		 * @param poolsStart number of the first pool you want to use
		 * @param poolsEnd number of the last pool you want to use
		 * @param mode this can send the pool number to the console, for internal use or both
		 * - CONSOLE for use only with the console
		 * - LOCAL for local use 
		 * - GLOBAL both internal and console
		 * @param callback  function to call when pool change
		 */
		Pools(uint8_t pinUp, uint8_t pinDown, uint8_t poolsStart, uint8_t poolsEnd, send_t mode = GLOBAL, cbptr callback = nullptr);
		Pools(uint8_t poolsStart, uint8_t poolsEnd, send_t mode = GLOBAL, cbptr callback = nullptr);

		/**
		 * @brief Get the current common pool number
		 * 
		 * @return uint16_t pool number
		 */
		uint16_t currentPool();

		/**
		 * @brief Get the last common pool number
		 * 
		 * @return uint16_t pool number
		 */
		uint16_t lastPool();

		/**
		 * @brief Update the output of the pool buttons, must be in loop()
		 * 
		 * @param stateUp optional for virtual devices, TRUE if button press
		 * @param stateDown optional for virtual devices, TRUE if button press
		 */
		void update();
		void update(bool stateUp, bool stateDown);

	private:
		void sendPool(uint16_t pool);
		uint8_t pinUp;
		uint8_t pinUpLast;
		uint8_t pinDown;
		uint8_t pinDownLast;
		uint16_t poolsStart;
		uint16_t poolsEnd;
		uint16_t poolNumber = 1;
		uint16_t poolLast;
		send_t mode;
		cbptr callback;
	};

/**
 * @brief Pools object to handle DataPools via buttons
 * 
 */
class Pages {
	public:
		/**
		 * @brief Construct a new pages object
		 * 
		 * @param pinUp pin of the up button, not needed for virtual devices
		 * @param pinDown pin of the down button, not needed for virtual devices
		 * @param poolsStart number of the first pool you want to use
		 * @param poolsEnd number of the last pool you want to use
		 * @param mode this can send the pool number to the console, for internal use or both
		 * - CONSOLE for use only with the console
		 * - LOCAL for local use 
		 * - GLOBAL both internal and console
		 * @param callback function to call when page change
		 */
		Pages(uint8_t pinUp, uint8_t pinDown, uint8_t pagesStart, uint8_t pagesEnd, send_t mode = GLOBAL, cbptr callback = nullptr);
		Pages(uint8_t pagesStart, uint8_t pagesEnd, send_t mode = GLOBAL, cbptr callback = nullptr);

		/**
		 * @brief Get the current common page number
		 * 
		 * @return uint16_t page number
		 */
		uint16_t currentPage();

		/**
		 * @brief Get the last common page number
		 * 
		 * @return uint16_t page number
		 */
		uint16_t lastPage();

		/**
		 * @brief Update the output of the pool buttons, must be in loop()
		 * 
		 * @param stateUp optional for virtual devices, TRUE if button press
		 * @param stateDown optional for virtual devices, TRUE if button press
		 */
		void update();
		void update(bool stateUp, bool stateDown);
	private:
		void sendPage(uint16_t page);
		uint8_t pinUp;
		uint8_t pinUpLast;
		uint8_t pinDown;
		uint8_t pinDownLast;
		uint16_t pagesStart;
		uint16_t pagesEnd;
		uint16_t pageNumber = 1;
		uint16_t pageLast;
		send_t mode;
		cbptr callback;
	};

/**
 * @brief Key object
 * 
 */
class Key {
	public:
		/**
		 * @brief Construct a new Key object
		 * 
		 * @param pin button pin, not needed for virtual devices
		 * @param key number of the executor button
		 */
		Key(uint8_t pin, uint16_t key);
		Key(uint16_t key);

		/**
		 * @brief Set a local pool number
		 * 
		 * @param page number must between 1 and 9999, if 0 common pool number is used
		 */
		void pool(uint16_t poolLocal = 0);

		/**
		 * @brief set a local page number
		 * 
		 * @param page number must between 1 and 9999, if 0 common page number is used
		 */
		void page(uint16_t pageLocal = 0);

		/**
		 * @brief Update the state of the Key button, must in loop()
		 * 
		 * @param state optional for virtual devices, TRUE if button press
		 */
		void update();
		void update(bool state);

	private:
  	uint8_t pin;
		uint8_t last;
		uint16_t key;
		uint16_t poolLocal = 0;
		uint16_t pageLocal = 0;
	};

/**
 * @brief Fader object
 * 
 */
class Fader {
	public:
		/**
		 * @brief Construct a new Fader object
		 * 
		 * @param analogPin pin for the fader leveler, not needed for virtual devices
		 * @param faderNumber number of the executor fader
		 */
		Fader(uint8_t analogPin, uint16_t fader);
		Fader(uint16_t fader);

		/**
		 * @brief Set a local pool number
		 * 
		 * @param page number must between 1 and 9999, if 0 common pool number is used
		 */
		void pool(uint16_t poolLocal = 0);

		/**
		 * @brief Set a local page number
		 * 
		 * @param page number must between 1 and 9999, if 0 glaobal page number is used
		 */
		void page(uint16_t pageLocal = 0);

		/**
		 * @brief Returns the actual fader value
		 * 
		 * @return int16_t fader value
		 */
		int32_t value();

		/**
		 * @brief Stop sending OSC values until the given value is fetched
		 * 
		 * @param value 
		 */
		void fetch(int16_t value);

		/**
		 * @brief Helper for fetch
		 * 
		 * @param delta range of fetch jitter
		 */
		void jitter(uint8_t delta);

		/**
		 * @brief Return the lock state
		 * 
		 * @return true OSC locked
		 * @return false OSC unlocked
		 */
		bool lock();

		/**
		 * @brief Set the lock state
		 * 
		 * @param state 
		 */
		void lock(bool state);

		/**
		 * @brief Update the state of the fader, must in loop()
		 * @brief For use with virtual inputs:
		 * 
		 * @param value optional for virtual inputs with 10 bits
		 */
		void update();
		void update(uint16_t value);

	private:
		bool lockState = false;
		uint8_t analogPin;
		uint8_t delta;
		uint16_t fader;
		int16_t poolLocal = 0;
		uint16_t pageLocal = 0;
		int16_t fetchValue;
		int16_t analogLast;
		int32_t valueLast;
		uint32_t updateTime;
	};

/**
 * @brief ExecutorKnob object
 * 
 */
class ExecutorKnob {
	public:
		/**
		 * @brief Construct a new ExecutorKnob object
		 * 
		 * @param pinA pin A of the encoder, not needed for virtual devices
		 * @param pinB pin B of the encoder, not needed for virtual devices
	 	 * @param executorKnobNumber number of the executorKnob
		 * @param direction the direction for the encoder, can be FORWARD or REVERSE, depends on hardware alignment
		 */
		ExecutorKnob(uint8_t pinA, uint8_t pinB, uint16_t executorKnob, uint8_t direction = FORWARD);
		ExecutorKnob(uint16_t executorKnob, uint8_t direction = FORWARD);

		/**
		 * @brief Set a local pool number
		 * 
		 * @param page number must between 1 and 9999, if 0 common pool number is used
		 */
		void pool(uint16_t poolLocal = 0);

		/**
		 * @brief Set a local page number
		 * 
		 * @param page number must between 1 and 9999, if 0 common page number is used
		 */
		void page(uint16_t pageLocal = 0);

		/**
		 * @brief Update the output of the executorKnob, must be in loop()
		 * 
		 * @param stateA optional for virtual devices, TRUE if button press
		 * @param stateB optional for virtual devices, TRUE if button press
		 */
		void update();
		void update(uint8_t stateA, uint8_t stateB);
	
	private:
		uint8_t pinA;
		uint8_t pinB;
		uint8_t pinALast;
		uint8_t pinACurrent;
		uint8_t direction;
		uint8_t value;
		int8_t encoderMotion;
		uint16_t executorKnob;
		uint16_t poolLocal = 0;
		uint16_t pageLocal = 0;
	};

/**
 * @brief CmdButton object
 * 
 */
class CmdButton {
	public:
		/**
		 * @brief Construct a new CmdButton object
		 * 
		 * @param pin button pin, not needed for virtual devices
		 * @param command command string
		 */
		CmdButton(uint8_t pin, const char *command);
		CmdButton(const char *command);

		/**
		 * @brief Update the state of the cmdButton, must in loop()
		 * 
		 * @param state optional for virtual devices, TRUE if button press
		 */
		void update();
		void update(bool state);

	private:
		uint8_t pin;
  	uint8_t last;
		char cmdString[OSC_STRING_SIZE];
	};

/**
 * @brief Creates osc messages with different data types and send it
 * 
 * @param osc message
 * @param value integer32, float, string value and nodata argument
 */
void oscMessage(const char pattern[], int32_t int32);
void oscMessage(const char pattern[], float float32);
void oscMessage(const char pattern[], const char string[]);
void oscMessage(const char pattern[]);

/**
 * @brief Big endian array to float conversation
 * 
 * @param msg source message
 * @param dataStart idx of the array
 * @return float 
 */
float htof(uint8_t *msg, uint8_t dataStart);

/**
 * @brief Big endian array to int32 conversation
 * 
 * @param msg source message
 * @param dataStart idx of the array
 * @return int32_t result
 */
int32_t htoi(uint8_t *msg, uint8_t dataStart);

/**
 * @brief Float to big endian array conversion
 * 
 * @param msg target message
 * @param dataStart idx of the array
 * @param value float
 */
void ftoh(uint8_t *msg, uint8_t dataStart, float value);

/**
 * @brief Int32 to big endian array conversion
 * 
 * @param msg target message
 * @param dataStart idx of the array
 * @param value int32
 */
void itoh(uint8_t *msg, uint8_t dataStart, int32_t value);

/**
 * @brief Integer to string conversation
 * 
 * @param number 
 * @return const char* 
 */
const char* itoa(int32_t number);

#endif
