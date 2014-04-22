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

#include "STK500Lib.h"
#include "STK_StreamServer.h"
#include "stk_universal.h"
#include "stk_defs.h"

STK_StreamServer::STK_StreamServer() {
	this->buff = NULL;
	this->maxlen = 0;
	this->buflen = 0;

	this->input = NULL;
	this->outputFile = NULL;
//    this->outputProg = NULL;
//    this->universalHdl = NULL;

	error = 0;
	progmode = false;
	current_addr = 0;
}


void STK_StreamServer::setBuffer(byte * buffer, int buflen) {
	this->buff = buffer;
	this->maxlen = buflen;
	this->buflen = 0;
}

void STK_StreamServer::resetBuffer() {
	memset(buff, 0x00, maxlen);
	buflen = 0;
}

void STK_StreamServer::setInputStream(STK_StreamWrapper * input) {
	this->input = input;
}


//  void STK_StreamServer::setoutputFileWriter(STK_outputFileWriter * outputFile) {
void STK_StreamServer::setOutputFile(STK_FileWrapper * outputFile) {
	this->outputFile = outputFile;
}


//  void STK_StreamServer::setOutputProgrammer(STK_Programmer * outputProg) {
//    this->outputProg = outputProg;
//  };

//  void STK_StreamServer::setUniversalEmulator(STK_UniversalHandler * emulator) {
//    this->universalHdl = emulator;
//  };

void STK_StreamServer::onCommand(uint8_t cmd) {
	resetBuffer();

	STK_DEBUG_PRINT("onCommand:");
	STK_DEBUG_PRINTLN2(cmd, HEX);

	switch (cmd) {
	case STK_GET_SYNC: //'0'
		onGetSync();
		break;
	case STK_GET_SIGN_ON: //'1'
		state = STK_UPLOAD_STARTED;
		onGetSignOn();
		break;

	case STK_GET_PARAMETER: //'A'
		onGetParameter();
		break;
	case STK_SET_DEVICE: //'B'
		onSetDevice();
		break;
	case STK_SET_DEVICE_EXT: //'E'
		onSetDeviceExt();
		break;

	case STK_ENTER_PROGMODE: //'P'
		state = STK_UPLOAD_STARTED;
		onEnterProgmode();
		break;
	case STK_LEAVE_PROGMODE: //'Q'
		onLeaveProgmode();
		break;
	case STK_LOAD_ADDRESS: //'U'
		onLoadAddress();
		break;

	case STK_PROG_FLASH: // 0x60
		onProgFlash();
		break;
	case STK_PROG_DATA: //0x61
		onProgData();
		break;
	case STK_PROG_PAGE: //0x64
		onProgPage();
//    program_page();
		break;
	case STK_READ_PAGE: // 't'
		onReadPage();
//    read_page();    
		break;

	case STK_UNIVERSAL: //'V'
		onUniversal();
		break;

	case STK_READ_SIGN: // 'u'
		onReadSign();
//    read_signature();
		break;

		// expecting a command, not CRC_EOP
		// this is how we can get back in sync
	case CRC_EOP:
		error++;
		input->write(STK_NOSYNC);
		break;

		// anything else we will return STK_UNKNOWN
	default:
		onUnknown(cmd);
	}
}


// ****************
// *** COMMANDS ***
// ****************

void STK_StreamServer::onGetSync() {
	error = 0;
	reply_empty();
}
;

void STK_StreamServer::onGetSignOn() {
	if (get8() == CRC_EOP) {
		input->write(STK_INSYNC);
		input->print("AVR ISP");
		input->write(STK_OK);
	}
}
;

void STK_StreamServer::onGetParameter() {
	uint8_t c = input->read();
	switch (c) {
	case 0x80:
		reply_byte(STK_HWVER);
		break;
	case 0x81:
		reply_byte(STK_SWMAJ);
		break;
	case 0x82:
		reply_byte(STK_SWMIN);
		break;
	case 0x93:
		reply_byte('S'); // serial programmer
		break;
	default:
		reply_byte(0);
	}
}
;

void STK_StreamServer::onSetDevice() {
	buflen = input->fill(buff, maxlen, 20);
//	fill(20);

	STK_DEBUG_BUFFER(" buff", buff, buflen);

#ifndef STK_IGNORE_PARAMETERS
	// call this after reading paramter packet into buff[]
	params.devicecode = buff[0];
	params.revision = buff[1];
	params.progtype = buff[2];
	params.parmode = buff[3];
	params.polling = buff[4];
	params.selftimed = buff[5];
	params.lockbytes = buff[6];
	params.fusebytes = buff[7];
	params.flashpoll = buff[8];
	// ignore buff[9] (= buff[8])
	// following are 16 bits (big endian)
	get16(&(params.eeprompoll), buff, 10, true);
	get16(&(params.pagesize), buff, 12, true);
	get16(&(params.eepromsize), buff, 14, true);
//    params.eeprompoll = beget16(&buff[10]);
//    params.pagesize   = beget16(&buff[12]);
//    params.eepromsize = beget16(&buff[14]);

	// 32 bits flashsize (big endian)
	params.flashsize = buff[16] * 0x01000000 + buff[17] * 0x00010000
			+ buff[18] * 0x00000100 + buff[19];
#endif

//    outputFile->onParameters(&params);  // TODO JFO

	reply_empty();
}

