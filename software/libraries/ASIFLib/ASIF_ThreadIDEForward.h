/*
 * ASIF_ThreadIDEBridge.h
 *
 *  Created on: 14 avr. 2014
 *      Author: jeff
 */

#ifndef ASIF_THREADIDEFORWARD_H_
#define ASIF_THREADIDEFORWARD_H_

#include "asif_config.h"

#if (defined(ASIF_PORTID_ENODE) && defined(ASIF_PORTID_IDE))

#include "ASIF_Thread.h"

class ASIF_ThreadIDEForward : public ASIF_Thread {
  byte * buffer;
  uint16_t buflen;
  int enodepin_value;
  int idepin_value;
  uint16_t currentPgm_id;

public:
  void setup();
  boolean isTriggered();
  void trigger();

private:
  boolean checkEnterProgmode();
  boolean checkLeaveProgmode();
  void prog_alignpinstate();
  void flushProg();
  boolean ide_changestate();
  void ide_alignpinstate();
  void zeroBuffer();
  void fillBuffer(Stream * input, int timeout);
  void sendBuffer(Stream * output);
  void dumpPins();
  void dumpState(const char * prefix);
};


#endif /* #if defined(ASIF_PORTID_ENODE) */

#endif /* ASIF_THREADIDEFORWARD_H_ */
