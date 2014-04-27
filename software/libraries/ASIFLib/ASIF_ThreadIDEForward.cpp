#include <Arduino.h>
#include "asif_config.h"

#if (defined(ASIF_PORTID_ENODE) && defined(ASIF_PORTID_IDE))

#include "asif_debug.h"
#include "ASIF_Control.h"
#include "ASIF_SD_filehandler.h"
#include "asif_ports.h"
#include "ASIF_ThreadIDEForward.h"

#include <STK500Lib.h>
#include <stk_defs.h>

#include <avr/pgmspace.h>

const prog_uchar string_0[] PROGMEM = "idePin=HIGH";
#define PGM_STRING_IDEPINHIGH	string_0
const prog_uchar string_1[] PROGMEM = "idePin=LOW";
#define PGM_STRING_IDEPINLOW		string_1

const prog_uchar string_2[] PROGMEM = "enodePin=HIGH";
#define PGM_STRING_ENODEPINHIGH	string_2
const prog_uchar string_3[] PROGMEM = "enodePin=LOW";
#define PGM_STRING_ENODEPINLOW		string_3



void ASIF_ThreadIDEForward::setup() {
	pinMode(ASIF_IDE_RST, INPUT);
	pinMode(ASIF_ENODE_RST, OUTPUT);
	enodepin_value = HIGH;
	digitalWrite(ASIF_ENODE_RST, enodepin_value);

	idepin_value = HIGH;
	idepin_value = digitalRead(ASIF_IDE_RST);

	threadId = ASIF_THREADID_IDE;
	buffer = ASIF_Control.allocBuffer(threadId, ASIF_IDE_BUFFERLEN);
	if (buffer == NULL)
		ASIF_DEBUG_WARNING(threadId, "null buffer");
	buflen = 0;

#ifndef ASIF_IDE_SERIALHARDWARE
//    ASIF_IDE_STREAM.listen();
#endif        
//    asif_port_init_ide();
}


boolean ASIF_ThreadIDEForward::isTriggered() {
	if (buffer == NULL)
		return(false);
	if (ide_changestate())
		return (true);
	if (asif_idePort.available() > 0)
		return (true);
	if (asif_enodePort.available() > 0)
		return(true);
	return (false);
}

void ASIF_ThreadIDEForward::trigger() {
	ASIF_Control.lockBuffer(threadId);
	if (buffer == NULL)
		return;

	if (ide_changestate()) {
		ide_alignpinstate();
		prog_alignpinstate();
		dumpPins();
		asif_enodePort.flushInput();
//      return;
	}

	if (asif_idePort.available() > 0) {
//		ASIF_DEBUG_DEBUGOP(threadId, "ide port available");
		zeroBuffer();

//		ASIF_DEBUG_DEBUGOP(threadId, "ide port fill");
		buflen = asif_idePort.fill(buffer, ASIF_IDE_BUFFERLEN);

		if (buflen > 0) {
			// WE RECEIVED SOME DATA FROM IDE !!!
			ASIF_DEBUG_DEBUGOP(threadId, "ide data recd");
			if (checkEnterProgmode()) {
				currentPgm_id = ASIF_FS.allocFile(threadId);
				if (currentPgm_id == ASIF_PGMID_NULL) {
					ASIF_Control.unlockBuffer(threadId);
					return;
				}
			}
			if (checkLeaveProgmode()) {
				ASIF_Control.pushProgram(threadId, currentPgm_id);
			}


			dumpState("ide->prog:");

			ASIF_DEBUG_DEBUGOP(threadId, "ide data send");
			asif_enodePort.send(buffer, buflen); //fillBuffer(&ASIF_IDE_STREAM, 10);

			if (ide_changestate()) {
				ide_alignpinstate();
				prog_alignpinstate();
				dumpPins();
			}
		}
	}
/*
	else {
		asif_enodePort.flushInput();
		ASIF_Control.unlockBuffer(threadId);
		return;
	}
*/

	if (asif_enodePort.available() > 0) {
		zeroBuffer();

		buflen = asif_enodePort.fill(buffer, ASIF_ENODE_BUFFERLEN); // fillBuffer(&ASIF_ENODE_STREAM, 10);

		if (buflen > 0) {
			dumpState("prog->ide:");

			asif_idePort.send(buffer, buflen);
		}
	}

	ASIF_Control.unlockBuffer(threadId);
}

boolean ASIF_ThreadIDEForward::checkEnterProgmode() {
	if (buffer == NULL)
		return(false);
	return (buffer[0] == STK_ENTER_PROGMODE);
}

boolean ASIF_ThreadIDEForward::checkLeaveProgmode() {
	if (buffer == NULL)
		return(false);
	return (buffer[0] == STK_LEAVE_PROGMODE);
}




void ASIF_ThreadIDEForward::prog_alignpinstate() {
	if (idepin_value != enodepin_value) {
		digitalWrite(ASIF_ENODE_RST, idepin_value);
		enodepin_value = idepin_value;
	}
}


boolean ASIF_ThreadIDEForward::ide_changestate() {
	int t_val = digitalRead(ASIF_IDE_RST);

	if (t_val != idepin_value) {
		//    idepin_value = t_val;
		return (true);
	}
	return (false);
}


void ASIF_ThreadIDEForward::ide_alignpinstate() {
	idepin_value = digitalRead(ASIF_IDE_RST);
}


void ASIF_ThreadIDEForward::zeroBuffer() {
	if (buffer == NULL)
		return;
	memset(buffer, 0x00, ASIF_IDE_BUFFERLEN);
	buflen = 0;
}


void ASIF_ThreadIDEForward::dumpPins() {
	if (idepin_value == HIGH) {
		ASIF_DEBUGPGM_IDE_LLOP(PGM_STRING_IDEPINHIGH);
	} else {
		ASIF_DEBUGPGM_IDE_LLOP(PGM_STRING_IDEPINLOW);
	}
	if (enodepin_value == HIGH) {
		ASIF_DEBUGPGM_IDE_LLOP(PGM_STRING_ENODEPINHIGH);
	} else {
		ASIF_DEBUGPGM_IDE_LLOP(PGM_STRING_ENODEPINLOW);
	}
}

void ASIF_ThreadIDEForward::dumpState(const char * prefix) {
	if (buffer == NULL)
		return;

	ASIF_DEBUG_IDE_LLOP(prefix, buflen);
	ASIF_DEBUG_DUMP("buf= ", buffer, buflen);
}


#endif /* (defined(ASIF_PORTID_ENODE) && defined(ASIF_PORTID_IDE)) */
