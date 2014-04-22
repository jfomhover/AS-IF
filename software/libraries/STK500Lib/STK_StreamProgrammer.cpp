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
#include "STK_StreamProgrammer.h"

#include "stk_defs.h"
#include "stk_util.h"

STK_StreamProgrammer::STK_StreamProgrammer() {
	this->comm = NULL;
	this->rstpin = -1;
	this->buffer = NULL;
	this->maxlen = 0;
	this->buflen = 0;
}

void STK_StreamProgrammer::setBuffer(byte * buffer, int buflen) {
	this->buffer = buffer;
	this->maxlen = buflen;
	this->buflen = 0;
}
;

void STK_StreamProgrammer::setInputStream(STK_StreamWrapper * comm,
		int rstpin) {
	this->comm = comm;
	this->rstpin = rstpin;
}
;

// NOTE : use src=NULL for just checking signature
int8_t STK_StreamProgrammer::programNode(STK_FileWrapper * src) {

	init();

	unsigned long int startMS = millis();
	STK_DEBUG_PRINTLN("set get sync");
	while (!stk_getsync()) {
		purgeStream();
		if ((millis() - startMS) > 2000) {
			return (STK_PROGRAMMER_ERROR_NOSYNC);
		}
	};  // wait for a sync
	purgeStream();
	STK_DEBUG_PRINTLN("stk set params");
	if (!stk_setdeviceparameters())
		return (STK_PROGRAMMER_ERROR_NONFATAL);

	STK_DEBUG_PRINTLN("stk enter progmode");
	if (!stk_enterprogmode())
		return (STK_PROGRAMMER_ERROR_NONFATAL);

	STK_DEBUG_PRINTLN("stk get signature");
	if (stk_getsignature()) {
		if (!verifySignature(0x1E, 0x95, 0x0F)) {
			// signature is bad, no need to retry... we won't be able to do much
			// maybe a wrong experiment node ?
			return (STK_PROGRAMMER_ERROR_FATAL);
		}
	} else {
		return (STK_PROGRAMMER_ERROR_NONFATAL);
	}

	/*
	 byte _universalarray[5*4] = {
	 0xA0, 0x03, 0xFC, 0x00, 0xFF,
	 0xA0, 0x03, 0xFD, 0x00, 0xFF,
	 0xA0, 0x03, 0xFE, 0x00, 0xFF,
	 0xA0, 0x03, 0xFF, 0x00, 0xFF,
	 };

	 for (int i=0; i<4; i++) {
	 if (!stk_universal(_universalarray+5*i))
	 return(STK_PROGRAMMER_ERROR_NONFATAL);
	 }
	 */

	STK_DEBUG_PRINTLN("stk get sync 2");
	while (!stk_getsync()) {
		purgeStream();
		if ((millis() - startMS) > 2000)
			return (STK_PROGRAMMER_ERROR_NOSYNC);
	};  // wait for a sync

	if (src != NULL) {
		unsigned int t_len = src->getProgramSize();

		if (t_len == 0)
			return (STK_PROGRAMMER_ERROR_NOFILE);

		unsigned int t_remaining = t_len;
		unsigned int t_currentaddr = 0;
//        purgeStream();

		while (t_remaining > STK_PROG_PAGE_SIZE) {
			STK_DEBUG_PRINTLN("stk load addr");
			if (!stk_loadaddress(t_currentaddr)) {
				return (STK_PROGRAMMER_ERROR_DANGER);
			}
			resetBuffer();
			src->readProgramPage(buffer, t_currentaddr, STK_PROG_PAGE_SIZE);
			buflen = STK_PROG_PAGE_SIZE;
			STK_DEBUG_PRINTLN("stk prog page");
			if (!stk_progpage(buffer, buflen)) {
				return (STK_PROGRAMMER_ERROR_DANGER);
			}
			t_remaining -= STK_PROG_PAGE_SIZE;
			t_currentaddr += STK_PROG_PAGE_SIZE;
		}

		// last page with remaining bytes
		if (!stk_loadaddress(t_currentaddr))
			return (STK_PROGRAMMER_ERROR_DANGER);
		resetBuffer();
		src->readProgramPage(buffer, t_currentaddr, t_remaining);
		buflen = t_remaining;
		if (!stk_progpage(buffer, buflen))
			return (STK_PROGRAMMER_ERROR_DANGER);

		// *** VERIFICATION ***
		STK_DEBUG_PRINTLN("stk verify");
		t_remaining = t_len;
		t_currentaddr = 0;
//        purgeStream();

		while (t_remaining > STK_PROG_PAGE_SIZE) {
			if (!stk_loadaddress(t_currentaddr))
				return (STK_PROGRAMMER_ERROR_UNVERIFIED);

			resetBuffer();
			if (!stk_readpage(STK_PROG_PAGE_SIZE)) {
				return (STK_PROGRAMMER_ERROR_UNVERIFIED);
			}
			if (!src->verifyProgramPage(buffer + 1, t_currentaddr,
					buflen - 2)) {
				return (STK_PROGRAMMER_ERROR_UNVERIFIED);
			}

			t_remaining -= STK_PROG_PAGE_SIZE;
			t_currentaddr += STK_PROG_PAGE_SIZE;
		}

		// last page with remaining bytes
		if (!stk_loadaddress(t_currentaddr))
			return (STK_PROGRAMMER_ERROR_UNVERIFIED);
		resetBuffer();
		if (!stk_readpage(t_remaining))
			return (STK_PROGRAMMER_ERROR_UNVERIFIED);
		if (!src->verifyProgramPage(buffer + 1, t_currentaddr, buflen - 2))
			return (STK_PROGRAMMER_ERROR_UNVERIFIED);
	}

	if (!stk_leaveprogmode())
		return (STK_PROGRAMMER_ERROR_BUTFINE);

	return (STK_PROGRAMMER_OK);
}

