/*
gma3 for GrandMa3 consoles by Stefan Staub (c) 2021 is licensed under CC BY-NC-SA 4.0
Attribution-NonCommercial-ShareAlike 4.0 International
*/

/**
 * @brief hardware sugestions
 * - fader is a linear type with 10kOhm, from Bourns or ALPS and can be 60/80/100mm long
 * - put a 10nF ceramic capitors between GND and fader leveler to prevent analog noise
 * - Arduino UNO, MEGA:
 *   use IOREF instead +5V to the top (single pin) of the fader (100%)
 *   GND to the center button pin (2 pins, the outer pin is normaly for the leveler) of the fader (0%)
 * - TEENSY 3.x:
 *   use ANALOG GND instead of the normal GND
 * - put 100nF ceramic capitors between ground and the input of the buttons and ecoders
 */

#ifndef GMA3_H
#define GMA3_H

#include "Arduino.h"
#include "Udp.h"
#include "Client.h"

// button values
#define BUTTON_PRESS   (int32_t)1
#define BUTTON_RELEASE (int32_t)0

// encoder direction
#define FORWARD 0
#define REVERSE 1

// fader settings
#define FADER_UPDATE_RATE_MS 40 // update each 40ms
#define FADER_THRESHOLD      4 // Jitter threshold of the faders

// OSC settings
#define OSC_PATTERN_SIZE 64
#define OSC_STRING_SIZE  64
#define OSC_MESSAGE_SIZE 256

// defines for TCPSLIP
#define END     0xC0 // indicates end of packet
#define ESC     0xDB // indicates byte stuffing
#define ESC_END 0xDC // ESC ESC_END means END data byte
#define ESC_ESC 0xDD // ESC ESC_ESC means ESC data byte

// GMA3 naming conventions
#define PREFIX "gma3"
#define PREFIX_PATTERN "/gma3/"
#define PAGE "Page"
#define FADER "Fader"
#define EXECUTOR_KNOB "Encoder"
#define KEY "Key"

/**
 * @brief Network protocol type and coding
 * 
 */
typedef enum ProtocolType {
	UDPOSC,
	TCP,
	TCPSLIP,
	} protocol_t;

/**
 * @brief OSC data types
 * 
 */
typedef enum OscType {
	NONE,
	INT32,
	FLOAT32,
	STRING,
	} osc_t;

struct Message {
	uint8_t message[OSC_MESSAGE_SIZE];
	int32_t size;
	protocol_t protocol;
	bool update;
	};

struct OSC {
	char pattern[OSC_PATTERN_SIZE];
	char tag[12];
	char string[OSC_STRING_SIZE];
	int32_t int32[2];
	float float32;
	};

/**
 * @brief Set GMA3 IP address
 * 
 * @param gma3IP GMA3 console IP address
 */
void interfaceGMA3(IPAddress ip);

/**
 * @brief set UDP interface
 * 
 * @param gma3Udp UDP interface
 * @param gma3UdpPort UDP port off GrandMA3, standard port is 8000
 */
void interfaceUDP(UDP &udp, uint16_t port = 8000);

/**
 * @brief Set TCP interface
 * 
 * @param tcp TCP interface
 * @param eosTcpPort TCP port off GrandMA3, standard port is 9000
 */
void interfaceTCP(Client &tcp, uint16_t port = 9000);

/**
 * @brief Set UDP interface
 * 
 * @param udp UDP interface
 * @param ip IP address of the receiver
 * @param port UDP port of the receiver
 */
void interfaceExternUDP(UDP &udp, IPAddress ip, uint16_t port);

/**
 * @brief Set TCP interface
 * 
 * @param tcp TCP interface
 * @param ip IP address of the receiver
 * @param port TCP port of the receiver
 */
void interfaceExternTCP(Client &tcp, IPAddress ip, uint16_t port);

/**
 * @brief send an OSC message via UDP to gma3 or external receiver
 * 
 */
void sendUDP();
void sendExternUDP();

/**
 * @brief Receive an OSC message via UDP
 * 
 * @return true if there is an OSC message received
 * @return false 
 */
bool receiveUDP();

/**
 * @brief Send an OSC message via TCP to gma3 or external receiver
 * 
 */
void sendTCP();
void sendExternTCP();

/**
 * @brief Send OSC data to gma3 or external receiver
 * 
 */
void sendOSC();
void sendExternOSC();

/**
 * @brief Parse the oscmessage
 * following assumtions
 * - the gma3 osc message could contains 1 string, max. 2 integer and 1 float
 * - the first item can only be a type of string
 * - the second item can only be a type of integer
 * - the third item can be a type of integer or float
 * @param msg OSC message
 */
void parseOSC(uint8_t *msg);

/**
 * @brief Return the parser results
 * 
 * @return const char* OSC pattern
 * @return const char* string argument
 * @return int32_t the two integer arguments
 * @return float the float argument used for fader level
 */
