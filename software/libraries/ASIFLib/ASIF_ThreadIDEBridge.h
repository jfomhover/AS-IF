/*
 * ASIF_ThreadIDEBridge.h
 *
 *  Created on: 14 avr. 2014
 *      Author: jeff
 */

#ifndef ASIF_THREADIDEBRIDGE_H_
#define ASIF_THREADIDEBRIDGE_H_

#include "ASIF_Thread.h"

class ASIF_ThreadIDEBridge : public ASIF_Thread {
  byte * buffer;
  uint16_t buflen;
  int enodepin_value;
  int idepin_value;
  uint32_t timeHL;
  uint16_t currentPgm_id;

public:
  void setup();
  boolean isTriggered();
  void trigger();

private:
  boolean checkEnterProgmode();
  boolean checkLeaveProgmode();
  void prog_alignpinstate();
  boolean ide_changestate();
  void ide_alignpinstate();
  void dumpPins();
};



#endif /* ASIF_THREADIDEBRIDGE_H_ */
