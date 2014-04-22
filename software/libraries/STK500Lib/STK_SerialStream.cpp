/*
********************************************************************

Copyright 2014, Jean-François Omhover (jf.omhover@gmail.com, twitter @jfomhover)

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

	Description : handles stk500 communication via serial
				  to receive uploaded sketches from arduino IDE
				  store the HEX file on SD
				  use that HEX file to program an external arduino node
	Last changed : April 22, 2014

********************************************************************
*/

#include <Arduino.h>
#include "STK_SerialStream.h"

// *** SERIAL COMMUNICATION ***
// note : input AND output

STK_SerialStream::STK_SerialStream(Stream * input, uint32_t timeout, boolean alwaysflush) {
	this->input = input;
	this->timeout = timeout;
	this->alwaysflush = alwaysflush;
}

boolean STK_SerialStream::start() {
	return (true);
}

void STK_SerialStream::halt() {

}

uint16_t STK_SerialStream::available() {
	return (input->available());
}

void STK_SerialStream::flushInput() {
	while (input->available() > 0)
		input->read();
}

void STK_SerialStream::flush() {
	input->flush();
}

int16_t STK_SerialStream::read() {
	unsigned long int startMS = millis();
	while ((millis() - startMS) < timeout) {
		if (input->available() > 0) {
			return(input->read());
		}
	}
	return (0x8000);
}

uint16_t STK_SerialStream::fill(uint8_t * buffer, uint16_t maxlen) {
	return (fill(buffer, maxlen, 0xFFFF));
}

uint16_t STK_SerialStream::fill(uint8_t * buffer, uint16_t maxlen,
		uint16_t expectedLen) {
	unsigned long int startMS = millis();
	uint16_t t_len = 0;

	while ((millis() - startMS) < timeout) {
		if (input->available() > 0) {
			int16_t t_ret = input->read();
			if (t_ret >= 0) {
				buffer[t_len++] = t_ret & 0x00FF;
				startMS = millis();
			}
		}

		if ((t_len >= maxlen) || (t_len >= expectedLen)) {
			//flushInput();
			return (t_len);
		}
	}

	return (t_len);
}

void STK_SerialStream::print(const char * str) {
	send((uint8_t *) str, strlen(str));
}

void STK_SerialStream::write(uint8_t b) {
	// TODO : error prone ?
	input->write(b);
	if (alwaysflush)
		flush();
}

void STK_SerialStream::send(uint8_t * buffer, uint16_t length) {
	for (uint16_t i = 0; i < length; i++) {
		input->write(buffer[i]);
	}
	if (alwaysflush)
		flush();
}

