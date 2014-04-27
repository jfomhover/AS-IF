/*
 ********************************************************************

 Copyright 2014, Jean-François Omhover (jf.omhover@gmail.com) and Nanosatisfi, Inc. (www.nanosatisfi.com)

 ********************************************************************
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 ********************************************************************

 Description : classes for the manipulation of ports for AS-IF
 this is necessary because different port can need different processing strategies
 (timeouts, remove leading zeros...)

 Last changed : April 13th, 2014

 ********************************************************************
 */

#include <Arduino.h>
#include "asif_config.h"
#include "ASIF_PortWrapper.h"
#include "ASIF_Port_Serial.h"

// *** SERIAL COMMUNICATION ***
// note : input AND output

ASIF_Port_Serial::ASIF_Port_Serial(Stream * input, uint32_t timeout,
		boolean alwaysflush, boolean leading0) {
	this->input = input;
	this->timeout = timeout;
	this->leading0 = leading0;
	this->alwaysflush = alwaysflush;
	currentByte = 0x00;
}
;

boolean ASIF_Port_Serial::start() {
	return (true);
}

void ASIF_Port_Serial::halt() {

}

uint16_t ASIF_Port_Serial::available() {
	if (!leading0)
		return (input->available());

	if (currentByte != 0x00)
		return (1);

	if (input->available() <= 0)
		return (false);

	currentByte = 0x00;
	while (currentByte == 0x00) {
		if (input->available() > 0) {
			currentByte = input->read();
		} else {
			return (0);
		}
	}
	return (1);
}

void ASIF_Port_Serial::flushInput() {
	while (input->available() > 0)
		input->read();
	currentByte = 0x00;
}

void ASIF_Port_Serial::flushOutput() {
	input->flush();
}

int16_t ASIF_Port_Serial::read() {
	if (!leading0) {
		if (input->available() > 0)
			return (input->read());
		else
			return (0x8000);
	}

	if (currentByte != 0x00) {
		byte b = currentByte;
		currentByte = 0x00;
		return (b);
	}

	unsigned long int startMS = millis();
	while ((millis() - startMS) < timeout) {
		if (input->available() > 0) {
			byte b = input->read();
			if (b != 0x00) {
				return (b);
			}
		}
	}
	return (0x00);
}

uint16_t ASIF_Port_Serial::fill(uint8_t * buffer, uint16_t maxlen) {
	return (fill(buffer, maxlen, ASIF_PORT_NOEXPECTEDLEN));
}

uint16_t ASIF_Port_Serial::fill(uint8_t * buffer, uint16_t maxlen,
		uint16_t expectedLen) {
	unsigned long int startMS = millis();
	uint16_t t_len = 0;
/*
	while(t_len < expectedLen) {
		if (input->available() > 0)
			buffer[t_len++] = input->read();
		if ((millis() - startMS) > 10000)
			return(t_len);
	}
	return(t_len);
*/
	while ((millis() - startMS) < timeout) {
		if ((leading0) && (t_len == 0) && (currentByte != 0x00)) {
			startMS = millis();
			buffer[t_len++] = currentByte;
			currentByte = 0x00;
		} else {
			if (input->available() > 0) {
				int16_t t_ret = input->read();
				if (t_ret >= 0) {
					buffer[t_len++] = t_ret & 0x00FF;
					startMS = millis();
				}
//				buffer[t_len++] = input->read();
//				startMS = millis();
			}
		}

		if ((t_len >= maxlen) || (t_len >= expectedLen))
			return (t_len);
	}

	return (t_len);

}

void ASIF_Port_Serial::print(const char * str) {
	send((uint8_t *) str, strlen(str));
}

void ASIF_Port_Serial::write(uint8_t b) {
	// TODO : error prone ?
	input->write(b);
	if (alwaysflush)
		flushOutput();
}

void ASIF_Port_Serial::send(uint8_t * buffer, uint16_t length) {
	for (uint16_t i = 0; i < length; i++) {
		input->write(buffer[i]);
	}
	if (alwaysflush)
		flushOutput();
}

