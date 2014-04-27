/*
 * ASIF_ThreadI2CSensor.h
 *
 *  Created on: 14 avr. 2014
 *      Author: jeff
 */

#ifndef ASIF_THREADI2CSENSOR_H_
#define ASIF_THREADI2CSENSOR_H_

#include "ASIF_Thread.h"
#include "ASIF_SD_filehandler.h"

class ASIF_ThreadI2CSensor : public ASIF_Thread {
  byte message[32];
  ASIF_SD_filehandler outfile;

public:
  void setup();
  boolean isTriggered();
  void trigger();
};


#endif /* ASIF_THREADI2CSENSOR_H_ */
