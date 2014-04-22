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

	Description : example that programs an external arduino node
				  using an HEX file stored on SD
				  
	Last changed : April 22, 2014

********************************************************************
*/

#include <Arduino.h>
#include <SdFat.h>
#include <SoftwareSerial.h>
#include <STK500Lib.h>
#include <STK_SDFileWrapper.h>
#include <STK_StreamProgrammer.h>
#include <STK_SerialStream.h>

// below : parameters for the input serial port
//#define NODE_USESERIALHARDWARE	Serial1
#define NODE_RX        5
#define NODE_TX        6
#define NODE_RST       7
#define NODE_BAUDRATE  9600
#define NODE_BUFFERLEN 140
#define NODE_SERIALTIMEOUT	100

// chip select pin of the SD card
#define SD_CSPIN		4

STK_StreamProgrammer stkprog;
STK_SDFileWrapper file;	// internal wrapper for files (can be SD, can be something else)

SdFat sd;					// SD library class
byte buffer[NODE_BUFFERLEN];	// buffer for the stk server

#ifndef NODE_USESERIALHARDWARE
SoftwareSerial node_serial(NODE_RX, NODE_TX);
#define NODE_STREAM	node_serial
#else
#define NODE_STREAM	NODE_USESERIALHARDWARE
#endif

void setup() {
	Serial.begin(115200);

	node_serial.begin(NODE_BAUDRATE);

	// initialize the SD card at SPI_FULL_SPEED for best performance.
	// try SPI_HALF_SPEED if bus errors occur.
	if (!sd.begin(SD_CSPIN, SPI_HALF_SPEED)) {
		sd.initErrorHalt();
	} else {
		Serial.println("SD init success");
	}

	file.setFilename("TEST.HEX");

	stkprog.setBuffer(buffer, NODE_BUFFERLEN);

	// set the input stream (internal wrapper) and node RST pin
	stkprog.setInputStream(
			new STK_SerialStream((Stream *) &NODE_STREAM,	// actual stream
									NODE_SERIALTIMEOUT,		// timeout for waiting for bytes
									false),		// true = flush stream each time a byte is sent/written
			NODE_RST
			);

	Serial.println("type a key to program node");

	while (!(Serial.available() > 0));

//  delay(5000);

	Serial.println("programming...");

	uint8_t t_ret = stkprog.programNode(&file);

	Serial.print("stkprog returned : ");
	Serial.println(t_ret);
}

void loop() {

}