void STK_StreamServer::onEnterProgmode() {
	if (outputFile->createProgram()) {
		reply_empty();
	} else {
		input->read();
//		get8();
		input->write(STK_FAILED);
		STK_DEBUG_PRINT("STK_FAILED\n");
	}
}


void STK_StreamServer::onLeaveProgmode() {
	error = 0;
	outputFile->closeProgram();
	reply_empty();
}


void STK_StreamServer::onLoadAddress() {
	buflen = input->fill(buff, maxlen, 3);
//	fill(3);

	STK_DEBUG_BUFFER(" buff", buff, buflen);

	current_addr = buff[0] + 256 * buff[1];
	if (CRC_EOP == buff[2]) {
		input->write((uint8_t) STK_INSYNC);
		input->write((uint8_t) STK_OK);
		STK_DEBUG_PRINT("STK_OK\n");
	} else {
		error++;
		input->write((uint8_t) STK_NOSYNC);
		STK_DEBUG_PRINT("STK_NOSYNC[");
		STK_DEBUG_PRINT2(buff[2], HEX);
		STK_DEBUG_PRINTLN("]");
	}
	//get16(&current_addr, false);  // TODO : little endian ? really ?
	current_addr = current_addr * 2;
//    reply_empty();
}
;

void STK_StreamServer::onUniversal() {
	buflen = input->fill(buff, maxlen, 4);
//	fill(4);

	STK_DEBUG_BUFFER(" buff", buff, buflen);
	// TODO      outputFile->onUniversal(buff);
	byte b = STK_request_universal(buff);
	reply_byte(b);
}
;

void STK_StreamServer::onReadSign() {
	if (CRC_EOP != input->read()) {
		error++;
		input->write(STK_NOSYNC);
		return;
	}
	buff[0] = STK_INSYNC;
	buff[3] = 0x0F;
	buff[2] = 0x95;
	buff[1] = 0x1E;
	buff[4] = STK_OK;
	STK_DEBUG_BUFFER("buff", buff, 5);

	input->send(buff, 5);
//	for (int i = 0; i < 5; i++)
//		input->write(buff[i]);
}
;

void STK_StreamServer::onReadPage() {
	uint8_t result = STK_FAILED;

	buflen = input->fill(buff, maxlen, 4);
	STK_DEBUG_BUFFER("buff", buff, buflen);

	uint16_t length;
	get16(&length, buff, 0, true);
	//	get16(&length, true);

	uint8_t memtype = buff[2];

	if (buff[3] != CRC_EOP) {
		error++;
		input->write(STK_NOSYNC);
		return;
	}


	if (memtype == 'F') {
		resetBuffer();

//		buff[0] = STK_INSYNC;
		input->write(STK_INSYNC);

		outputFile->readProgramPage(buff, current_addr, length);
		input->send(buff, length);

		input->write(STK_OK);
//		buff[length+1] = STK_OK;

		STK_DEBUG_BUFFER("buff", buff, length);
	} else {
		error++;
		input->write(STK_FAILED);
	}
//    if (memtype == 'E') result = eeprom_read_page(length);
}

void STK_StreamServer::onProgFlash() {
	uint16_t length;
	get16(&length, true);
	reply_empty();
}
;