void STK_StreamProgrammer::init() {
	pinMode(rstpin, OUTPUT);
	digitalWrite(rstpin, HIGH);

	STK_DELAY(500);
	comm->write(0xFF);
//  comm->flush();

	STK_DELAY(10);
	digitalWrite(rstpin, LOW);

	STK_DELAY(30);
	comm->write(0xFF);
//  comm->flush();

	STK_DELAY(70);
	digitalWrite(rstpin, HIGH);

	STK_DELAY(200);
	comm->write(0xFF);
//  comm->flush();

	STK_DELAY(70);
	digitalWrite(rstpin, LOW);

	STK_DELAY(1000);
	comm->write(STK_GET_SYNC);
	comm->write(CRC_EOP);
//	comm->flush();
	getAnswer(2, STK_OUTPUTSERIAL_TIMEOUT);
}

boolean STK_StreamProgrammer::stk_getsync() {
	//STK_DELAY(10);
	comm->write(STK_GET_SYNC);
	comm->write(CRC_EOP);
//  SerialPROG.flush();
	getAnswer(2, STK_OUTPUTSERIAL_TIMEOUT);
	return (stk_checksyncanswer());
}

boolean STK_StreamProgrammer::stk_setdeviceparameters() {
	resetBuffer();
	buffer[0] = STK_SET_DEVICE;

	STK_Parameters * params = (STK_Parameters *) (buffer + 1);
	params->devicecode = 0x86;
	params->revision = 0x00;
	params->progtype = 0x00;
	params->parmode = 0x01;
	params->polling = 0x01;
	params->selftimed = 0x01;
	params->lockbytes = 0x01;
	params->fusebytes = 0x03;
	params->flashpoll = 0xFFFF;
	params->eeprompoll = 0xFFFF;
	params->pagesize = STK_PROG_PAGE_SIZE;
	params->eepromsize = 1024;
	params->flashsize = 32768;
	_transform_LE2BE(params);

	buffer[sizeof(STK_Parameters) + 1] = CRC_EOP;

	buflen = sizeof(STK_Parameters) + 2;

	//STK_DELAY(0);
//      comm->write(STK_SET_DEVICE);
	sendBuffer();
//      comm->write(CRC_EOP);
	getAnswer(2, STK_OUTPUTSERIAL_TIMEOUT);
	return (stk_checksyncanswer());
}

boolean STK_StreamProgrammer::stk_universal(byte * sequence4bytes,
		byte answer) {
	//STK_DELAY(0);
	comm->write(STK_UNIVERSAL);
	for (int i = 0; i < 4; i++)
		comm->write(sequence4bytes[i]);
	comm->write(CRC_EOP);
	getAnswer(3, STK_OUTPUTSERIAL_TIMEOUT);
	if (!stk_checksyncanswer())
		return (false);
	if (buffer[1] != answer)
		return (false);
	return (true);
}

boolean STK_StreamProgrammer::stk_enterprogmode() {
	//STK_DELAY(0);
	comm->write(STK_ENTER_PROGMODE);
	comm->write(CRC_EOP);
//  SerialPROG.flush();
	getAnswer(2, STK_OUTPUTSERIAL_TIMEOUT);
	return (stk_checksyncanswer());
}

boolean STK_StreamProgrammer::stk_leaveprogmode() {
	//STK_DELAY(0);
	comm->write(STK_LEAVE_PROGMODE);
	comm->write(CRC_EOP);
//  SerialPROG.flush();
	getAnswer(2, STK_OUTPUTSERIAL_TIMEOUT);
	return (stk_checksyncanswer());
}