const char* patternOSC();
const char* stringOSC();
int32_t int1OSC();
int32_t int2OSC();
float floatOSC();

/**
 * @brief Key object
 * 
 */
class Key {
	public:
		/**
		 * @brief Construct a new Key object
		 * 
		 * @param pin button pin
		 * @param page number of the page
		 * @param key number of the executor button
		 */
		Key(uint8_t pin, uint16_t page, uint16_t key, protocol_t protocol = UDPOSC);

		/**
		 * @brief update the state of the Key button, must in loop()
		 * 
		 */
		void update();
	private:
  	uint8_t pin;
		protocol_t protocol;
		char pattern[OSC_PATTERN_SIZE] = "/";
  	uint8_t last;
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
	 * @param analogPin pin for the fader leveler
	 * @param pageNumber number of the page
	 * @param faderNumber number of the executor fader
	 */
	Fader(uint8_t analogPin, uint16_t page, uint16_t fader, protocol_t protocol = UDPOSC);

	/**
	 * @brief update the state of the Fader, must in loop()
	 * 
	 */
	void update();

	private:
		uint8_t analogPin;
		protocol_t protocol;
		char pattern[OSC_PATTERN_SIZE] = "/";
		int16_t analogLast;
		int16_t valueLast;
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
		 * @param pinA pin A of the encoder
		 * @param pinB pin B of the encoder
		 * @param pageNumber number of the page
	 	 * @param executorKnobNumber number of the executorKnob
		 * @param direction the direction for the encoder, can be FORWARD or REVERSE, depends on hardware alignment
		 */
		ExecutorKnob(uint8_t pinA, uint8_t pinB, uint16_t page, uint16_t executorKnob, protocol_t protocol = UDPOSC, uint8_t direction = FORWARD);

		/**
		 * @brief update the output of the executorKnob, must be in loop()
		 * 
		 */
		void update();
	
	private:
		uint8_t pinA;
		uint8_t pinB;
		protocol_t protocol;
		char pattern[OSC_PATTERN_SIZE] = "/";
		uint8_t pinALast;
		uint8_t pinACurrent;
		uint8_t direction;
		int8_t encoderMotion;
		uint8_t value;
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
		 * @param pin button pin
		 * @param cmdPattern command string
		 */
		CmdButton(uint8_t pin, const char command[], protocol_t protocol = UDPOSC);

		/**
		 * @brief update the state of the cmdButton, must in loop()
		 * 
		 */
		void update();

	private:
		char cmdString[OSC_STRING_SIZE];
		protocol_t protocol;
		char pattern[OSC_PATTERN_SIZE] = "/";
  	uint8_t pin;
  	uint8_t last;
	};

class OscButton {
	public:
	/**
	 * @brief Construct a new osc Button object for sending an integer value
	 * 
	 * @param pin button pin
	 * @param pattern OSC address
	 * @param integer32 value must cast (int32_t) when using with a non matching size
	 */
	OscButton(uint8_t pin, const char pattern[], int32_t integer32, protocol_t protocol = UDPOSC);
	
	/**
	 * @brief Construct a new osc Button object for sending a float value
	 * 
	 * @param pin button pin
	 * @param pattern OSC address
	 * @param float32 float value
	 */
	OscButton(uint8_t pin, const char pattern[], float float32, protocol_t protocol = UDPOSC);
	
	/**
	 * @brief Construct a new osc Button object for sending a string
	 * 
	 * @param pin button pin
	 * @param pattern OSC address
	 * @param message message string
	 */
	OscButton(uint8_t pin, const char pattern[], const char message[], protocol_t protocol = UDPOSC);
	
	/**
	 * @brief Construct a new osc Button object with no value
	 * 
	 * @param pin button pin
	 * @param pattern OSC address
	 */
	OscButton(uint8_t pin, const char pattern[], protocol_t protocol = UDPOSC);
	
	/**
	 * @brief Update the state of the OSC button, must in loop()
	 * 
	 */
	void update();

	private:
		enum osc_t {NONE, INT32, FLOAT32, STRING};
		osc_t typ;
		char patternString[OSC_PATTERN_SIZE];
		int32_t integer32;
		float float32;
		char messageString[OSC_STRING_SIZE];
		protocol_t protocol;
		uint8_t pin;
    uint8_t last;
	};

/**
 * @brief Creates osc messages with different data types
 * 
 * @param osc message
 * @param value integer32, float, string value and none argument
 * @param protocol type of the used protocol, UDP, TCP or TCPSLIP
 */
void oscMessage(const char pattern[], int32_t int32, protocol_t protocol = UDPOSC);
void oscMessage(const char pattern[], float float32, protocol_t protocol = UDPOSC);
void oscMessage(const char pattern[], const char string[], protocol_t protocol = UDPOSC);
void oscMessage(const char pattern[], protocol_t protocol = UDPOSC);

/**
 * @brief Encode TCPSLIP messages for external OSC buttons
 * 
 */
void slipEncode();

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
const char* itoa(int number);

#endif