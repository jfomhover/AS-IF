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

	Description : example that receives uploaded sketches from arduino IDE
				  store the HEX file on SD
				  
	Last changed : April 22, 2014

********************************************************************
*/

#include <Arduino.h>
#include <SdFat.h>
#include <SoftwareSerial.h>
#include <STK500Lib.h>
#include <STK_SDFileWrapper.h>
#include <STK_StreamServer.h>
#include <STK_SerialStream.h>

// below : parameters for the input serial port
//#define IDE_USESERIALHARDWARE	Serial1
#define IDE_RX        5
#define IDE_TX        6
#define IDE_RST       7
#define IDE_BAUDRATE  9600
#define IDE_BUFFERLEN 140
#define IDE_SERIALTIMEOUT	100

// chip select pin of the SD card
#define SD_CSPIN		4

STK_StreamServer stkserver;	// server that listens on serial port for STK formatted upload of a sketch
STK_SDFileWrapper file;		// internal wrapper for files (can be SD, can be something else)

SdFat sd;					// SD library class
byte buffer[IDE_BUFFERLEN];	// buffer for the stk server

#ifndef IDE_USESERIALHARDWARE
SoftwareSerial ide_serial(IDE_RX, IDE_TX);
#define IDE_STREAM	ide_serial
#else
#define IDE_STREAM	IDE_USESERIALHARDWARE
#endif

void setup() {
	Serial.begin(9600);

	IDE_STREAM.begin(IDE_BAUDRATE);

	// initialize the SD card at SPI_FULL_SPEED for best performance.
	// try SPI_HALF_SPEED if bus errors occur.
	if (!sd.begin(SD_CSPIN, SPI_HALF_SPEED)) {
		sd.initErrorHalt();
	} else {
		Serial.println("SD init success");
	}

	// set the filename of the output file (internal wrapper)
	file.setFilename("TEST.HEX");

	// set the buffer of the stk server
	stkserver.setBuffer(buffer, IDE_BUFFERLEN);
	
	// set the input stream (internal wrapper)
	stkserver.setInputStream(
			new STK_SerialStream((Stream *) &IDE_STREAM,	// actual stream
									IDE_SERIALTIMEOUT,		// timeout for waiting for bytes
									true)					// true = flush stream each time a byte is sent/written
	);
	
	// set the output file (wrapper) for the stk server
	stkserver.setOutputFile(&file);
}

void loop() {
	if (ide_serial.available() > 0) {
		stkserver.onCommand(ide_serial.read());
	}
}