// addr is given in BYTES (not WORDS)
boolean STK_StreamProgrammer::stk_loadaddress(uint16_t addr) {
	//STK_DELAY(0);
	uint16_t t_addr = addr / 2;
	byte * t_ptr = (byte *) &t_addr;
//      comm->write(t_addr);  // TODO : big endian or little endian ?
	resetBuffer();
	buffer[0] = STK_LOAD_ADDRESS;
	buffer[1] = t_ptr[0];
	buffer[2] = t_ptr[1];
	buffer[3] = CRC_EOP;
	buflen = 4;

	sendBuffer();
	/*
	 comm->write(STK_LOAD_ADDRESS);
	 comm->write(t_ptr[0]);
	 comm->write(t_ptr[1]);
	 comm->write(CRC_EOP);
	 */
	getAnswer(2, STK_OUTPUTSERIAL_TIMEOUT);
	return (stk_checksyncanswer());
}

boolean STK_StreamProgrammer::stk_progpage(byte * page, uint16_t len) {
	//STK_DELAY(0);
	//resetBuffer();
	for (int i = 0; i < len; i++) {
		buffer[len + 3 - i] = page[len - 1 - i];
	}
//      memcpy((void*)(buffer+4), (void*)page, len);

	buffer[0] = STK_PROG_PAGE;
	byte * t_ptr = (byte *) &len;
	buffer[1] = t_ptr[1];
	buffer[2] = t_ptr[0];
	buffer[3] = STK_PAGE_FLASH;
//      for (int i=0; i<len; i++)
//        buffer[i+4] = page[i];
	buffer[len + 4] = CRC_EOP;
	buflen = len + 5;

	sendBuffer();
	/*
	 comm->write(STK_PROG_PAGE);
	 byte * t_ptr = (byte *) &len;
	 comm->write(t_ptr[1]);
	 comm->write(t_ptr[0]);
	 comm->write(STK_PAGE_FLASH);

	 for (int i=0; i<len; i++)
	 comm->write(page[i]);

	 comm->write(CRC_EOP);
	 */
	getAnswer(2, STK_OUTPUTSERIAL_TIMEOUT);
	return (stk_checksyncanswer());
}

boolean STK_StreamProgrammer::stk_readpage(unsigned int len) {
	if (len > maxlen)
		return (false);

	//STK_DELAY(0);
	resetBuffer();
	buffer[0] = STK_READ_PAGE;
	byte * t_ptr = (byte *) &len;
	buffer[1] = t_ptr[1];
	buffer[2] = t_ptr[0];
	buffer[3] = STK_PAGE_FLASH;
	buffer[4] = CRC_EOP;
	buflen = 5;
	sendBuffer();
	/*
	 comm->write(STK_READ_PAGE);
	 byte * t_ptr = (byte *)&len;
	 comm->write(t_ptr[1]);
	 comm->write(t_ptr[0]);
	 comm->write(STK_PAGE_FLASH);
	 comm->write(CRC_EOP);
	 */
	getAnswer(len + 2, 200);
	if (buflen != (len + 2))
		return (false);
	return (stk_checksyncanswer());
}

boolean STK_StreamProgrammer::stk_getsignature() {
	//STK_DELAY(0);
	comm->write(STK_READ_SIGN);
	comm->write(CRC_EOP);
//  SerialPROG.flush();

	getAnswer(5, STK_OUTPUTSERIAL_TIMEOUT);
	return (stk_checksyncanswer());
}

boolean STK_StreamProgrammer::verifyUniversal(byte b) {
	if (buflen < 3)
		return (false);
	if (buffer[1] != b)
		return (false);
	return (true);
}

boolean STK_StreamProgrammer::verifyPage(byte * page, uint16_t len) {
	if (len > (buflen + 2))
		return (false);
	for (int i = 0; i < len; i++)
		if (page[i] != buffer[i + 1]) {
			return (false);
		}
	return (true);
}

boolean STK_StreamProgrammer::verifySignature(byte a, byte b, byte c) {
	if (buflen < 4)
		return (false);
	if (buffer[1] != a)
		return (false);
	if (buffer[2] != b)
		return (false);
	if (buffer[3] != c)
		return (false);
	return (true);
}

// checks if the current answer is (STK_INSYNC) ... something ... (STK_OK)
boolean STK_StreamProgrammer::stk_checksyncanswer() {
	if (buflen < 2)
		return (false);
	if (buffer[0] != STK_INSYNC)
		return (false);
	if (buffer[buflen - 1] != STK_OK)
		return (false);
	return (true);
}

// ***********************
// *** COMM MANAGEMENT ***
// ***********************

void STK_StreamProgrammer::resetBuffer() {
	memset(buffer, 0x00, maxlen);
	buflen = 0;
}

void STK_StreamProgrammer::purgeStream() {
//	delay(STK_OUTPUTSERIAL_TIMEOUT);
	while (comm->available() > 0)
		comm->read();
}

void STK_StreamProgrammer::sendBuffer() {
	comm->send(buffer, buflen);
}

// note : timeout should be short, because it's the interval between two receive !
void STK_StreamProgrammer::getAnswer(unsigned int len,
		unsigned long int timeout) {
	resetBuffer();
	buflen = comm->fill(buffer, maxlen, len);
	STK_DEBUG_BUFFER("buffer", buffer, buflen);
}

