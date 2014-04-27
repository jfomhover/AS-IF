/*
 * ASIF_ThreadENODEProg.h
 *
 *  Created on: 14 avr. 2014
 *      Author: jeff
 */

#ifndef ASIF_THREADENODEPROG_H_
#define ASIF_THREADENODEPROG_H_

#include "asif_config.h"

#if defined(ASIF_PORTID_ENODE)

#include "asif_debug.h"
#include "ASIF_SD_filehandler.h"
#include <STK500Lib.h>
#include <STK_StreamProgrammer.h>
#include "ASIF_Thread.h"

class ASIF_ThreadENODEProg: public ASIF_Thread {
	byte * buffer;
	int buflen;
	STK_StreamProgrammer stk_prog;
	ASIF_SD_filehandler prog_source;

public:
	void setup();
	boolean isTriggered();
	void trigger();
};

#endif /* ASIF_PORTID_ENODE */

#endif /* ASIF_THREADENODEPROG_H_ */
