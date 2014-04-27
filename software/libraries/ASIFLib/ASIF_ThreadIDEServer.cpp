/*
 * ASIF_ThreadIDEServer.cpp
 *
 *  Created on: 14 avr. 2014
 *      Author: jeff
 */

#include <Arduino.h>
#include "asif_config.h"

#if (defined(ASIF_PORTID_ENODE) && defined(ASIF_PORTID_IDE))

#include "ASIF_ThreadIDEServer.h"
#include "asif_debug.h"
#include "ASIF_Thread.h"
#include "ASIF_Control.h"
#include "ASIF_SD_filehandler.h"
#include <STK500Lib.h>
#include <STK_StreamServer.h>

void ASIF_ThreadIDEServer::setup() {
	threadId = ASIF_THREADID_IDE;
	buffer = ASIF_Control.allocBuffer(threadId, ASIF_IDE_BUFFERLEN);
	if (buffer == NULL)
		ASIF_DEBUG_WARNING(threadId, "null buffer");
	buflen = ASIF_IDE_BUFFERLEN;

	progfile.setSourceId(threadId);

	stkisp.setBuffer(buffer, buflen);
	stkisp.setInputStream(&asif_idePort);
	stkisp.setOutputFile(&progfile); // TODO
}

boolean ASIF_ThreadIDEServer::isTriggered() {
	if (buffer == NULL)
		return (false);
	return (asif_idePort.available() > 0);
}

void ASIF_ThreadIDEServer::trigger() {
	if (buffer == NULL)
		return;

	ASIF_Control.lockPort(ASIF_PORTID_IDE);
	if (isTriggered()) {
		ASIF_Control.lockBuffer(threadId);
		byte b = asif_idePort.read();
		if (b == STK_ENTER_PROGMODE) {
			stkisp.onCommand(b);

			// hook up
			while (b != STK_LEAVE_PROGMODE) {
				if (isTriggered()) {
					b = asif_idePort.read();
					stkisp.onCommand(b);
				}
			}
		} else {
			stkisp.onCommand(b);
		}

		ASIF_Control.unlockBuffer(threadId);
	}
	ASIF_Control.unlockPort(ASIF_PORTID_IDE);
}

#endif /* (defined(ASIF_PORTID_ENODE) && defined(ASIF_PORTID_IDE)) */
