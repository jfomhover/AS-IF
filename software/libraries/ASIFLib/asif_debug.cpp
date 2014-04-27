/*
 * asif_debug.cpp
 *
 *  Created on: 17 avr. 2014
 *      Author: jeff
 */


#include <Arduino.h>
#include "asif_config.h"
#include "asif_debug.h"
#include "asif_ports.h"
#include <avr/pgmspace.h>


void asif_debug_pgm(const prog_uchar *str) {
	char c;

	while((c = pgm_read_byte(str++)))
		ASIF_DEBUG_STREAM.write(c);

	ASIF_DEBUG_STREAM.write('\n');
}


void asif_debug_src(uint8_t srcid) {
	ASIF_DEBUG_STREAM.write('#');
	ASIF_DEBUG_STREAM.print(srcid);
	ASIF_DEBUG_STREAM.write(' ');
}

void asif_debug_header(uint8_t srcid) {
	ASIF_DEBUG_STREAM.write('[');
	ASIF_DEBUG_STREAM.print(millis());
	ASIF_DEBUG_STREAM.write(']');
	ASIF_DEBUG_STREAM.write(' ');
	switch(srcid) {
	case ASIF_DEBUGSRC_CTL:
		ASIF_DEBUG_STREAM.print("CTL");
		break;
	case ASIF_DEBUGSRC_IDE:
		ASIF_DEBUG_STREAM.print("IDE");
		break;
	case ASIF_DEBUGSRC_ENODE:
		ASIF_DEBUG_STREAM.print("NDE");
		break;
	case ASIF_DEBUGSRC_SENSOR:
		ASIF_DEBUG_STREAM.print("I2C");
		break;
	case ASIF_DEBUGSRC_CAMERA:
		ASIF_DEBUG_STREAM.print("CAM");
		break;
	case ASIF_DEBUGSRC_GENERAL:
		ASIF_DEBUG_STREAM.print("GEN");
		break;
	case ASIF_DEBUGSRC_WARNING:
		ASIF_DEBUG_STREAM.print("WNG");
		break;
	case ASIF_DEBUGSRC_DEBUG:
		ASIF_DEBUG_STREAM.print("DBG");
		break;
	case ASIF_DEBUGSRC_ERROR:
		ASIF_DEBUG_STREAM.print("ERR");
		break;
	default:
		ASIF_DEBUG_STREAM.print("> ");
		break;
	}
	ASIF_DEBUG_STREAM.write(' ');
	ASIF_DEBUG_STREAM.write('\t');
	ASIF_DEBUG_STREAM.flush();
}
