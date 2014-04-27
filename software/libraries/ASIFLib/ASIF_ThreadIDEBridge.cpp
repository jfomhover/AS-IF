#include <Arduino.h>
#include "asif_config.h"
#include "asif_debug.h"
#include "ASIF_Control.h"
#include "ASIF_SD_filehandler.h"
#include "asif_ports.h"
#include "ASIF_ThreadIDEBridge.h"

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



void ASIF_ThreadIDEBridge::setup() {
	pinMode(ASIF_IDE_RST, INPUT);
	pinMode(ASIF_ENODE_RST, OUTPUT);
	enodepin_value = HIGH;
	digitalWrite(ASIF_ENODE_RST, enodepin_value);

	idepin_value = HIGH;
	idepin_value = digitalRead(ASIF_IDE_RST);

	threadId = ASIF_THREADID_IDE;
	timeHL = millis();
}


boolean ASIF_ThreadIDEBridge::isTriggered() {
	return(true);
}

void ASIF_ThreadIDEBridge::trigger() {
	int t_idepin_value = digitalRead(ASIF_IDE_RST);

	if (t_idepin_value == idepin_value) {
		return;
	}

	if ((t_idepin_value == LOW) && (idepin_value == HIGH)) {
		timeHL = millis();
		idepin_value = t_idepin_value;
		enodepin_value = idepin_value;
		digitalWrite(ASIF_ENODE_RST, enodepin_value);
		dumpPins();
		return;
	}

	if ((t_idepin_value == HIGH) && (idepin_value == LOW)) {
		// going back on HIGH
		idepin_value = t_idepin_value;
		enodepin_value = idepin_value;
		digitalWrite(ASIF_ENODE_RST, enodepin_value);
		dumpPins();

		if ((millis() - timeHL) > 500) {
			// if time spent is above 0.5s, it seems we're facing an upload
			currentPgm_id = ASIF_FS.allocFile(threadId);
			ASIF_Control.pushProgram(threadId, currentPgm_id);
			timeHL = millis();
		}
	}
}

void ASIF_ThreadIDEBridge::dumpPins() {
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



