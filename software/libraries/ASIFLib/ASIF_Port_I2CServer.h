/*
 * ASIF_Port_I2CServer.h
 *
 *  Created on: 14 avr. 2014
 *      Author: jeff
 */

#ifndef ASIF_PORT_I2CSERVER_H_
#define ASIF_PORT_I2CSERVER_H_

#include "ASIF_PortWrapper.h"
#include <Wire.h>

// *** I2C COMMUNICATION ***
// note : input only as a server

class ASIF_Port_I2CServer: public ASIF_PortWrapper {
private:
	uint8_t _i2c_content[32];
	uint8_t _length;
	boolean halted;

public:
	boolean start();		// starts the I2C server
	void halt();// can't really halt onReceive(), so just using some internal boolean

	void resetbuffer();				// resets the internal buffer
	uint16_t available();			// returns the length of the current internal buffer
	void flushInput();				// flushs the Wire input
	void flushOutput();				// flushs... nothing.

	void pullI2c(uint16_t len);		// pulls values from I2C, called when receive interrupt on I2C

	uint16_t fill(uint8_t * buffer, uint16_t maxlen);// fills the given buffer with the current content of the port
													 // NOTE : returns the content that was sent on last onReceive()
	uint16_t fill(uint8_t * buffer, uint16_t maxlen, uint16_t expectedLen);

	int16_t read();						// not applicable
	void print(const char * str);		// not applicable
	void send(uint8_t * buffer, uint16_t length);		// not applicable
};

#endif /* ASIF_PORT_I2CSERVER_H_ */
