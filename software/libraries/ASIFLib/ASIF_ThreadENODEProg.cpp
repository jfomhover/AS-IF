/*
 * ASIF_ThreadENODEProg.cpp
 *
 *  Created on: 14 avr. 2014
 *      Author: jeff
 */

#include <Arduino.h>
#include "asif_config.h"

#if defined(ASIF_PORTID_ENODE)

#include "asif_debug.h"
#include "ASIF_SD_filehandler.h"
#include <STK500Lib.h>
#include <STK_StreamServer.h>
#include <STK_StreamProgrammer.h>
#include "ASIF_Control.h"
#include "ASIF_ThreadENODEProg.h"

void ASIF_ThreadENODEProg::setup() {
	threadId = ASIF_THREADID_ENODE;
	buffer = ASIF_Control.allocBuffer(threadId, ASIF_ENODE_BUFFERLEN);
	buflen = ASIF_ENODE_BUFFERLEN;

	if (buffer == NULL)
		ASIF_DEBUG_WARNING(threadId, "null buffer");

	stk_prog.setBuffer(buffer, buflen);
	stk_prog.setInputStream(&asif_enodePort, ASIF_ENODE_RST);
}

boolean ASIF_ThreadENODEProg::isTriggered() {
	if (buffer == NULL)
		return(false);

	// check whether there's a program to upload
	return (ASIF_Control.hasProgram());
}

void ASIF_ThreadENODEProg::trigger() {
	if (isTriggered()) {
		uint16_t t_pgmid = ASIF_Control.popProgram();
		if (t_pgmid == ASIF_PGMID_NULL)
			return;

		ASIF_DEBUG_DEBUGOP("pop", t_pgmid);
		ASIF_DEBUG_STREAM_FLUSH();

		prog_source.setSourceId(threadId);
		prog_source.setFileId(t_pgmid);

		ASIF_Control.lockPort(ASIF_PORTID_ENODE);
		uint16_t t_size = 0;
		if ((t_size = prog_source.getProgramSize()) > 0) {
			ASIF_DEBUG_DEBUGOP("prog", t_size);
			ASIF_DEBUG_STREAM_FLUSH();

			ASIF_Control.lockBuffer(threadId);
			int8_t t_ret = -1;
			for (int i=0; i<5; i++) {
				if (t_ret < 0)
					t_ret = stk_prog.programNode(&prog_source);
				if (t_ret < 0)
					ASIF_DELAY(5000);
			}
			ASIF_Control.unlockBuffer(threadId);

			if (t_ret < 0)  // TODO : error handling
					{
//          t_pgm->state = PGM_ERROR_UPLOADING;
				ASIF_DEBUG_ERROR(threadId, t_ret);
				ASIF_Control.endProgram(threadId, t_pgmid);
			} else {
				ASIF_Control.beginProgram(threadId, t_pgmid);
//          t_pgm->state = PGM_RUNNING;
			}
		}

		ASIF_Control.unlockPort(ASIF_PORTID_ENODE);
	}
}

#endif /* defined(ASIF_PORTID_ENODE) */
