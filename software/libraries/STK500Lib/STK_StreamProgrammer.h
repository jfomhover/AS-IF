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

#ifndef _STK_STREAMPROGRAMMER_H_
#define _STK_STREAMPROGRAMMER_H_

#include "STK_Programmer.h"
#include "STK_FileWrapper.h"
#include "STK_StreamWrapper.h"

class STK_StreamProgrammer : public STK_Programmer {
  private:
	STK_StreamWrapper * comm;
    int rstpin;
    byte * buffer;
    int buflen;
    int maxlen;
    
  public:
    STK_StreamProgrammer();
    void setInputStream(STK_StreamWrapper * comm, int rstpin);
    void setBuffer(byte * buffer, int buflen);
    
#define STK_PROGRAMMER_OK                0   // everything is fine
#define STK_PROGRAMMER_ERROR_NOSYNC      -1  // can't get sync from node
#define STK_PROGRAMMER_ERROR_NONFATAL    -2  // interrupted for some reason, you can always retry...
#define STK_PROGRAMMER_ERROR_NOFILE      -3  // hex file is not available, or accessible
#define STK_PROGRAMMER_ERROR_BUTFINE     -4  // interrupted, but it should be okay
#define STK_PROGRAMMER_ERROR_UNVERIFIED  -5  // interrupted during verification
#define STK_PROGRAMMER_ERROR_FATAL       -6  // interrupted for mismatch, don't even retry
#define STK_PROGRAMMER_ERROR_DANGER      -7  // interrupted and put the whole thing danger (like, the code is half uploaded)

    // NOTE : use src=NULL for just checking signature
    int8_t programNode(STK_FileWrapper * src);
    void init();
    boolean stk_getsync();
    boolean stk_setdeviceparameters();
    boolean stk_universal(byte * sequence4bytes, byte answer);
    boolean stk_enterprogmode();
    boolean stk_leaveprogmode();
    // addr is given in BYTES (not WORDS)
    boolean stk_loadaddress(uint16_t addr);
    boolean stk_progpage(byte * page, uint16_t len);
    boolean stk_readpage(unsigned int len);
    boolean stk_getsignature();
    boolean verifyUniversal(byte b);
    boolean verifyPage(byte * page, uint16_t len);
    boolean verifySignature(byte a, byte b, byte c);
    boolean stk_checksyncanswer();

  // ***********************
  // *** COMM MANAGEMENT ***
  // ***********************

    void resetBuffer();
    void purgeStream();
    void sendBuffer();

    // note : timeout should be short, because it's the interval between two receive !
    void getAnswer(unsigned int len, unsigned long int timeout);
};

#endif /* _STK_SERIALPROGRAMMER_H_ */
