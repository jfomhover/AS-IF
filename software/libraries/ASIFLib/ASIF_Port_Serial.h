/*
********************************************************************

Copyright 2014, Jean-François Omhover (jf.omhover@gmail.com) and Nanosatisfi, Inc. (www.nanosatisfi.com)

********************************************************************
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
********************************************************************

    Description : classes for the manipulation of ports for AS-IF
                  this is necessary because different port can need different processing strategies
                  (timeouts, remove leading zeros...)
                  
    Last changed : April 13th, 2014
    
********************************************************************
*/

#ifndef _ASIF_PORTCLASSES_H_
#define _ASIF_PORTCLASSES_H_

#include "ASIF_PortWrapper.h"
#include <STK500Lib.h>
#include <STK_StreamWrapper.h>

// *** SERIAL COMMUNICATION ***
// note : input AND output

class ASIF_Port_Serial : public ASIF_PortWrapper, public STK_StreamWrapper {
private:
  Stream * input;
  uint32_t timeout;
  boolean leading0;
  boolean alwaysflush;
  byte currentByte;

public:
  ASIF_Port_Serial(Stream * input, uint32_t timeout, boolean alwaysflush, boolean leading0);
  boolean start();
  void halt();
  uint16_t available();
  void flushInput();
  void flushOutput();
  int16_t read();
  uint16_t fill(uint8_t * buffer, uint16_t maxlen);
  uint16_t fill(uint8_t * buffer, uint16_t maxlen, uint16_t expectedLen);
  void write(uint8_t b);
  void print(const char * str);
  void send(uint8_t * buffer, uint16_t length);
};



#endif /* _ASIF_PORTCLASSES_H_ */