void STK_StreamServer::onProgPage() {

	buflen = input->fill(buff, maxlen, STK_PROG_PAGE_SIZE+3);

	uint16_t length;

	get16(&length, buff, 0, true);

	uint8_t memtype = buff[2];

	//fill(length + 1);

	STK_DEBUG_PRINT("addr=");
	STK_DEBUG_PRINT(current_addr);
	STK_DEBUG_BUFFER(" buff", buff, buflen);

	if (buflen < (length+3)) {
		error++;
		input->write(STK_NOSYNC);
		STK_DEBUG_PRINT("STK_NOSYNC (recd ");
		STK_DEBUG_PRINT(buflen);
		STK_DEBUG_PRINTLN("/");
		STK_DEBUG_PRINT(length);
		STK_DEBUG_PRINTLN(")");
		STK_DEBUG_BUFFER(" buff", buff, buflen);
		return;
	}

	if (length == STK_PROG_PAGE_SIZE) {
		if ((input->read() & 0x00FF) != CRC_EOP) {
			error++;
			input->write(STK_NOSYNC);
			STK_DEBUG_PRINTLN("STK_NOSYNC (crc_eop)");
			STK_DEBUG_BUFFER(" buff", buff, buflen);
			return;
		}
	} else {
		if (buff[buflen-1] != CRC_EOP) {
			error++;
			input->write(STK_NOSYNC);
			STK_DEBUG_PRINTLN("STK_NOSYNC (crc_eop)");
			STK_DEBUG_BUFFER(" buff", buff, buflen);
			return;
		}
	}



	STK_DEBUG_PRINT("memtype=");
	STK_DEBUG_PRINTLN(memtype);


	// flash memory @here, (length) bytes
	if (memtype == 'F') {
		uint32_t t_len = outputFile->writeProgramPage(buff+3, current_addr, length);

		if (t_len < length) {
			error++;
			input->write(STK_FAILED);
			STK_DEBUG_PRINT("STK_NOSYNC (write error ");
			STK_DEBUG_PRINT(t_len);
			STK_DEBUG_PRINT("/");
			STK_DEBUG_PRINT(length);
			STK_DEBUG_PRINTLN(")");
			return;
		}

		input->write(STK_INSYNC);
		input->write(STK_OK);
		STK_DEBUG_PRINTLN("STK_OK");
		return;
	}

	if (memtype == 'E') {
		error++;
		input->write(STK_NOSYNC);
		STK_DEBUG_PRINTLN("STK_NOSYNC (eeprom)");
		return;
	}
}
;

void STK_StreamServer::onSetDeviceExt() {
	input->fill(buff, maxlen, 5);
//	fill(5);

	STK_DEBUG_BUFFER(" buff", buff, buflen);
	reply_empty();
}
;

void STK_StreamServer::onProgData() {
}
;

void STK_StreamServer::onUnknown(uint8_t cmd) {
	error++;
	if (CRC_EOP == input->read())
		input->write(STK_UNKNOWN);
	else
		input->write(STK_NOSYNC);
}


// ***************
// *** REPLIES ***
// ***************

void STK_StreamServer::reply_empty() {
	byte b = input->read();
	if (CRC_EOP == b) {
		input->write((uint8_t) STK_INSYNC);
		input->write((uint8_t) STK_OK);
		STK_DEBUG_PRINT("STK_OK\n");
	} else {
		error++;
		input->write((uint8_t) STK_NOSYNC);
		STK_DEBUG_PRINT("STK_NOSYNC[");
		STK_DEBUG_PRINT2(b, HEX);
		STK_DEBUG_PRINTLN("]");
	}
}
;

void STK_StreamServer::reply_byte(uint8_t b) {
	if (CRC_EOP == input->read()) {
		input->write(STK_INSYNC);
		input->write(b);
		input->write(STK_OK);
	} else {
		error++;
		input->write(STK_NOSYNC);
	}
}

void STK_StreamServer::reply_nosync() {
	if (get8() != CRC_EOP)
		error++;
	input->write(STK_NOSYNC);
}

// ************************
// *** COMM with Stream ***
// ************************

uint8_t STK_StreamServer::get8(unsigned long int timeout) {
	return(input->read());
}

void STK_StreamServer::get16(uint16_t * destination, boolean bigendian) {
	if (input->available() < 2)
		return;

	uint8_t * t_ptr = (uint8_t *) destination;
	if (bigendian) {
		t_ptr[1] = input->read();
		t_ptr[0] = input->read();
	} else {
		t_ptr[0] = input->read();
		t_ptr[1] = input->read();
	}
}

void STK_StreamServer::get16(uint16_t * destination, uint8_t * source,
		int offset, boolean bigendian) {
	uint8_t * t_ptr = (uint8_t *) destination;
	if (bigendian) {
		t_ptr[0] = source[offset + 1];
		t_ptr[1] = source[offset];
	} else {
		t_ptr[0] = source[offset];
		t_ptr[1] = source[offset + 1];
	}
}
;

void STK_StreamServer::get32(uint32_t * destination, boolean bigendian) {
	if (input->available() < 4)
		return;

	uint8_t * t_ptr = (uint8_t *) destination;
	if (bigendian) {
		t_ptr[3] = input->read();
		t_ptr[2] = input->read();
		t_ptr[1] = input->read();
		t_ptr[0] = input->read();
	} else {
		t_ptr[0] = input->read();
		t_ptr[1] = input->read();
		t_ptr[2] = input->read();
		t_ptr[3] = input->read();
	}
}

void STK_StreamServer::get32(uint32_t * destination, uint8_t * source,
		int offset, boolean bigendian) {
	uint8_t * t_ptr = (uint8_t *) destination;
	if (bigendian) {
		t_ptr[0] = source[offset + 3];
		t_ptr[1] = source[offset + 2];
		t_ptr[2] = source[offset + 1];
		t_ptr[3] = source[offset];
	} else {
		t_ptr[0] = source[offset];
		t_ptr[1] = source[offset + 1];
		t_ptr[2] = source[offset + 2];
		t_ptr[3] = source[offset + 3];
	}
}
