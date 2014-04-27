/*
 * ASIF_ThreadIDEServer.h
 *
 *  Created on: 14 avr. 2014
 *      Author: jeff
 */

#ifndef ASIF_THREADIDESERVER_H_
#define ASIF_THREADIDESERVER_H_

#include "asif_config.h"

#if (defined(ASIF_PORTID_ENODE) && defined(ASIF_PORTID_IDE))

#include "ASIF_Thread.h"
#include "ASIF_SD_filehandler.h"
#include <STK500Lib.h>
#include <STK_StreamServer.h>


class ASIF_ThreadIDEServer: public ASIF_Thread {
	byte * buffer;
	int buflen;
	STK_StreamServer stkisp;
	ASIF_SD_filehandler progfile;

public:
	void setup();
	boolean isTriggered();
	void trigger();
};

#endif /* (defined(ASIF_PORTID_ENODE) && defined(ASIF_PORTID_IDE)) */

#endif /* ASIF_THREADIDESERVER_H_ */
