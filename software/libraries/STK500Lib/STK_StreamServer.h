/*
********************************************************************

Copyright 2014, Jean-François Omhover (jf.omhover@gmail.com, twitter @jfomhover)

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

	Description : handles stk500 communication via serial
				  to receive uploaded sketches from arduino IDE
				  store the HEX file on SD
				  use that HEX file to program an external arduino node
	Last changed : April 22, 2014

********************************************************************
*/

#ifndef _AVRISP_SERVER_H_
#define _AVRISP_SERVER_H_

#include <Arduino.h>
#include "STK500Lib.h"
#include "stk_defs.h"
#include "STK_FileWrapper.h"
#include "STK_StreamWrapper.h"

//#define STK_IGNORE_PARAMETERS

enum _stk_upload_state {
	STK_UPLOAD_STARTED,
	STK_UPLOAD_RECEIVED,
	STK_UPLOAD_VERIFIED,
	STK_UPLOAD_CLOSED
};

class STK_StreamServer {  
private:
  byte * buff;  // yes, buffer is provided by the user of this class !
  int buflen;
  int maxlen;

  int error;
  boolean progmode;
  _stk_upload_state state;
  uint16_t current_addr;
#ifndef STK_IGNORE_PARAMETERS
  STK_Parameters params;
#endif
  STK_StreamWrapper * input;
  STK_FileWrapper * outputFile;
//  STK_Programmer * outputProg;
//  STK_UniversalHandler * universalHdl;
  
public:
  STK_StreamServer();
  void setBuffer(byte * buffer, int buflen);
  void setInputStream(STK_StreamWrapper * input);
  void setOutputFile(STK_FileWrapper * output);
//  void setOutputProgrammer(STK_Programmer * output);
//  void setUniversalEmulator(STK_UniversalHandler * emulator);
  void onCommand(uint8_t cmd);
  void resetBuffer();

private:
  // *** COMMANDS ***
  void onGetSync(); // OK
  void onGetSignOn(); // OK
  void onGetParameter(); // OK
  void onSetDevice(); // OK
  void onSetDeviceExt(); // TODO
  void onEnterProgmode(); // OK
  void onLeaveProgmode(); // OK
  void onLoadAddress(); // OK
  void onProgFlash(); // OK
  void onProgData(); // OK
  void onProgPage(); // OK
  void onReadPage(); // TODO
  void onUniversal(); // OK
  void onReadSign(); // OK
  void onUnknown(uint8_t cmd); // OK

  // *** REPLIES ***
  void reply_empty();
  void reply_byte(uint8_t b);
  void reply_nosync();

  // *** COMM with Stream ***
  void fill(int n, unsigned long int timeout = STK_INPUTSERIAL_TIMEOUT);
  uint8_t get8(unsigned long int timeout = STK_INPUTSERIAL_TIMEOUT);
  void get16(uint16_t * destination, boolean bigendian);
  void get16(uint16_t * destination, uint8_t * source, int offset, boolean bigendian);  
  void get32(uint32_t * destination, boolean bigendian);
  void get32(uint32_t * destination, uint8_t * source, int offset, boolean bigendian);
};


#endif /* _AVRISP_SERVER_H_ */
