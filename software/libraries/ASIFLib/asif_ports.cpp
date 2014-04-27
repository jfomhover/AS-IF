#include <Arduino.h>
#include "asif_config.h"
#include "asif_debug.h"
#include "asif_ports.h"
#include "ASIF_PortWrapper.h"
#include "ASIF_Port_I2CServer.h"
#include "ASIF_Port_Serial.h"

#ifdef ASIF_PORTID_I2C
#include <Wire.h>
#endif
#include <SdFat.h>

// ******************
// *** IDE STREAM ***
// ******************
#ifdef ASIF_PORTID_IDE
#ifndef ASIF_IDE_SERIALHARDWARE
#include <SoftwareSerial.h>
SoftwareSerial asif_ideSerial(ASIF_IDE_RX, ASIF_IDE_TX);

ASIF_Port_Serial asif_idePort(&ASIF_IDE_STREAM, ASIF_IDE_SERIAL_TIMEOUT, false, false);
#else
ASIF_Port_Serial asif_idePort(&ASIF_IDE_STREAM, ASIF_IDE_SERIAL_TIMEOUT, false, false);
#endif
#endif

boolean asif_port_init_ide() {
#ifdef ASIF_PORTID_IDE
	ASIF_IDE_STREAM.begin(ASIF_IDE_BAUDRATE);
	pinMode(ASIF_IDE_RST, INPUT);
	ASIF_DEBUG_DEBUGOP("port", "ide on");
	return (asif_idePort.start());
#else
	return(false);
#endif
}

void asif_port_halt_ide() {
#ifdef ASIF_PORTID_IDE
	ASIF_IDE_STREAM.end();
	ASIF_DEBUG_DEBUGOP("port", "ide off");
#endif
}

// ********************
// *** ENODE STREAM ***
// ********************

#ifdef ASIF_PORTID_ENODE
#ifndef ASIF_ENODE_SERIALHARDWARE
#include <SoftwareSerial.h>
SoftwareSerial asif_enodeSerial(ASIF_ENODE_RX, ASIF_ENODE_TX);

ASIF_Port_Serial asif_enodePort(&ASIF_ENODE_STREAM, ASIF_ENODE_SERIAL_TIMEOUT, false, false);
#else
ASIF_Port_Serial asif_enodePort(&ASIF_ENODE_STREAM, ASIF_ENODE_SERIAL_TIMEOUT, true, true);
#endif
#endif

boolean asif_port_init_enode() {
#ifdef ASIF_PORTID_ENODE
	ASIF_ENODE_STREAM.begin(ASIF_ENODE_BAUDRATE);
	pinMode(ASIF_ENODE_RST, OUTPUT);
	digitalWrite(ASIF_ENODE_RST, LOW);
	ASIF_DEBUG_DEBUGOP("port", "enode on");
	return (asif_enodePort.start());
#else
	return(false);
#endif
}

void asif_port_halt_enode() {
#ifdef ASIF_PORTID_ENODE
	ASIF_ENODE_STREAM.end();
#endif
}

// *********************
// *** CAMERA STREAM ***
// *********************
#ifdef ASIF_PORTID_CAMERA
#ifndef ASIF_CAMERA_SERIALHARDWARE
#include <SoftwareSerial.h>
SoftwareSerial asif_cameraSerial(ASIF_CAMERA_RX, ASIF_CAMERA_TX);

ASIF_Port_Serial asif_cameraPort(&ASIF_CAMERA_STREAM, ASIF_CAMERA_SERIAL_TIMEOUT, false);
#else
ASIF_Port_Serial asif_cameraPort(&ASIF_CAMERA_STREAM,
ASIF_CAMERA_SERIAL_TIMEOUT, true);
#endif
#endif

boolean asif_port_init_camera() {
#ifdef ASIF_PORTID_CAMERA
	ASIF_CAMERA_STREAM.begin(ASIF_CAMERA_BAUDRATE);
	ASIF_DEBUG_DEBUGOP("port", "cam on");
	return (asif_cameraPort.start());
#else
	return(false);
#endif
}

void asif_port_halt_camera() {
#ifdef ASIF_PORTID_CAMERA
	ASIF_CAMERA_STREAM.end();
#endif
}


// ********************
// *** DEBUG STREAM ***
// ********************

#if (defined(ASIF_PORTID_DEBUG) && defined(ASIF_DEBUGMODE_ON))
#ifndef ASIF_DEBUG_SERIALHARDWARE
#include <SoftwareSerial.h>
SoftwareSerial asif_debugSerial(ASIF_DEBUG_RX, ASIF_DEBUG_TX);

ASIF_Port_Serial asif_debugPort(&ASIF_DEBUG_STREAM, ASIF_DEBUG_SERIAL_TIMEOUT, false, false);
#else
ASIF_Port_Serial asif_debugPort(&ASIF_DEBUG_STREAM, ASIF_DEBUG_SERIAL_TIMEOUT, false, true);
#endif
#endif

boolean asif_port_init_debug() {
#ifdef ASIF_DEBUGMODE_ON
#ifdef ASIF_PORTID_DEBUG
	ASIF_DEBUG_STREAM.begin(ASIF_DEBUG_BAUDRATE);
//	ASIF_DEBUG_DEBUGOP("port", "cam on");
	return (asif_debugPort.start());
#endif
#else
	return(true);
#endif
}

void asif_port_halt_debug() {
#ifdef ASIF_DEBUGMODE_ON
#ifdef ASIF_PORTID_DEBUG
	ASIF_DEBUG_STREAM.end();
#endif
#endif
}

// ***************
// *** SENSORS ***
// ***************

#ifdef ASIF_PORTID_I2C
ASIF_Port_I2CServer asif_i2cPort;
#endif

boolean asif_port_init_i2c() {
#ifdef ASIF_PORTID_I2C
	ASIF_DEBUG_DEBUGOP("port", "i2c on");
	return (asif_i2cPort.start());
#else
	return(false);
#endif
}

void asif_port_halt_i2c() {
}

// ***************
// *** SD CARD ***
// ***************

#ifdef ASIF_PORTID_SPI
SdFat asif_sdPort;
#endif

boolean asif_port_init_spi() {
#ifdef ASIF_PORTID_SPI
	pinMode(ASIF_SD_CSPIN, OUTPUT);
	if (!asif_sdPort.begin(ASIF_SD_CSPIN, SPI_HALF_SPEED)) {
//      sd.initErrorHalt();
		ASIF_DEBUG_ERROR(ASIF_PORTID_SPI, "SD init failed.");
		return (false);
	} else {
		ASIF_DEBUG_GENERALOP(ASIF_PORTID_SPI, "SD init done.");
		return (true);
	}
#endif
}

void asif_port_halt_spi() {
}
