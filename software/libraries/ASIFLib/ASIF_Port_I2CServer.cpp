/*
 * ASIF_Port_I2CServer.cpp
 *
 *  Created on: 14 avr. 2014
 *      Author: jeff
 */

#include <Arduino.h>
#include "asif_config.h"
#include "asif_ports.h"
#include "ASIF_PortWrapper.h"
#include "ASIF_Port_I2CServer.h"
#ifdef ASIF_PORTID_I2C
#include <Wire.h>
#endif


void asif_receiveEvent(int howMany) {
	asif_i2cPort.pullI2c(howMany);
}


// *** I2C COMMUNICATION ***
// note : input only as a server

// starts the I2C server
boolean ASIF_Port_I2CServer::start() {
#ifdef ASIF_PORTID_I2C
	Wire.begin(I2C_ADD_ASSV_1);
	Wire.onReceive(asif_receiveEvent);
	halted = false;
#endif
	return(true);
}

// can't halt onReceive(), so just using some boolean
void ASIF_Port_I2CServer::halt() {
	halted = true;
}

// resets the internal buffer
void ASIF_Port_I2CServer::resetbuffer() {
	memset(_i2c_content, 0x00, 32);
	_length = 0;
}

// returns the length of the current internal buffer
uint16_t ASIF_Port_I2CServer::available() {
	if (halted)
		return(0);
	return (_length);
}

// flushs the Wire input
void ASIF_Port_I2CServer::flushInput() {
	while(Wire.available() > 0)
		Wire.read();

	resetbuffer();
}

// flushs... nothing.
void ASIF_Port_I2CServer::flushOutput() {
	resetbuffer();
}

// pulls values from I2C, called when receive interrupt on I2C
void ASIF_Port_I2CServer::pullI2c(uint16_t len) {
//	resetbuffer();

	if (halted) {
		// if port is halted, do nothing
		flushInput();
		return;
	}

	_length = 0;
	for (uint16_t i = 0; i < len; i++) {
		if (i < 32)
			_i2c_content[_length++] = Wire.read();
		else
			Wire.read(); // note : i don't see why there should be more than 32 bytes, but you never know...
	}
}

// fills the given buffer with the current content of the port
uint16_t ASIF_Port_I2CServer::fill(uint8_t * buffer, uint16_t maxlen) {
	if (_length > maxlen)
		return (0);

	if (_length == 0)
		return(0);

	uint16_t t_len = _length;

	if (t_len > maxlen)
		t_len = maxlen;

	memcpy((void*) buffer, (void*) _i2c_content, t_len);
	resetbuffer();

	return (t_len);
}

// fills the given buffer with the current content of the port
uint16_t ASIF_Port_I2CServer::fill(uint8_t * buffer, uint16_t maxlen,
		uint16_t expectedLen) {
	if (maxlen < expectedLen) // note : you'd have to write poor code to expect more data that you can handle, but you never know...
		return (fill(buffer, maxlen));
	else
		return (fill(buffer, expectedLen));
}

// not applicable here
void ASIF_Port_I2CServer::print(const char * str) {
}

// not applicable here (i don't want to do that)
void ASIF_Port_I2CServer::send(uint8_t * buffer, uint16_t length) {
}

// not applicable here
int16_t ASIF_Port_I2CServer::read() {
	return(0x8000);
}
