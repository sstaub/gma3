/*
gma3 OSC library for ARduino Ethernet UDP is placed under the MIT license
Copyright (c) 2020 Stefan Staub

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/


#ifndef GMA3_H
#define GMA3_H

#include "Arduino.h"
#include "Udp.h"
#include "OSCMessage.h"

// button values
#define BUTTON_PRESS    1
#define BUTTON_RELEASE  0

// encoder direction
#define FORWARD  0
#define REVERSE  1

// fader settings
#define FADER_UPDATE_RATE_MS  40 // update each 40ms
#define FADER_THRESHOLD       4 // Jitter threshold of the faders

// OSC settings
#define NAME_LENGTH_MAX     32
#define COMMAND_LENGTH_MAX  64
#define PATTERN_LENGTH_MAX  128

/**
 * @brief set interface
 * 
 * @param gma3Udp UDP interface
 * @param gam3IP IP off GrandMA3
 * @param gma3Port OSC port off GrandMA3
 */
void interface(UDP &gma3Udp, IPAddress gam3IP, uint16_t gma3Port = 8000);

/**
 * @brief set the Prefix name
 * 
 * @param prefix 
 */
void setPrefix(const char prefix[]);

/**
 * @brief set the Page name
 * 
 * @param page 
 */
void setPageName(const char page[]);

/**
 * @brief set the Fader name
 * 
 * @param fader 
 */
void setFaderName(const char fader[]);

/**
 * @brief set the ExecutorKnob name
 * 
 * @param executorKnob 
 */
void setExecutorKnobName(const char executorKnob[]);

/**
 * @brief set the Key name
 * 
 * @param key 
 */
void setKeyName(const char key[]);

/**
 * @brief send an OSC message via UDP
 * 
 * @param msg OSC message
 * @param ip optional destination IP address
 * @param port optional destination port
 */
void send(OSCMessage& msg);
void send(OSCMessage& msg, IPAddress ip, uint16_t port);

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
		Key(uint8_t pin, uint16_t page, uint16_t key);

		/**
		 * @brief update the state of the Key button, must in loop()
		 * 
		 */
		void update();
	private:

  	uint8_t pin;
		char pageString[7];
		char keyString[7];
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
	Fader(uint8_t analogPin, uint16_t page, uint16_t fader);

	/**
	 * @brief update the state of the Fader, must in loop()
	 * 
	 */
	void update();

	private:

		uint8_t analogPin;
		char pageString[7];
		char faderString[7];
		int16_t analogLast;
		uint8_t valueLast;
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
		ExecutorKnob(uint8_t pinA, uint8_t pinB, uint16_t page, uint16_t executorKnob, uint8_t direction = FORWARD);

		/**
		 * @brief update the output of the executorKnob, must be in loop()
		 * 
		 */
		void update();
	
	private:
		char pageString[7];
		char executorKnobString[7];
		uint8_t pinA;
		uint8_t pinB;
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
		CmdButton(uint8_t pin, const char command[]);

		/**
		 * @brief update the state of the cmdButton, must in loop()
		 * 
		 */
		void update();

	private:

		char cmdString[COMMAND_LENGTH_MAX];
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
	 * @param ip destination IP address
	 * @param port destination port
	 */
	OscButton(uint8_t pin, const char pattern[], int32_t integer32, IPAddress ip, uint16_t port);
	
	/**
	 * @brief Construct a new osc Button object for sending a float value
	 * 
	 * @param pin button pin
	 * @param pattern OSC address
	 * @param float32 float value
	 * @param ip destination IP address
	 * @param port destination port
	 */
	OscButton(uint8_t pin, const char pattern[], float float32, IPAddress ip, uint16_t port);
	
	/**
	 * @brief Construct a new osc Button object for ssending a String
	 * 
	 * @param pin button pin
	 * @param pattern OSC address
	 * @param message message string
	 * @param ip destination IP address
	 * @param port odestination port
	 */
	OscButton(uint8_t pin, const char pattern[], const char message[], IPAddress ip, uint16_t portt);
	
	/**
	 * @brief Construct a new osc Button object with no value
	 * 
	 * @param pin button pin
	 * @param pattern OSC address
	 * @param ip destination IP address
	 * @param port destination port
	 */
	OscButton(uint8_t pin, const char pattern[], IPAddress ip, uint16_t port);
	
	/**
	 * @brief update the state of the Macro button, must in while() loop
	 * 
	 */
	void update();

	private:

		enum osc_t {NONE, INT32, FLOAT32, STRING};
		osc_t typ;
		char patternString[PATTERN_LENGTH_MAX];
		int32_t integer32;
		float float32;
		char messageString[COMMAND_LENGTH_MAX];
		IPAddress ip;
		uint16_t port;
		uint8_t pin;
    uint8_t last;

	};


#endif