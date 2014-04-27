/*
 * ASIF_ThreadCamera.h
 *
 *  Created on: 15 avr. 2014
 *      Author: jeff
 */

#ifndef ASIF_THREADCAMERA_H_
#define ASIF_THREADCAMERA_H_

#ifdef ASIF_PORTID_CAMERA

#include "ASIF_Thread.h"
#include <ZMCameraLib.h>

class ASIF_ThreadCamera : public ASIF_Thread {
  public:
    void setup();
    boolean isTriggered();
    void trigger();
};

#endif /* ASIF_PORTID_CAMERA */

#endif /* ASIF_THREADCAMERA_H_ */
