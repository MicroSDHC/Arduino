 /*
 V1.0 - first version
 
 Created by Igor Jarc <igor.jarc1@gmail.com>
 See http://iot-playground.com for details
 
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */
#ifndef EasyIoTEsp_h
#define EasyIoTEsp_h

#include "Arduino.h"
#include "Esp8266EasyIoTConfig.h"
#include "Esp8266EasyIoT.h"
#include "Esp8266EasyIoTMsg.h"



#include <avr/eeprom.h>

#ifdef __cplusplus
#include <Arduino.h>
#endif


#define BUFFER_SIZE 128
#define BUFFERMASK 0x7F


#define PING_TIME 5000
#define COMMAND_RESPONSE_TIME 15000

#ifdef DEBUG
#define debug(x,...) debugPrint(x, ##__VA_ARGS__)
#else
#define debug(x,...) 
#endif

					
// Message types
typedef enum {
	E_START = 0,
	E_CWMODE,       // wifi mode
	E_CWJAP,		// join the AP
	E_CWJAP1,		// join the AP
	E_CIPSTART,		// set up TCP connection
	E_CIPSEND,		// sending	
	E_CIPSEND_1,	// 
	E_IDLE,			// socket open waiting for send or receive
	E_RECEIVE,	    // receive data
	E_CIPCLOSE,		// close connection	
	E_HWRESET,		// HW reset

	E_WAIT_OK,
} e_internal_state;



// EEPROM start address for EasyIotEsp library data
#define EEPROM_START 0
// EEPROM location of node id
#define EEPROM_NODE_ID_ADDRESS EEPROM_START

#define AUTO 0xFFFF // 0-56635. Id 56635 is reserved for auto initialization of nodeId.




#ifdef __cplusplus
class Esp8266EasyIoT
{
public:
	Esp8266EasyIoT();
	void begin(void (* msgCallback)(const Esp8266EasyIoTMsg &), int resetPin, Stream *serial);
	void begin(void (* msgCallback)(const Esp8266EasyIoTMsg &), int resetPin, Stream *serial, Stream *serialDebug);

	bool waitIdle();
	bool process();

	void present(uint8_t sensorId, uint8_t sensorType, bool ack=false);
	void send(Esp8266EasyIoTMsg &message);
	void hwReset();

	uint16_t _nodeId;
private:
	void requestNodeId();
	void sendinternal(Esp8266EasyIoTMsg &message);

	bool isDebug;
	Stream *_serial;
#ifdef DEBUG
	Stream *_serialDebug;
#endif
	unsigned long _lastPing;
	bool _newMessage;

	unsigned long _pingTimmer;
	// comand response
	bool _waitingCommandResponse;
	unsigned long _commandRespondTimer;

	bool isTimeout(unsigned long startTime, unsigned long timeout);
	void setPingTimmer();
	void resetPingTimmer();

#ifdef DEBUG
	void debugPrint(const char *fmt, ... );
#endif

protected:
	Esp8266EasyIoTMsg msg;
	Esp8266EasyIoTMsg ack;
	int _resetPin;
//	Esp8266EasyIoTMsg msgsend;

//link fuctions
	private:  
	e_internal_state processesp();
	bool writeesp(Esp8266EasyIoTMsg &message);
	void executeCommand(String cmd, unsigned long respondTimeout);
	bool isOk(bool chop = true);
	bool isError(bool chop = true);

	void receiveAll();

	e_internal_state _state;
	e_internal_state _okState;
	e_internal_state _errorState;
	unsigned long _startTime;
	unsigned long _respondTimeout;
	uint8_t _rxPin;
	uint8_t _txPin;
	char _rxBuffer[BUFFER_SIZE];
	byte _rxHead; // First written one
	byte _rxTail; // Last written one. 
	String _cmd;
	int _connectErrCnt;
	int _initErrCnt;
	bool _rxFlushProcessed;
	bool rxPos(const char* reference, byte* from=0, byte* to=0);
	bool rxPos(const char* reference, byte thishead, byte thistail, byte* from=0, byte* to=0);
	bool rxchopUntil(const char* reference, bool movetotheend, bool usehead);
	String rxCopy(byte from, byte to);
	void rxFlush();
	void startTimmer(unsigned long respondTimeout);

	const uint8_t* _txBuff;
	uint8_t _txLen;

	void (*msgCallback)(const Esp8266EasyIoTMsg &);
};
#endif

#endif