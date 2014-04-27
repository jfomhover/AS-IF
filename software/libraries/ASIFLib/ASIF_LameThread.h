/*
 * ASIF_LameThread.h
 *
 *  Created on: 15 avr. 2014
 *      Author: jeff
 */

#ifndef ASIF_LAMETHREAD_H_
#define ASIF_LAMETHREAD_H_

#include "asif_config.h"
#include "asif_debug.h"
#include "ASIF_Control.h"
#include "freeMemory.h"

class ASIF_LameThread : public ASIF_Thread {
	unsigned long int timeMS;
	char * string;
public:
    void setup() {
    	string = (char*)ASIF_Control.allocBuffer(0xFF, 32);
    }
    boolean isTriggered() {
    	return(true);
    }

    void trigger() {
    	ASIF_Control.lockPort(ASIF_PORTID_DEBUG);
    	ASIF_Control.lockBuffer(0xFF);

    	timeMS = millis();
    	memset((void*)string, 0x00, 32);
    	int t_len = 0;

    	string[t_len++] = 'T';
    	string[t_len++] = '=';
    	ultoa(timeMS, string + t_len, 10);
    	t_len = strlen(string);
    	string[t_len++] = ' ';
    	string[t_len++] = 'M';
    	string[t_len++] = '=';
    	ultoa(freeMemory(), string + t_len, 10);
    	t_len = strlen(string);
//    	string[t_len++] = '\n';
    	ASIF_OUTPUT(string);
//    	asif_debugPort.print(string);
//    	ASIF_Control.cleanPrograms();

    	ASIF_Control.unlockBuffer(0xFF);
    	ASIF_Control.unlockPort(ASIF_PORTID_DEBUG);

    	ASIF_DELAY(60000);
    }
};


#endif /* ASIF_LAMETHREAD_H_ */
